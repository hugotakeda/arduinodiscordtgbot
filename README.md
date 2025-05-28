ESP32-CAM Monitoring System
This project implements a monitoring system using the ESP32-CAM (AI-Thinker module) to detect motion, capture photos, stream video, and send alerts to a Discord server. The system features a web interface for real-time status updates and logs, a PIR sensor for motion detection, an OLED display for local status, and a button to toggle video streaming.
Features

Motion Detection: Uses a PIR sensor to detect motion and trigger photo capture.
Photo Capture and Alerts: Captures photos and sends them to a Discord webhook when motion is detected.
Video Streaming: Streams live video via a web interface.
Web Interface: Displays system status, logs, and controls for starting/stopping the stream and capturing photos.
OLED Display: Shows real-time status updates (e.g., Wi-Fi connection, motion detection).
Button Control: Toggles video streaming on/off using a physical button.
Static IP Configuration: Uses a fixed IP address for reliable access to the web interface.

Hardware Requirements

ESP32-CAM (AI-Thinker module): Main microcontroller with OV2640 camera.
PIR Sensor: Connected to GPIO 33 for motion detection.
Push Button: Connected to GPIO 34 for toggling video streaming.
SSD1306 OLED Display (128x64): Connected via I2C for status display.
USB-TTL Adapter: For programming the ESP32-CAM (e.g., CP2102, CH340, FTDI).
Jumper Wires: For connecting GPIO 0 to GND during upload.
Power Supply: 5V or 3.3V (minimum 500mA) to power the ESP32-CAM.

Software Requirements

Arduino IDE: Version 2.x or later recommended.
ESP32 Board Support: Install via Arduino Boards Manager (URL: https://raw.githubusercontent.com/espressif/arduino-esp32/master/package_esp32_index.json).
Libraries:
esp_camera (for camera functionality)
WiFi (for network connectivity)
ESPAsyncWebServer (for asynchronous web server)
AsyncTCP (dependency for ESPAsyncWebServer)
Adafruit_GFX (for OLED display graphics)
Adafruit_SSD1306 (for OLED display control)
HTTPClient (for sending Discord webhooks)
ArduinoJson (for JSON parsing)



Install libraries via the Arduino IDE Library Manager.
Pin Configuration
The pin assignments are tailored for the AI-Thinker ESP32-CAM module:

Camera Pins:
PWDN: GPIO 26
XCLK: GPIO 32
SIOD: GPIO 13
SIOC: GPIO 12
Y9-Y2: GPIO 39, 36, 23, 18, 15, 4, 14, 5
VSYNC: GPIO 27
HREF: GPIO 25
PCLK: GPIO 19


PIR Sensor: GPIO 33
Push Button: GPIO 34 (with internal pull-up)
OLED Display: I2C (SDA: GPIO 13, SCL: GPIO 12)

Setup Instructions
1. Hardware Connections

Connect the USB-TTL Adapter:
GND (adapter) → GND (ESP32-CAM)
5V or 3.3V (adapter) → 5V or 3.3V (ESP32-CAM, check your module's specs)
TX (adapter) → U0R (GPIO 3)
RX (adapter) → U0T (GPIO 1)


Connect the PIR Sensor:
OUT → GPIO 33
VCC → 5V or 3.3V (check sensor specs)
GND → GND


Connect the Push Button:
One pin → GPIO 34
Other pin → GND


Connect the OLED Display:
SDA → GPIO 13
SCL → GPIO 12
VCC → 3.3V
GND → GND


GPIO 0 for Bootloader:
During upload, connect GPIO 0 to GND using a jumper wire.
Remove the jumper after uploading to run the code.



2. Software Setup

Install the Arduino IDE and ESP32 board support.
Install the required libraries listed above.
Open the ESP32-CAM-Monitoring.ino file in the Arduino IDE.
Update the Wi-Fi credentials in the code:const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";


(Optional) Update the Discord webhook URL if needed:const char* discordWebhook = "YOUR_DISCORD_WEBHOOK_URL";



3. Uploading the Code

Enter Bootloader Mode:
Connect GPIO 0 to GND.
Press the Reset button on the ESP32-CAM or reconnect power.


Configure Arduino IDE:
Board: Select AI Thinker ESP32-CAM (or ESP32 Wrover Module).
Port: Choose the correct serial port (e.g., /dev/cu.usbserial-* on macOS or COM* on Windows).
Upload Speed: Set to 115200.
Partition Scheme: Use Default 4MB with spiffs or Huge APP (3MB No OTA/1MB SPIFFS).


Upload:
Click Upload in the Arduino IDE.
When "Connecting...." appears, ensure GPIO 0 is connected to GND and press Reset if needed.
After upload, disconnect GPIO 0 from GND and press Reset to run the program.



4. Testing

Serial Monitor: Open at 115200 baud to view logs (e.g., "WiFi conectado", "Movimento detectado").
Web Interface: Access http://192.168.4.1 in a browser to view the status, logs, and control streaming or photo capture.
Discord Alerts: Verify that motion detection triggers photo uploads and messages to the configured Discord webhook.
OLED Display: Check for status updates (e.g., "WiFi conectado", "Movimento detectado").
Button: Press the button (GPIO 34) to toggle video streaming.

Troubleshooting

Upload Error ("No serial data received"):
Ensure GPIO 0 is connected to GND during upload.
Check USB-TTL adapter drivers (CP2102, CH340, etc.).
Use a high-quality USB cable and verify power supply (minimum 500mA).
Try a lower upload speed (e.g., 115200) or a different USB port/computer.


Camera Initialization Failure:
Verify camera pin connections match the code.
Ensure sufficient power supply.


Wi-Fi Connection Issues:
Double-check SSID and password.
Ensure the ESP32-CAM is within Wi-Fi range.


Memory Issues:
The sketch uses ~91% of program space. If unstable, move the HTML page to SPIFFS.



Project Structure
ESP32-CAM-Monitoring/
├── ESP32-CAM-Monitoring.ino  # Main Arduino sketch
├── README.md                 # This file

Contributing
Feel free to submit issues or pull requests for improvements, such as optimizing memory usage, adding features, or enhancing the web interface.
License
This project is licensed under the MIT License. See the LICENSE file for details.
Acknowledgments

ESP32-CAM documentation
ESPAsyncWebServer library
Adafruit SSD1306 library

