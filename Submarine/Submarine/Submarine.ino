//ESP32-S3 CAM

#include "sensor.h"
#include "CameraStream.h"
#include "UDPConnection.h"

void onEvent(arduino_event_id_t event) {
  Serial.print("Evento Ethernet: ");
  Serial.println(event);
}

void setup() {
  Network.onEvent(onEvent);

  //serial
  Serial.println("=== START SERIAL === ");
  Serial.begin(115200);
  while (!Serial) { delay(10); }
  Serial.setDebugOutput(true);
  esp_reset_reason_t reason = esp_reset_reason();
  Serial.printf("Reset reason = %d\n", reason);

  //Start camera streaming
  CameraStreamCallback(ToSendFrameSlice);
  StreamInit();

  //UDP connection
  UDPInit();

  //create async task to send frames slices
  xTaskCreatePinnedToCore(FrameSplit, "FrameSplit", 8192, NULL, 3, NULL, 1);
}

//calback to send UDP images slices
void ToSendFrameSlice(const PacketHeader& header, const uint8_t* data, size_t data_len) {
  sendFrameSlice(header, data, data_len);
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
}
