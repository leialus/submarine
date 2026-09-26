//ESP32-S3 CAM

#include <WiFi.h>
#include "GlobalVars.h"
#include "ComProtocols.h"
#include "sensor.h"
#include "CameraStream.h"
#include "UDPConnection.h"

ActionProt currentActionProt(0, false);
volatile bool newAction = false; 

static unsigned long lastHearthbeatDettected = 0;

void onEvent(arduino_event_id_t event) {
  Serial.print("Evento Ethernet: ");
  Serial.println(event);
}

//Received action ballback
void HanddlerCommands(const ActionProt& prot){
  currentActionProt = prot;
  newAction = true;
}

void HanddlerHarthbeats() {
  isConnectedToFloat = true;
  lastHearthbeatDettected = millis();
}

void setup() {
  WiFi.mode(WIFI_OFF);
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
  MyUDPConnectionCallbacks(HanddlerCommands, HanddlerHarthbeats);

  //create async task to send frames slices
  xTaskCreatePinnedToCore(FrameSplit, "FrameSplit", 8192, NULL, 3, NULL, 1);
}

//calback to send UDP images slices
void ToSendFrameSlice(const FrameProt& prot, const uint8_t* data, size_t data_len) {
  sendFrameSlice(prot, data, data_len);
}

void loop() {
  static unsigned long lastMem = 0;

  if (millis() - lastHearthbeatDettected >= 6000) {
    isConnectedToFloat = false;
  }
  
  if (millis() - lastMem >= 2000) {
    lastMem = millis();

    Serial.printf("isConnectedToFloat: %s\n", isConnectedToFloat ? "true" : "false");

    boardHeat = temperatureRead();
    Serial.printf(
      "Heap=%u | PSRAM=%u | MinHeap=%u | MinPSRAM=%u\n | temp=%.1f °C\n",
      ESP.getFreeHeap(),
      ESP.getFreePsram(),
      ESP.getMinFreeHeap(),
      ESP.getMinFreePsram(),
      boardHeat
    );

    String telemetry = "#Submarine: ";
    telemetry += String(boardHeat);
    
    UDPSenderTelemetry(telemetry);
  }

  UDPReceiver(); 
  
  if (newAction && isConnectedToFloat) {
    //DO SOMETHING
  } 

  delay(10);
}
