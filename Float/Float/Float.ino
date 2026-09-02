/*
pinos esp32s3
*/

#include <SPI.h> 
#include <ETH.h>
#include <WiFiUdp.h>

#include "BoardConfig.h"
#include "MyWebServer.h"

IPAddress local_IP(192, 168, 10, 1);
IPAddress gateway(192, 168, 10, 2);
IPAddress subnet(255, 255, 255, 0);

IPAddress remote_IP(192, 168, 10, 2);

WiFiUDP udp;
const uint16_t UDP_PORT = 5000;

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

  WebServerInit();
  delay(1000);

  Serial.println();
  Serial.println("===  W5500 initializing... ===");
  Serial.println("TODO");
}

void loop() {
  server.handleClient();  // Listen for incoming requests

  // WebSocket
  webSocket.loop();
  webSocketStream.loop();
  //TODO: create rj45 (UDP) connection
  //get frame or message

  //TODO: send frames using websockts
  //WebSocketBroadcastStream(fb->buf, tamanho);
  //webSocket.loop();
}
