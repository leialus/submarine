//ESP32-S3 CAM

#include <Arduino.h>
#include <SPI.h>
#include "esp_camera.h"
#include "sensor.h"
//#include "MyWebServer.h" //TODO: change to eternet later

#include "BoardConfig.h"

void w5500Reset() {
  digitalWrite(W5500_RST, LOW);
  delay(100);
  digitalWrite(W5500_RST, HIGH);
  delay(200);
}

uint8_t w5500Read(uint16_t address, uint8_t block) {
  uint8_t data;

  digitalWrite(W5500_CS, LOW);

  SPI.transfer(address >> 8);
  SPI.transfer(address & 0xFF);

  // Read + VDM + bloco
  SPI.transfer((block << 3) | 0x00);

  data = SPI.transfer(0x00);

  digitalWrite(W5500_CS, HIGH);

  return data;
}

void setup() {
  // turn camera off and not allow wificonnection
  pinMode(PWDN_GPIO_NUM, OUTPUT);
  digitalWrite(PWDN_GPIO_NUM, HIGH);
  delay(1000);

  Serial.begin(115200);

  Serial.println("\n--- START SERIAL ---");
  
  Serial.setDebugOutput(true);

  esp_reset_reason_t reason = esp_reset_reason();

  Serial.printf("Reset reason = %d\n", reason);

  Serial.println("\n--- HARDWARE CHECK ESP32-S3 CAM ---");
  
  uint32_t flashSize = ESP.getFlashChipSize();
  Serial.printf("Tamanho da Flash: %d MB (%d Bytes)\n", flashSize / (1024 * 1024), flashSize);
  
  if (psramInit()) {
    Serial.println("PSRAM Inicializada com sucesso!");
    uint32_t psramSize = ESP.getPsramSize();
    uint32_t freePsram = ESP.getFreePsram();
    Serial.printf("Tamanho Total da PSRAM: %d MB (%d Bytes)\n", psramSize / (1024 * 1024), psramSize);
    Serial.printf("PSRAM Livre: %d Bytes\n", freePsram);
  } else {
    Serial.println("ERRO: PSRAM não foi detectada ou está desativada nas configurações!");
  }

  Serial.println("\n--- INIT ---");
  digitalWrite(PWDN_GPIO_NUM, LOW);
  delay(1000);

  //Serial.println("\n--- WEBSERVER INIT... ---");
  //WebServerInit();

  //delay(3000);

  Serial.println("\n--- CAMERA INIT... ---");

  camera_config_t config = CameraConfig();
  esp_err_t err = esp_camera_init(&config);

  if (err != ESP_OK) {

    Serial.printf("Falha ao inicializar camera: 0x%x\n", err);

    return;

  }

  Serial.println("Camera OK!");
  delay(1000);
  
  sensor_t * s = esp_camera_sensor_get();
  if (s != NULL) {
    Serial.printf("🔍 ID da Câmera Detectado - PID: 0x%02X, VER: 0x%02X\n", s->id.PID, s->id.VER);
  } else {
    Serial.println("❌ Não foi possível ler as informações de ID do sensor.");
  }

  delay(1000);
  Serial.println();
  Serial.println("=== TESTE GPIO + SPI ESP32-S3-CAM ===");

  pinMode(W5500_CS, OUTPUT);
  digitalWrite(W5500_CS, HIGH);

  pinMode(W5500_RST, OUTPUT);
  digitalWrite(W5500_RST, HIGH);

  Serial.println("GPIO CS configurado.");
  Serial.println("GPIO RST configurado.");

  SPI.begin(W5500_SCLK, W5500_MISO, W5500_MOSI, W5500_CS);

  w5500Reset();

  uint8_t mr = w5500Read(0x0000, 0x00);

  Serial.print("MR = 0x");
  if (mr < 0x10) Serial.print("0");
  Serial.println(mr, HEX);

  // PHYCFGR
  uint8_t phy = w5500Read(0x002E, 0x00);

  Serial.print("PHYCFGR = 0x");
  if (phy < 0x10) Serial.print("0");
  Serial.println(phy, HEX);
  Serial.println("SPI inicializado.");
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

  // Listen for incoming requests
  //server.handleClient();  

  camera_fb_t *fb = esp_camera_fb_get();
  
  if (fb) {
    size_t tamanho = fb->len;

    //WebSocketBroadcastStream(fb->buf, tamanho);
    //webSocket.loop();

    esp_camera_fb_return(fb);
  } else {
    Serial.println("frame capture fail");
  }
}
