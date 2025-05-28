# Sistema de Monitoramento com ESP32-CAM

Este projeto implementa um sistema de monitoramento utilizando o ESP32-CAM (módulo AI-Thinker) para detectar movimento, capturar fotos, transmitir vídeo ao vivo e enviar alertas para um servidor do Discord. O sistema conta com uma interface web para status em tempo real e registros, um sensor PIR para detecção de movimento, display OLED para status local e um botão físico para alternar o modo de streaming.

## Funcionalidades

* **Detecção de Movimento**: Utiliza um sensor PIR para detectar movimento e acionar a captura de imagens.
* **Captura de Fotos e Alertas**: Captura fotos e envia para um webhook do Discord ao detectar movimento.
* **Transmissão de Vídeo**: Transmite vídeo ao vivo através de uma interface web.
* **Interface Web**: Exibe status do sistema, registros e controles para iniciar/parar o streaming e capturar imagens.
* **Display OLED**: Mostra atualizações de status em tempo real (como conexão Wi-Fi, detecção de movimento).
* **Botão Físico**: Permite alternar o estado da transmissão de vídeo.
* **IP Estático**: Utiliza endereço IP fixo para facilitar o acesso à interface web.

## Requisitos de Hardware

* **ESP32-CAM (módulo AI-Thinker)**: Microcontrolador principal com câmera OV2640.
* **Sensor PIR**: Conectado ao GPIO 33 para detecção de movimento.
* **Botão**: Conectado ao GPIO 34 para alternar o modo de transmissão.
* **Display OLED SSD1306 (128x64)**: Conectado via I2C para exibir o status.
* **Adaptador USB-TTL**: Para programar o ESP32-CAM (ex.: CP2102, CH340, FTDI).
* **Jumpers**: Para conexão do GPIO 0 ao GND durante o upload.
* **Fonte de Alimentação**: 5V ou 3.3V (mínimo 500mA).

## Requisitos de Software

* **Arduino IDE**: Recomendado versão 2.x ou superior.
* **Suporte à Placa ESP32**: Instale via Boards Manager com a URL:
  `https://raw.githubusercontent.com/espressif/arduino-esp32/master/package_esp32_index.json`
* **Bibliotecas Necessárias**:

  * `esp_camera` (câmera)
  * `WiFi` (conectividade de rede)
  * `ESPAsyncWebServer` (servidor web assíncrono)
  * `AsyncTCP` (dependência do ESPAsyncWebServer)
  * `Adafruit_GFX` (gráficos para OLED)
  * `Adafruit_SSD1306` (controle do display OLED)
  * `HTTPClient` (envio de webhooks para o Discord)
  * `ArduinoJson` (manipulação de JSON)

Use o Gerenciador de Bibliotecas da Arduino IDE para instalar.

## Configuração de Pinos

Pinos configurados para o módulo AI-Thinker ESP32-CAM:

* **Câmera**:

  * PWDN: GPIO 26
  * XCLK: GPIO 32
  * SIOD: GPIO 13
  * SIOC: GPIO 12
  * Y9-Y2: GPIO 39, 36, 23, 18, 15, 4, 14, 5
  * VSYNC: GPIO 27
  * HREF: GPIO 25
  * PCLK: GPIO 19

* **Sensor PIR**: GPIO 33

* **Botão**: GPIO 34 (com pull-up interno)

* **Display OLED (I2C)**: SDA - GPIO 13, SCL - GPIO 12

## Instruções de Montagem

### 1. Ligações de Hardware

1. **Adaptador USB-TTL**:

   * GND → GND (ESP32-CAM)
   * 5V ou 3.3V → 5V/3.3V do ESP32-CAM (verifique as especificações)
   * TX → U0R (GPIO 3)
   * RX → U0T (GPIO 1)

2. **Sensor PIR**:

   * OUT → GPIO 33
   * VCC → 5V ou 3.3V
   * GND → GND

3. **Botão**:

   * Um terminal → GPIO 34
   * Outro terminal → GND

4. **Display OLED**:

   * SDA → GPIO 13
   * SCL → GPIO 12
   * VCC → 3.3V
   * GND → GND

5. **GPIO 0 (modo de upload)**:

   * Durante o upload, conecte GPIO 0 ao GND com um jumper
   * Após o upload, remova o jumper para executar o código

### 2. Configuração do Software

1. Instale a Arduino IDE e suporte à placa ESP32
2. Instale todas as bibliotecas listadas acima
3. Abra o arquivo `ESP32-CAM-Monitoring.ino`
4. Atualize as credenciais Wi-Fi:

   ```cpp
   const char* ssid = "SEU_WIFI_SSID";
   const char* password = "SUA_SENHA_WIFI";
   ```
5. (Opcional) Configure a URL do webhook do Discord:

   ```cpp
   const char* discordWebhook = "SUA_WEBHOOK_DO_DISCORD";
   ```

### 3. Upload do Código

1. **Modo de Upload**:

   * Conecte GPIO 0 ao GND
   * Pressione o botão Reset ou reconecte a alimentação

2. **Configuração na Arduino IDE**:

   * **Placa**: AI Thinker ESP32-CAM (ou ESP32 Wrover Module)
   * **Porta**: Selecione a porta serial correta
   * **Velocidade de Upload**: 115200
   * **Esquema de Partições**: Default 4MB with spiffs ou Huge APP (3MB No OTA/1MB SPIFFS)

3. **Realizar Upload**:

   * Clique em **Upload**
   * Quando aparecer "Connecting....", pressione Reset (se necessário)
   * Após o upload, remova o jumper do GPIO 0 e pressione Reset para iniciar

### 4. Testes

* **Monitor Serial**: Acesse a 115200 baud para ver os logs (ex.: “WiFi conectado”, “Movimento detectado”)
* **Interface Web**: Acesse `http://192.168.4.1` no navegador para visualizar status e controles
* **Alertas no Discord**: Verifique o recebimento das fotos e mensagens via webhook
* **Display OLED**: Cheque se o status do sistema aparece corretamente
* **Botão**: Pressione o botão para iniciar/parar a transmissão de vídeo

## Solução de Problemas

* **Erro de Upload ("No serial data received")**:

  * Confirme a conexão do GPIO 0 ao GND durante o upload
  * Verifique os drivers do adaptador USB-TTL
  * Use um cabo USB de qualidade e fonte confiável (mínimo 500mA)
  * Teste velocidades menores (ex.: 115200) ou outro computador/porta

* **Falha na Câmera**:

  * Verifique os pinos da câmera e alimentação correta

* **Problemas com Wi-Fi**:

  * Verifique SSID/senha
  * Certifique-se de que o ESP32-CAM está dentro do alcance do roteador

* **Problemas de Memória**:

  * O código utiliza \~91% da memória. Para melhorar a estabilidade, considere mover a página HTML para SPIFFS

## Estrutura do Projeto

```
ESP32-CAM-Monitoring/
├── ESP32-CAM-Monitoring.ino  # Sketch principal
├── README.md                 # Este arquivo
```

## Contribuições

Contribuições são bem-vindas! Envie *issues* ou *pull requests* para melhorias, otimizações ou novos recursos.

## Licença

Este projeto está licenciado sob a Licença MIT. Veja o arquivo [LICENSE](LICENSE) para detalhes.

## Agradecimentos

* [Documentação ESP32-CAM](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/hw-reference/modules/esp32-cam.html)
* [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
* [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
