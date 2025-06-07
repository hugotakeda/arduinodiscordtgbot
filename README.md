ESP32-CAM Fridge Monitor
Overview
The ESP32-CAM Fridge Monitor is a project that uses an ESP32-CAM module equipped with an OV2640 camera and a PIR (Passive Infrared) sensor to detect movement near a refrigerator. When motion is detected, the system wakes up, displays a warning on an SSD1306 OLED display, captures a photo, and sends it to a server via HTTP POST. After completing its tasks, the ESP32 enters deep sleep mode to conserve power.
This project is designed for monitoring unauthorized access to a fridge, with a visual warning and photo capture functionality.
Features

Motion Detection: Uses a PIR sensor to detect movement near the fridge.
Visual Warning: Displays a countdown and warning messages on a 128x64 SSD1306 OLED display.
Photo Capture: Captures images using the OV2640 camera module on the ESP32-CAM.
HTTP POST: Sends captured images to a specified server URL.
Deep Sleep: Enters low-power deep sleep mode after completing tasks to save energy.
Wi-Fi Connectivity: Connects to a Wi-Fi network to upload images.

Hardware Requirements

ESP32-CAM Module: A development board with an ESP32-S module and OV2640 camera.
PIR Sensor: Connected to GPIO 33 for motion detection.
SSD1306 OLED Display: 128x64 resolution, connected via I2C (SCL: GPIO 22, SDA: GPIO 21).
Power Supply: USB or battery (ensure stable 3.3V or 5V supply).
Wi-Fi Network: A local Wi-Fi network for internet connectivity.

Software Requirements

Arduino IDE: For programming the ESP32-CAM.
ESP32 Board Support: Add the ESP32 board package to the Arduino IDE.
Libraries:
WiFi.h: For Wi-Fi connectivity (included with ESP32 board package).
esp_http_client.h: For HTTP POST requests (included with ESP32 board package).
U8x8lib.h: For controlling the SSD1306 OLED display (install via Arduino Library Manager).
OV2640.h: Custom library for the OV2640 camera (ensure it’s included in the src folder).


Server: A web server running PHP (or another language) to receive images (see Server Setup).

Installation

Set Up the Arduino IDE:

Install the Arduino IDE from arduino.cc.
Add ESP32 board support:
Go to File > Preferences, add the following URL to "Additional Boards Manager URLs":https://raw.githubusercontent.com/espressif/arduino-esp32/master/package_esp32_index.json


In Tools > Board > Boards Manager, search for "ESP32" and install the package.


Select the board: Tools > Board > ESP32 Arduino > ESP32-CAM.


Install Libraries:

Install the U8x8lib library:
Go to Sketch > Include Library > Manage Libraries, search for "U8g2", and install.


Ensure the OV2640.h library is in the src folder of your project.


Configure the Code:

Open the provided Arduino sketch (fridge_monitor.ino) in the Arduino IDE.
Update the following variables in the code:const char *ssid = "VIVOFIBRA-1FE1"; // Replace with your Wi-Fi SSID
const char *password = "2NBJwUFuUX"; // Replace with your Wi-Fi password
const char *post_url = "http://192.168.15.10:8888/esp32cam/posted.php"; // Replace with your server URL


Ensure the post_url points to your server (see Server Setup).


Upload the Code:

Connect the ESP32-CAM to your computer via a USB-to-serial adapter (e.g., FTDI module).
Select the correct port in Tools > Port.
Upload the sketch to the ESP32-CAM.



Server Setup
The ESP32-CAM sends images via HTTP POST to a server. Below is an example of setting up a server using PHP on a local machine with MAMP (macOS).
Prerequisites

MAMP: Download and install from mamp.info.
PHP: Included with MAMP.
Directory: /Applications/MAMP/htdocs.

Steps

Start MAMP:

Open MAMP and start the Apache server (default port: 8888).
Confirm the Apache server is running (green light in MAMP).


Create the PHP Script:

In /Applications/MAMP/htdocs, create a folder named esp32cam.

Create a file named posted.php with the following content:
<?php
header('Content-Type: text/plain');

$upload_dir = 'uploads/';
if (!is_dir($upload_dir)) {
    mkdir($upload_dir, 0777, true);
}

$filename = $upload_dir . 'image_' . time() . '.jpg';
$image_data = file_get_contents('php://input');

if ($image_data !== false && !empty($image_data)) {
    if (file_put_contents($filename, $image_data)) {
        echo "Image uploaded successfully: $filename";
    } else {
        echo "Failed to save image.";
    }
} else {
    echo "No image data received.";
}
?>


Create a folder named uploads in /Applications/MAMP/htdocs/esp32cam.

Set permissions:
chmod -R 777 /Applications/MAMP/htdocs/esp32cam/uploads




Test the Server:

Access http://localhost:8888/esp32cam/posted.php in a browser. You should see "No image data received."
Update the post_url in the Arduino code to match your server’s IP (e.g., http://192.168.15.10:8888/esp32cam/posted.php).


Online Deployment (Optional):

For remote access, use a hosting service or set up port forwarding on your router.
Consider adding security (e.g., HTTPS, authentication key).



Usage

Power the ESP32-CAM:

Connect the ESP32-CAM to a power source (USB or battery).
The device enters deep sleep and waits for the PIR sensor to detect motion.


Motion Detection:

When the PIR sensor (GPIO 33) detects motion, the ESP32 wakes up.
The OLED display shows a warning ("STEP AWAY FROM THE FRIDGE") followed by a 5-second countdown.


Photo Capture and Upload:

If motion is still detected after 3 seconds, the camera captures a photo.
The photo is sent to the server via HTTP POST.
The device then returns to deep sleep.


View Images:

Check the /Applications/MAMP/htdocs/esp32cam/uploads folder for saved images.
Alternatively, set up a web interface to view images remotely.



Troubleshooting

Wi-Fi Connection Fails:

Verify the SSID and password in the code.
Check the serial monitor (Serial.begin(115200)) for connection status.
Ensure the ESP32-CAM is within Wi-Fi range.


Images Not Saving:

Confirm the server is running and the post_url is correct.
Check MAMP logs: /Applications/MAMP/logs/php_error.log or /Applications/MAMP/logs/apache_error.log.
Verify permissions on the uploads folder (chmod -R 777).


PIR Sensor Issues:

Ensure the PIR sensor is connected to GPIO 33 and configured as an input.
Test the sensor by monitoring digitalRead(PIR_PIN) in the serial monitor.


OLED Display Not Working:

Check I2C connections (SCL: GPIO 22, SDA: GPIO 21).
Ensure the U8x8lib library is installed correctly.



Security Considerations

Authentication: Add a secret key to the PHP script and ESP32 code to prevent unauthorized access:
PHP:$secret_key = "your_secret_key";
if ($_SERVER['HTTP_X_SECRET_KEY'] !== $secret_key) {
    echo "Invalid key.";
    exit;
}


ESP32:esp_http_client_set_header(http_client, "X-Secret-Key", "your_secret_key");




HTTPS: Use a secure server with SSL for online deployments.
Firewall: Ensure the server’s port (e.g., 8888) is open but protected.

Future Improvements

Add a web interface to view captured images.
Integrate notifications (e.g., email or Telegram) when motion is detected.
Store images in cloud storage (e.g., AWS S3, Google Cloud Storage).
Implement real-time streaming instead of single photos.

License
This project is licensed under the MIT License. See the LICENSE file for details.
Acknowledgments

ESP32 Arduino Core for ESP32 support.
U8g2 Library for OLED display control.
MAMP for local server setup on macOS.

