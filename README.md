
---

````markdown
# 📷 ESP32 Discord Surveillance Bot

Sistema de videomonitoramento com ESP32, que detecta movimento e envia alertas ao Discord com imagem em tempo real. Também permite transmitir vídeo ao vivo em uma chamada do Discord via OBS.

---

## 🧩 Componentes Utilizados

- 📦 **TTGO ESP32 WROVER** com câmera (OV2640)
- 🟪 **Sensor de Movimento PIR**
- 📺 **Display OLED SSD1306 (opcional)**
- 👁️ **Lente olho de peixe (fish-eye)**

---

## 🚀 Funcionalidades

- ✅ Detecta movimento com sensor PIR
- ✅ Captura imagem da câmera no momento do movimento
- ✅ Envia alerta ao canal Discord marcando `@everyone`
- ✅ Servidor MJPEG para transmissão em tempo real
- ✅ Transmissão de vídeo para chamada de voz via bot + OBS

---

## 🌐 Estrutura do Projeto

```bash
ESP32-Discord-Bot/
│
├── esp32/
│   └── camera_stream.ino        # Código do ESP32 com servidor MJPEG
│
├── bot/
│   ├── send_alert.js            # Bot que envia imagem e alerta ao Discord
│   ├── call_bot.js              # Bot que entra na chamada de voz
│   └── package.json             # Dependências Node.js
│
└── README.md
````

---

## ⚙️ Como Funciona

### 1. ESP32

* Conecta-se ao Wi-Fi
* Inicializa a câmera
* Roda um servidor HTTP com:

  * `/` → Página com stream ao vivo
  * `/stream` → MJPEG
* (Opcional) Endpoint `/capture` para capturar uma imagem JPEG

### 2. Bot de Alerta (Node.js)

* Monitora o ESP32
* Quando o movimento é detectado, busca uma imagem via HTTP
* Envia a imagem para o canal Discord com `@everyone`

### 3. Transmissão via OBS + Bot

* Crie uma **Fonte de Navegador** no OBS apontando para `http://<ESP32_IP>`
* Ative a **Webcam Virtual**
* O bot entra em uma chamada de voz e transmite essa "webcam virtual"

---

## 🔧 Instalação

### 📲 1. Flash do ESP32

* Instale as bibliotecas:

  * `esp_camera.h`
  * `WiFi.h`
  * `WebServer.h`
* Configure seu Wi-Fi no código
* Faça upload via Arduino IDE ou PlatformIO

### 💻 2. Bot Discord (Node.js)

1. Instale o Node.js
2. Instale as dependências:

```bash
cd bot
npm install
```

3. Crie um bot no Discord e obtenha o token
4. Crie um comando `entrarchamada` se desejar usar o bot de voz
5. Rode o bot:

```bash
node send_alert.js      # para envio de alerta com imagem
node call_bot.js        # para entrar em chamada de voz
```

---

## 🛠️ Configurações Relevantes

### `camera_stream.ino`

```cpp
const char* ssid = "SEU_WIFI";
const char* password = "SENHA_WIFI";
```

### `send_alert.js`

```js
const TOKEN = 'SEU_TOKEN_DISCORD_BOT';
const CHANNEL_ID = 'ID_DO_CANAL';
const CAMERA_URL = 'http://<ESP32_IP>/capture';
```

---

## 🎥 Exemplo de Funcionamento

* Alguém passa na frente do sensor PIR

* O ESP32 envia uma imagem

* O bot envia no Discord:

  ```
  @everyone Movimento detectado!
  ```

* Ao ativar o OBS + bot de chamada, o vídeo ao vivo é transmitido diretamente no Discord.

---

## 📌 Notas Finais

* O ESP32 não pode transmitir diretamente para chamadas do Discord por limitações de hardware e protocolos.
* O uso do OBS com Webcam Virtual simula isso no desktop, ideal para visualização de vídeo ao vivo.

---

## 📃 Licença

MIT © 2025 - Projeto educacional de monitoramento com ESP32 e Discord

```
MIT License

Copyright (c) 2025 [Seu Nome]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the “Software”), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell  
copies of the Software, and to permit persons to whom the Software is  
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in  
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE  
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER  
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN  
THE SOFTWARE.

```
