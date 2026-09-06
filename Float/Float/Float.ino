/*
pinos esp32s3
*/
#include "MyWebServer.h"
#include "UDPConnection.h"

void onEvent(arduino_event_id_t event) {
  Serial.print("Evento Ethernet: ");
  Serial.println(event);
}

void HanddlerCommands(int btn, int action){
  //TODO: send command to submarine using UDP
}

void WebsocketLog(const String& myMessage){
  String message = myMessage;
  message + "\n";
  message.trim();
  if (message.length() > 0) {
    WebSocketBroadcastMessage(message);
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
}

void loop() {
  server.handleClient();  // Listen for incoming requests

  // WebSocket
  webSocket.loop();
  webSocketStream.loop();
  
  //UDP-----------------------------------------------------
  UDPReceiver();
  UDPSender();
  
  //TODO: create rj45 (UDP) connection
  //get frame or message

  //TODO: send frames using websockts
  //WebSocketBroadcastStream(fb->buf, tamanho);
  //webSocket.loop();
}
