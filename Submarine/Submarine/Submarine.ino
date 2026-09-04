//ESP32-S3 CAM

#include <Arduino.h>
#include "esp_camera.h"
#include "sensor.h"
#include "UDPConnection.h"
#include "BoardConfig.h"

void onEvent(arduino_event_id_t event) {
  Serial.print("Evento Ethernet: ");
  Serial.println(event);
}

void setup() {
  Network.onEvent(onEvent);

  //serial
  Serial.println("=== START SERIAL === ");
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  esp_reset_reason_t reason = esp_reset_reason();
  Serial.printf("Reset reason = %d\n", reason);
  // turn camera off
 /* pinMode(PWDN_GPIO_NUM, OUTPUT);
  digitalWrite(PWDN_GPIO_NUM, HIGH);
  delay(1000);


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
*/
  delay(1000);

  //UDP connection
  Serial.println("=== UDP connection initializing... ===");
  UDPInit();
  Serial.println("=== UDP initialized ===");
}

void loop() {
  static unsigned long lastCheck = 0;

  if (millis() - lastCheck >= 2000)
  {
    Serial.println();
    Serial.println("-------- FLOAT --------");

    Serial.print("ETH started: ");
    Serial.println(ETH.started() ? "SIM" : "NAO");

    Serial.print("Link: ");
    Serial.println(ETH.linkUp() ? "UP" : "DOWN");

    Serial.print("IP: ");
    Serial.println(ETH.localIP());

    Serial.print("remote_IP: ");
    Serial.println(remote_IP);

    Serial.print("UDP_PORT: ");
    Serial.println(UDP_PORT);

    Serial.println("---------------------------");    

    lastCheck = millis();
  }


  
  /*static unsigned long lastMem = 0;
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
  }*/

   //UDP-----------------------------------------------------
  //UDPReceiver();
  UDPSender();

  camera_fb_t *fb = esp_camera_fb_get();
  
  /*if (fb) {
    size_t tamanho = fb->len;

    //TODO send by UDP later
    //WebSocketBroadcastStream(fb->buf, tamanho);
    //webSocket.loop();

    esp_camera_fb_return(fb);
  } else {
    Serial.println("frame capture fail");
  }*/
}
