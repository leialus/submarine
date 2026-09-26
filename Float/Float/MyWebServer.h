#ifndef MyWebServer
#define MyWebServer

#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "Webpage.h"

#define QUEUE_SIZE 6
QueueHandle_t frameSlicesQueue;

inline void (*onToWebSendMessage)(const String&) = nullptr;
inline void (*onActionUpdate)(const ActionProt&) = nullptr;

void MyWebServerCallbacks(void (*ToWebSendMessageCallback)(const String&), void (*ActionCallback)(const ActionProt&)) {
  onToWebSendMessage = ToWebSendMessageCallback;
  onActionUpdate = ActionCallback;
}

const char* ssid = "001ROV";
const char* password = "12345678";

// web server at 80 (HTTP)
WebServer server(80);

//web Sockets at 81
WebSocketsServer webSocket = WebSocketsServer(81);

void handleRoot() {
  server.send_P(200, "text/html", WEBPAGE_HTML);
}

void handleButton(ActionProt actionProt) {
  String message = (actionProt.protType == 0) ? "to Float: " : "to submarine: ";
  message += String(actionProt.button);
  message += actionProt.action ? " PRESSED" : " RELEASED";

  if (onToWebSendMessage) {
    onToWebSendMessage(message);
  }

  if (onActionUpdate) {
    onActionUpdate(actionProt);
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  if (type == WStype_BIN) {
    if (length == sizeof(ActionProt)) {
      ActionProt actionProt(0, 0);

      memcpy(&actionProt, payload, sizeof(ActionProt));
      
      handleButton(actionProt); 
    } else {
      Serial.println("Err: Tinvalid package size.");
    }
  } else if (type == WStype_TEXT) {
    //e.g
    /*
    String mensagem = String((char*)payload);

    StaticJsonDocument<200> doc;

    DeserializationError error = deserializeJson(doc, mensagem);

    if (error) {
      Serial.print("Erro JSON: ");
      Serial.println(error.c_str());
      return;
    }

    const char* str1   = doc["field1"];

    //Do something
    return;
    */
  } 
}

void WebServerInit() {
  frameSlicesQueue = xQueueCreate(QUEUE_SIZE, sizeof(UDPPkt));

  Serial.println("=== WiFi initializing... ===");
  WiFi.softAP(ssid, password);
  Serial.println("Online: " + String(ssid));
  Serial.print("ESP IP Address: ");
  Serial.println(WiFi.softAPIP()); // print ESP IP Address

  // WebServer acesses
  // start
  server.on("/", handleRoot); 

  // serever initialized
  server.begin();
  Serial.println("=== HTTP Server initialized!=== ");

  Serial.println();

  Serial.println("=== webSocket initializing... ===");
  // Websockets initialized
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("=== WebSocket Server initialized!=== ");  
}

void WebSocketBroadcastPack(UDPPkt pkt){
    if(pkt.len > 0){
      webSocket.broadcastBIN(pkt.data, pkt.len);
    }
}

void WebSocketBroadcastStream(bool isDebugFrame){
  if (!isSubmarineOnline) return;

  UDPPkt pkt;
  if (xQueueReceive(frameSlicesQueue, &pkt, 0) == pdTRUE) {

    if (pkt.len >= sizeof(FrameProt)) {
      WebSocketBroadcastPack(pkt);

      if (isDebugFrame){
        FrameProt* prot = (FrameProt*)pkt.data;
        DebugPacketHeader(*prot);
      }
    }
  }
}

void WebSocketBroadcastMessage(String message){
  webSocket.broadcastTXT(message);
}

void WebSocketBroadcastTelemetry(String telemetry){
  if (telemetry.length() < 5) return;

  TelemetryProt prot;
  UDPPkt pkt;

  pkt.len = sizeof(prot) + telemetry.length();

  memcpy(pkt.data, &prot, sizeof(prot));
  memcpy(pkt.data + sizeof(prot), telemetry.c_str(), telemetry.length());
  
  WebSocketBroadcastPack(pkt);
}

#endif
