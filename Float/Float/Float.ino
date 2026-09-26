/*
pinos esp32s3
*/
#include "GlobalVars.h"
#include "ComProtocols.h"
#include "MyWebServer.h"
#include "UDPConnection.h"

UDPPkt telemetryPkt;
static unsigned long lastSubmarineTelemetry = 0;

void onEvent(arduino_event_id_t event) {
  Serial.print("Evento Ethernet: ");
  Serial.println(event);
}

//Received action ballback
void HanddlerCommands(const ActionProt& actionProt){
  
  //if this comand is suposed to run only on Float 
  if (actionProt.protType != PROT_ACTION || !isSubmarineOnline) return;

  //send command to submarine using Udp
  UDPSenderComand(actionProt);
}

//callback to populate telemetry from Submarine
//We need to do it cuz UDP task is async, if we try send to websockets we get Cache error
void HanddleTelemetryMessage(UDPPkt pkt){
  telemetryPkt = pkt;
  isSubmarineOnline = true;
  lastSubmarineTelemetry = millis();
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
  UDPReceiver();
}

void AddQueueSlices(const uint8_t* data, size_t len) {
    static UDPPkt pkt;
    pkt.len = len;
     // copy data to struct
    memcpy(pkt.data, data, len);

    // try and add to queue
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
  xTaskCreatePinnedToCore(TaskReceptorUDP, "ReceptorUDP", 6144, NULL, 1, NULL, 1);
}

void loop() {
  static unsigned long lastMem = 0;
  static bool isDebugFrame = false;

  if (millis() - lastSubmarineTelemetry >= 6000) {
    lastSubmarineTelemetry = millis();
    isSubmarineOnline = false;
  }

  if (millis() - lastMem >= 2000) {
    isDebugFrame = true;
    lastMem = millis();

    UDPSenderHarthbeat();

    Serial.printf(
      "Heap=%u | PSRAM=%u | MinHeap=%u | MinPSRAM=%u\n | temp=%.1f °C\n",
      ESP.getFreeHeap(),
      ESP.getFreePsram(),
      ESP.getMinFreeHeap(),
      ESP.getMinFreePsram(),
      temperatureRead()
    );
    
    if (isSubmarineOnline) {
      //send Submarine Telemetry
      WebSocketBroadcastPack(telemetryPkt);
      telemetryPkt.len = 0;
    } else {
      WebSocketBroadcastTelemetry("#Submarine: Offline");
    }
    
    //send Float Telemetry
    String telemetry = "#Float: ";
    telemetry += String(temperatureRead());
    WebSocketBroadcastTelemetry(telemetry);

    telemetry = "";
  }

  // WebSocket
  webSocket.loop();

  // Listen for incoming requests
  server.handleClient();

  WebSocketBroadcastStream(isDebugFrame);

  isDebugFrame = false;

  delay(10);
}
