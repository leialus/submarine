/*
pinos esp32s3
*/
#include "Action.h"
#include "MyWebServer.h"
#include "UDPConnection.h"

String telemetry = "";

void onEvent(arduino_event_id_t event) {
  Serial.print("Evento Ethernet: ");
  Serial.println(event);
}

//Received action ballback
void HanddlerCommands(const ActionPackage& actionPackage){
  
  //if this comand is suposed to run only fror 
  if (actionPackage.typeMessage == 0) return;

  //send command to submarine using Udp
  UDPSenderComand(actionPackage);
}

//callback to populate telemetry from Submarine
//We need to do it cuz UDP task is async, if we try send to websockets we get Cache error
void HanddleTelemetryMessage(const String& myMessage){
  telemetry = myMessage;
}

//Message callback to send by websocket
void WebsocketLog(const String& myMessage){
  String message = myMessage;
  message += "\n";
  message.trim();
  if (message.length() > 0) {
    WebSocketBroadcastMessage(message);
  }
}

void TaskReceptorUDP(void *pvParameters) {
  UDPReceptFrameSlice();
}

void AddQueueSlices(const uint8_t* data, size_t len) {
    if (len > CHUNK_DATA_SIZE) {
      Serial.println("data is too big");
      return;
    }

    static PacketData pkt;
    pkt.len = len;
     // copy data to struct
    memcpy(pkt.data, data, len);

    // tryb and add to queue
    if (xQueueSend(frameSlicesQueue, &pkt, 0) != pdTRUE) {
        Serial.println("queue is full drop package");
    }
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

  MyWebServerCallbacks(WebsocketLog, HanddlerCommands);

  WebServerInit();
  delay(1000);
  
  //UDP connection
  UDPInit();
  UDPConnectionCallback(AddQueueSlices, HanddleTelemetryMessage);
  xTaskCreatePinnedToCore(TaskReceptorUDP, "ReceptorUDP", 4096, NULL, 1, NULL, 1);
}

void loop() {
  static unsigned long lastMem = 0;
  static bool isDebugFrame = false;
  if (millis() - lastMem >= 2000) {
    isDebugFrame = true;

    lastMem = millis();

    Serial.printf(
      "Heap=%u | PSRAM=%u | MinHeap=%u | MinPSRAM=%u\n | temp=%.1f °C\n",
      ESP.getFreeHeap(),
      ESP.getFreePsram(),
      ESP.getMinFreeHeap(),
      ESP.getMinFreePsram(),
      temperatureRead()
    );

    if (telemetry.length() > 5){
      //send Submarine Telemetry
      WebsocketLog(telemetry);

      //send Float Telemetry
      String telemetry = "#Float: ";
      telemetry += String(temperatureRead());
      WebsocketLog(telemetry);

      telemetry = "";
    }
  }


  // WebSocket
  webSocket.loop();

  // Listen for incoming requests
  server.handleClient();

  WebSocketBroadcastStream(isDebugFrame);


  isDebugFrame = false;

  delay(10);
}
