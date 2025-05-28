#include "esp_camera.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Configurações da câmera (conforme pinos do ESP32-CAM AI-Thinker)
#define PWDN_GPIO_NUM     26
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      32
#define SIOD_GPIO_NUM      13
#define SIOC_GPIO_NUM      12
#define Y9_GPIO_NUM        39
#define Y8_GPIO_NUM        36
#define Y7_GPIO_NUM        23
#define Y6_GPIO_NUM        18
#define Y5_GPIO_NUM        15
#define Y4_GPIO_NUM         4
#define Y3_GPIO_NUM        14
#define Y2_GPIO_NUM         5
#define VSYNC_GPIO_NUM     27
#define HREF_GPIO_NUM      25
#define PCLK_GPIO_NUM      19

// Configurações OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pinos
#define PIR_PIN 33
#define BUTTON_PIN 34

// NOTA IMPORTANTE PARA UPLOAD:
// Para fazer o upload do código, conecte o pino GPIO 0 ao GND antes de pressionar o botão Reset.
// Após o upload, desconecte o GPIO 0 do GND para executar o código normalmente.

// Configurações de rede
const char* ssid = "SEU_WIFI_SSID"; // Substitua pelo seu SSID
const char* password = "SUA_SENHA_WIFI"; // Substitua pela sua senha
IPAddress local_IP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

// Configurações Discord
const char* discordWebhook = "https://discord.com/api/webhooks/1374892740776689684/qZdTQyp5pv3oK4i1NhB94KBcK1mwf4IWvE-UPKp70w0_4CTLXkslwqyrbNro5dhoWSI8";
String discordMessage = "@everyone Movimento detectado!";

// Variáveis globais
unsigned long lastDetectionTime = 0;
bool motionDetected = false;
bool streamEnabled = false;
const long detectionInterval = 30000; // 30 segundos entre detecções

// Servidor Web Assíncrono
AsyncWebServer server(80);
AsyncEventSource events("/events");

// Buffer para logs
String systemLogs;

// Protótipos das funções
void setupCamera();
void displayStatus(String message);
void addLog(String message);
void sendDiscordAlert();
void captureAndSendPhoto();
void setupWebServer();

// Declaração antecipada da página HTML
const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Monitoramento ESP32</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 20px;
            background-color: #f5f5f5;
        }
        .container {
            max-width: 800px;
            margin: 0 auto;
            background-color: white;
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }
        h1 {
            color: #333;
            text-align: center;
        }
        .status {
            padding: 15px;
            margin: 15px 0;
            border-radius: 5px;
            background-color: #f0f0f0;
        }
        .log-container {
            margin-top: 20px;
            border: 1px solid #ddd;
            border-radius: 5px;
            padding: 10px;
            max-height: 300px;
            overflow-y: auto;
        }
        .log-entry {
            padding: 8px;
            border-bottom: 1px solid #eee;
        }
        .log-entry:last-child {
            border-bottom: none;
        }
        .timestamp {
            color: #666;
            font-size: 0.9em;
        }
        .message {
            margin-left: 10px;
        }
        .video-container {
            margin-top: 20px;
            text-align: center;
        }
        button {
            background-color: #4CAF50;
            color: white;
            border: none;
            padding: 10px 15px;
            border-radius: 4px;
            cursor: pointer;
            margin: 5px;
        }
        button:hover {
            background-color: #45a049;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Sistema de Monitoramento ESP32</h1>
        
        <div class="status">
            <h2>Status do Sistema</h2>
            <p><strong>Última Detecção:</strong> <span id="lastDetection">Nenhuma</span></p>
            <p><strong>Status do Stream:</strong> <span id="streamStatus">Inativo</span></p>
            <p><strong>IP:</strong> <span id="ipAddress">192.168.4.1</span></p>
        </div>
        
        <div>
            <button onclick="startStream()">Iniciar Stream</button>
            <button onclick="stopStream()">Parar Stream</button>
            <button onclick="capturePhoto()">Capturar Foto</button>
        </div>
        
        <div class="video-container">
            <img id="videoStream" src="http://192.168.4.1:81/stream" style="display: none; max-width: 100%;">
        </div>
        
        <div class="log-container" id="logContainer">
            <h2>Logs do Sistema</h2>
            <div id="logs">Carregando logs...</div>
        </div>
    </div>
    
    <script>
        // Atualiza dados da página
        function updateStatus() {
            fetch('/status')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('lastDetection').textContent = 
                        data.lastDetection;
                    document.getElementById('streamStatus').textContent = 
                        data.streamEnabled ? 'Ativo' : 'Inativo';
                    document.getElementById('ipAddress').textContent = 
                        data.ip;
                });
        }
        
        // Atualiza logs
        function updateLogs() {
            fetch('/logs')
                .then(response => response.text())
                .then(data => {
                    document.getElementById('logs').innerHTML = data;
                    // Rolagem automática para o final
                    const logContainer = document.getElementById('logContainer');
                    logContainer.scrollTop = logContainer.scrollHeight;
                });
        }
        
        // Inicia stream de vídeo
        function startStream() {
            document.getElementById('videoStream').style.display = 'block';
            fetch('/control?stream=on')
                .then(() => updateStatus());
        }
        
        // Para stream de vídeo
        function stopStream() {
            document.getElementById('videoStream').style.display = 'none';
            fetch('/control?stream=off')
                .then(() => updateStatus());
        }
        
        // Captura foto
        function capturePhoto() {
            fetch('/capture')
                .then(response => {
                    if(response.ok) {
                        alert('Foto capturada e enviada com sucesso!');
                    } else {
                        alert('Erro ao capturar foto');
                    }
                });
        }
        
        // Configura Server-Sent Events para atualização em tempo real
        const eventSource = new EventSource('/events');
        eventSource.addEventListener('logs', function(e) {
            document.getElementById('logs').innerHTML = e.data;
            const logContainer = document.getElementById('logContainer');
            logContainer.scrollTop = logContainer.scrollHeight;
        });
        
        // Atualiza a página periodicamente
        setInterval(updateStatus, 5000);
        
        // Inicializa
        updateStatus();
        updateLogs();
    </script>
</body>
</html>
)rawliteral";

void setupCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    addLog("Erro na inicialização da câmera");
    return;
  }
}

void setup() {
  Serial.begin(115200);
  
  // Inicializa OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.display();
  delay(2000);
  display.clearDisplay();
  
  // Configura pinos
  pinMode(PIR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Configura IP estático
  if(!WiFi.config(local_IP, gateway, subnet)) {
    addLog("Falha na configuração do IP estático");
  }
  
  // Conecta WiFi
  WiFi.begin(ssid, password);
  displayStatus("Conectando WiFi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    display.print(".");
    display.display();
  }
  
  displayStatus("WiFi conectado\nIP: " + WiFi.localIP().toString());
  addLog("Sistema iniciado");
  addLog("WiFi conectado, IP: " + WiFi.localIP().toString());
  
  // Inicializa câmera
  setupCamera();
  
  // Configura servidor web
  setupWebServer();
  
  // Inicia servidor
  server.begin();
  addLog("Servidor web iniciado");
}

void loop() {
  // Verifica sensor PIR
  if(digitalRead(PIR_PIN) == HIGH) {
    if(!motionDetected && (millis() - lastDetectionTime > detectionInterval)) {
      motionDetected = true;
      lastDetectionTime = millis();
      addLog("Movimento detectado!");
      displayStatus("Movimento detectado!");
      
      // Envia notificação para o Discord
      sendDiscordAlert();
      
      // Captura e envia foto
      captureAndSendPhoto();
    }
  } else {
    motionDetected = false;
  }
  
  // Verifica botão para habilitar/desabilitar stream
  if(digitalRead(BUTTON_PIN) == LOW) {
    delay(50); // Debounce
    if(digitalRead(BUTTON_PIN) == LOW) {
      streamEnabled = !streamEnabled;
      displayStatus(streamEnabled ? "Stream: ON" : "Stream: OFF");
      addLog(streamEnabled ? "Stream ativado" : "Stream desativado");
      delay(1000); // Evita múltiplos toques
    }
  }
  
  delay(100);
}

void displayStatus(String message) {
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(message);
  display.display();
}

void addLog(String message) {
  String logEntry = "[" + String(millis()/1000) + "] " + message;
  Serial.println(logEntry);
  systemLogs += logEntry + "<br>";
  
  // Limita o tamanho dos logs para evitar estouro de memória
  if(systemLogs.length() > 2000) {
    systemLogs = systemLogs.substring(systemLogs.length() - 2000);
  }
  
  // Envia atualização para clientes web via Server-Sent Events
  events.send(systemLogs.c_str(), "logs", millis());
}

void sendDiscordAlert() {
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(discordWebhook);
    http.addHeader("Content-Type", "application/json");
    
    String payload = "{\"content\":\"" + discordMessage + "\"}";
    
    int httpResponseCode = http.POST(payload);
    
    if(httpResponseCode > 0) {
      addLog("Alerta enviado ao Discord");
    } else {
      addLog("Falha ao enviar alerta ao Discord");
    }
    
    http.end();
  }
}

void captureAndSendPhoto() {
  camera_fb_t * fb = esp_camera_fb_get();
  if(!fb) {
    addLog("Falha ao capturar foto");
    return;
  }
  
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(discordWebhook);
    http.addHeader("Content-Type", "multipart/form-data");
    
    String boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW";
    String head = "--" + boundary + "\r\nContent-Disposition: form-data; name=\"file\"; filename=\"photo.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--" + boundary + "--\r\n";
    
    http.addHeader("Content-Type", "multipart/form-data; boundary=" + boundary);
    
    int httpResponseCode = http.POST((uint8_t*)head.c_str(), head.length());
    http.sendRequest("POST", (uint8_t*)fb->buf, fb->len);
    http.sendRequest("POST", (uint8_t*)tail.c_str(), tail.length());
    
    if(httpResponseCode > 0) {
      addLog("Foto enviada ao Discord");
    } else {
      addLog("Falha ao enviar foto ao Discord");
    }
    
    http.end();
  }
  
  esp_camera_fb_return(fb);
}

void setupWebServer() {
  // Rota para a página principal
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", htmlPage);
  });
  
  // Rota para obter logs
  server.on("/logs", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", systemLogs);
  });
  
  // Rota para status
  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = "{\"lastDetection\":\"" + String(motionDetected ? "AGORA" : "Nenhuma") + 
                 "\",\"streamEnabled\":" + String(streamEnabled ? "true" : "false") + 
                 ",\"ip\":\"" + WiFi.localIP().toString() + "\"}";
    request->send(200, "application/json", json);
  });
  
  // Rota para controle
  server.on("/control", HTTP_GET, [](AsyncWebServerRequest *request){
    if(request->hasParam("stream")) {
      String value = request->getParam("stream")->value();
      streamEnabled = (value == "on");
      addLog("Stream " + String(streamEnabled ? "ativado" : "desativado") + " via web");
      request->send(200, "text/plain", "OK");
    } else {
      request->send(400, "text/plain", "Parâmetro inválido");
    }
  });
  
  // Rota para captura de foto
  server.on("/capture", HTTP_GET, [](AsyncWebServerRequest *request){
    captureAndSendPhoto();
    request->send(200, "text/plain", "Foto capturada e enviada");
  });
  
  // Server-Sent Events para atualização em tempo real
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);
}
