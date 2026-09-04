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
  Serial.println("=== START SERIAL === ");
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  esp_reset_reason_t reason = esp_reset_reason();
  Serial.printf("Reset reason = %d\n", reason);

  Network.onEvent(onEvent);
  MyWebServerCallbacks(WebsocketLog, HanddlerCommands);

  //WebServerInit();
  //delay(1000);

  UDPInit();
  delay(1000);
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

  server.handleClient();  // Listen for incoming requests

  // WebSocket
  webSocket.loop();
  webSocketStream.loop();

  //UDP-----------------------------------------------------
  UDPReceiver();
  //UDPSender();
  
  //TODO: create rj45 (UDP) connection
  //get frame or message

  //TODO: send frames using websockts
  //WebSocketBroadcastStream(fb->buf, tamanho);
  //webSocket.loop();
}
