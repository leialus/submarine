/*
pinos esp32s3
*/
#include "MyWebServer.h"
#include "UDPConnection.h"

void onEvent(arduino_event_id_t event) {
  Serial.print("Evento Ethernet: ");
  Serial.println(event);
}

//Received action ballback
void HanddlerCommands(int btn, int action){
  //TODO: send command to submarine using UDP
}

//Message callback to send by websocket
void WebsocketLog(const String& myMessage){
  String message = myMessage;
  message + "\n";
  message.trim();
  if (message.length() > 0) {
    WebSocketBroadcastMessage(message);
  }
}

void TaskReceptorUDP(void *pvParameters) {
  UDPReceptFrameSlice();
}

void AddQueueSlices(const uint8_t* data, size_t len) {
  uint8_t* dataCopy = (uint8_t*)malloc(len);
  if (dataCopy) {
      memcpy(dataCopy, data, len);
      PacketData pkt = { dataCopy, len };
      if (xQueueSend(frameSlicesQueue, &pkt, 0) != pdTRUE) {
          free(dataCopy); // queue is full
          Serial.println("queue is full drop package");
      }
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
  UDPConnectionCallback(AddQueueSlices);
  xTaskCreatePinnedToCore(TaskReceptorUDP, "ReceptorUDP", 4096, NULL, 1, NULL, 1);
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

  server.handleClient();  // Listen for incoming requests

  // WebSocket
  webSocket.loop();
  webSocketStream.loop();

  WebSocketBroadcastStream();
  
  //TODO: create rj45 (UDP) connection
  //get frame or message

  //TODO: send frames using websockts
  //WebSocketBroadcastStream(fb->buf, tamanho);
  //webSocket.loop();
}
