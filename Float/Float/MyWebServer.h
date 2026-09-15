#ifndef MyWebServer
#define MyWebServer

#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "Webpage.h"
#include "FrameHanddler.h"

inline void (*onToWebSendMessage)(const String&) = nullptr;
inline void (*onActionUpdate)(const ActionPackage&) = nullptr;

void MyWebServerCallbacks(void (*ToWebSendMessageCallback)(const String&), void (*ActionCallback)(const ActionPackage&)) {
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

void handleButton(ActionPackage actionPackage) {
  String message = (actionPackage.typeMessage == 0) ? "to Float: " : "to submarine: ";
  message += String(actionPackage.button);
  message += actionPackage.action ? " PRESSED" : " RELEASED";

  if (onToWebSendMessage) {
    onToWebSendMessage(message);
  }

  if (onActionUpdate) {
    onActionUpdate(actionPackage);
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  if (type == WStype_BIN) {
    if (length == sizeof(ActionPackage)) {
      ActionPackage actionPackage(0, 0, 0);

      memcpy(&actionPackage, payload, sizeof(ActionPackage));
      
      handleButton(actionPackage); 
    } else {
      Serial.println("Err: Tinvalid package size.");
    }
  } else if (type == WStype_TEXT) {
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
  } 
}

void WebServerInit() {
  frameSlicesQueue = xQueueCreate(QUEUE_SIZE, sizeof(PacketData));

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

void WebSocketBroadcastStream(bool isDebugFrame){
  PacketData pkt;
  if (xQueueReceive(frameSlicesQueue, &pkt, 0) == pdTRUE) {

    if (pkt.len >= sizeof(PacketHeader)) {
      webSocket.broadcastBIN(pkt.data, pkt.len);

      if (isDebugFrame){
        PacketHeader* header = (PacketHeader*)pkt.data;
        DebugPacketHeader(*header);
      }
    }
  }
}

void WebSocketBroadcastMessage(String message){
  webSocket.broadcastTXT(message);
}

#endif
