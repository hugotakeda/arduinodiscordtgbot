#include "src/OV2640.h"
#include <WiFi.h>
#include "esp_http_client.h"
#include "src/U8x8lib.h"

const char *ssid =     "VIVOFIBRA-1FE1";         // Put your SSID here
const char *password = "2NBJwUFuUX";     // Put your PASSWORD here

const char *post_url = "http://192.168.15.10:8888/esp32cam/posted.php"; // Location where images are POSTED

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 22, /* data=*/ 21);

#define PIR_PIN 33

OV2640 cam;

void setup()
{
  Serial.begin(115200);

  u8x8.begin();
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);

  pinMode(PIR_PIN, INPUT); //pir
  esp_sleep_enable_ext0_wakeup((gpio_num_t )PIR_PIN, HIGH);

  cam.init(esp32cam_ttgo_t_config);

  if (init_wifi()) { // Connected to WiFi
    Serial.println("Internet connected");
  } else {
    u8x8.setCursor(4, 2);
    u8x8.print("Lucky!");
    u8x8.setCursor(2, 4);
    u8x8.print("No Internet!");
  }
}

bool init_wifi()
{
  int connAttempts = 0;
  Serial.println("\r\nConnecting to: " + String(ssid));
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED ) {
    delay(500);
    Serial.print(".");
    if (connAttempts > 10) return false;
    connAttempts++;
  }
  return true;
}

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
  switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
      Serial.println("HTTP_EVENT_ERROR");
      break;
    case HTTP_EVENT_ON_CONNECTED:
      Serial.println("HTTP_EVENT_ON_CONNECTED");
      break;
    case HTTP_EVENT_HEADER_SENT:
      Serial.println("HTTP_EVENT_HEADER_SENT");
      break;
    case HTTP_EVENT_ON_HEADER:
      Serial.println();
      Serial.printf("HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
      break;
    case HTTP_EVENT_ON_DATA:
      Serial.println();
      Serial.printf("HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
      if (!esp_http_client_is_chunked_response(evt->client)) {
        // Write out data
        // printf("%.*s", evt->data_len, (char*)evt->data);
      }
      break;
    case HTTP_EVENT_ON_FINISH:
      Serial.println("");
      Serial.println("HTTP_EVENT_ON_FINISH");
      break;
    case HTTP_EVENT_DISCONNECTED:
      Serial.println("HTTP_EVENT_DISCONNECTED");
      break;
  }
  return ESP_OK;
}

void warning_screen()
{
  u8x8.fillDisplay();
  u8x8.clear();
  u8x8.fillDisplay();
  u8x8.clear();
  u8x8.fillDisplay();
  u8x8.clear();
  u8x8.fillDisplay();
  u8x8.clear();

  u8x8.setCursor(6, 0);
  u8x8.print("STEP");
  delay(600);
  u8x8.setCursor(6, 2);
  u8x8.print("AWAY");
  delay(800);
  u8x8.setCursor(4, 4);
  u8x8.print("FROM");
  delay(500);
  u8x8.setCursor(9, 4);
  u8x8.print("MY");
  delay(500);
  u8x8.setCursor(5, 6);
  u8x8.print("HOUSE");
  delay(1000);
  u8x8.clear();
  u8x8.setCursor(4, 1);
  u8x8.print("PHOTO IN");
  u8x8.setCursor(5, 7);
  u8x8.print("SECONDS");
  delay(500);
  u8x8.setFont(u8x8_font_courB18_2x3_n);
  for (int s = 5; s > 0; s--) {
    u8x8.setCursor(7, 3);
    u8x8.print(s);
    delay(1000);
  }
}

static esp_err_t take_send_photo()
{
  Serial.println("Taking picture...");

  esp_err_t res = ESP_OK;
  cam.run();

  esp_http_client_handle_t http_client;

  esp_http_client_config_t config_client = {0};
  config_client.url = post_url;
  config_client.event_handler = _http_event_handler;
  config_client.method = HTTP_METHOD_POST;

  http_client = esp_http_client_init(&config_client);

  esp_http_client_set_post_field(http_client, (char *)cam.getfb(), cam.getSize());

  esp_http_client_set_header(http_client, "Content-Type", "image/jpg");

  esp_err_t err = esp_http_client_perform(http_client);
  if (err == ESP_OK) {
    Serial.print("esp_http_client_get_status_code: ");
    Serial.println(esp_http_client_get_status_code(http_client));
  }

  esp_http_client_cleanup(http_client);
  back_to_sleep();
}

void back_to_sleep()
{
  Serial.println("Device Sleeping...");
  delay(1000);
  u8x8.setPowerSave(true);
  WiFi.mode(WIFI_OFF);
  btStop();
  esp_deep_sleep_start();
}

void loop()
{
  Serial.println("Device Woken");
  warning_screen();

  long timer_millis = millis();
  while (millis() - timer_millis < 3000) { //Test for movement for 3 seconds after warning screen
    if (digitalRead(PIR_PIN)) { // If still movement - Take photo
      take_send_photo();
    }
  }
  back_to_sleep();
}
