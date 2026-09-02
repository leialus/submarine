//ESP32-S3 CAM

#include <Arduino.h>
#include <SPI.h>
#include <ETH.h>
#include <WiFiUdp.h>
#include "esp_camera.h"
#include "sensor.h"
//#include "MyWebServer.h" //TODO: change to eternet later
#include "BoardConfig.h"

IPAddress local_IP(192, 168, 10, 2);
IPAddress gateway(192, 168, 10, 1);
IPAddress subnet(255, 255, 255, 0);

IPAddress remote_IP(192, 168, 10, 1);
WiFiUDP udp;
const uint16_t UDP_PORT = 5000;

void onEvent(arduino_event_id_t event) {
  Serial.print("Evento Ethernet: ");
  Serial.println(event);
}
void setup() {
  Network.onEvent(onEvent);

  // turn camera off
  pinMode(PWDN_GPIO_NUM, OUTPUT);
  digitalWrite(PWDN_GPIO_NUM, HIGH);
  delay(1000);

  //serial
  Serial.println("=== START SERIAL === ");
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  esp_reset_reason_t reason = esp_reset_reason();
  Serial.printf("Reset reason = %d\n", reason);

  //Check esp32-s3 cam
  Serial.println("=== HARDWARE CHECK ESP32-S3 CAM ===");
  
  uint32_t flashSize = ESP.getFlashChipSize();
  Serial.printf("Tamanho da Flash: %d MB (%d Bytes)\n", flashSize / (1024 * 1024), flashSize);
  
  if (psramInit()) {
    Serial.println("=== PSRAM initialized ===");
    uint32_t psramSize = ESP.getPsramSize();
    uint32_t freePsram = ESP.getFreePsram();
    Serial.printf("Tamanho Total da PSRAM: %d MB (%d Bytes)\n", psramSize / (1024 * 1024), psramSize);
    Serial.printf("PSRAM Livre: %d Bytes\n", freePsram);
  } else {
    Serial.println("ERR: PSRAM fail or desactivated");
  }

  //initializing camera
  Serial.println("=== Camera initializing... ===");

  digitalWrite(PWDN_GPIO_NUM, LOW);
  delay(1000);

  //Serial.println("\n--- WEBSERVER INIT... ---");
  //WebServerInit();

  //delay(3000);

  camera_config_t config = CameraConfig();
  esp_err_t err = esp_camera_init(&config);

  if (err != ESP_OK) {

    Serial.printf("Fail on camera initilizing: 0x%x\n", err);

    return;
  }

  Serial.println("Camera initialized!");
  delay(1000);
  
  sensor_t * s = esp_camera_sensor_get();
  if (s != NULL) {
    Serial.printf("Detected camera - PID: 0x%02X, VER: 0x%02X\n", s->id.PID, s->id.VER);
  } else {
    Serial.println("Camera detection fail");
  }
  
  Serial.println("=== Camera initialized ===");

  delay(1000);

  //UDP connection
  Serial.println("=== UDP connection initializing... ===");
  //udp code in here
  Serial.println("=== UDP initialized ===");
}

void loop() {
  static unsigned long lastMem = 0;

  if (millis() - lastMem >= 2000) {
      lastMem = millis();

      Serial.printf(
          "Heap=%u | PSRAM=%u | MinHeap=%u | MinPSRAM=%u\n | temp=%.1f °C\n",
          ESP.getFreeHeap(),
          ESP.getFreePsram(),
          ESP.getMinFreeHeap(),
          ESP.getMinFreePsram(),
          temperatureRead()
      );
  }

  //TODO: remove later
  // Listen for incoming requests
  //server.handleClient();  

  camera_fb_t *fb = esp_camera_fb_get();
  
  if (fb) {
    size_t tamanho = fb->len;

    //TODO send by UDP later
    //WebSocketBroadcastStream(fb->buf, tamanho);
    //webSocket.loop();

    esp_camera_fb_return(fb);
  } else {
    Serial.println("frame capture fail");
  }
}
