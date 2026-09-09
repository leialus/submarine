#ifndef MyWebServer
#define MyWebServer

#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "Webpage.h"
#include "FrameHanddler.h"

inline void (*onToWebSendMessage)(const String&) = nullptr;
inline void (*onActionUpdate)(const int, const int) = nullptr;

void MyWebServerCallbacks(void (*ToWebSendMessageCallback)(const String&), void (*ActionCallback)(const int, const int)) {
  onToWebSendMessage = ToWebSendMessageCallback;
  onActionUpdate = ActionCallback;
}

const char* buttonNames[] = {
  "up", "down", "left", "right",
  "a", "b", "x", "y",
  "start", "select"
};

const int numButtons = sizeof(buttonNames) / sizeof(buttonNames[0]);

const char* ssid = "001ROV";
const char* password = "12345678";

// web server at 80 (HTTP)
WebServer server(80);

//web Sockets at 81
WebSocketsServer webSocket = WebSocketsServer(81);

//web Sockets at 82
WebSocketsServer webSocketStream = WebSocketsServer(82);

void handleRoot() {
  server.send_P(200, "text/html", WEBPAGE_HTML);
}

int getButtonId(const String& name) {
  for (int i = 0; i < numButtons; i++) {
    if (name == String(buttonNames[i])) {
      return i;
    }
  }
  return -1;
}

void handleButton(const String& button, bool pressed) {
  Serial.print(button);
  Serial.println(pressed ? " PRESSED" : " RELEASED");

  int id = getButtonId(button);
  Serial.write(0xFF);
  Serial.write(pressed ? 1 : 0);
  Serial.write(id);

  server.send(200, "text/plain", "OK");

  String message = button;
  message += pressed ? " PRESSED" : " RELEASED";

  if (onToWebSendMessage) {
    onToWebSendMessage(message);
  }

  if (onActionUpdate) {
    onActionUpdate(id, pressed);
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  if (type != WStype_TEXT) {
    return;
  }

  String mensagem = String((char*)payload);

  StaticJsonDocument<200> doc;

  DeserializationError error = deserializeJson(doc, mensagem);

  if (error) {
    Serial.print("Erro JSON: ");
    Serial.println(error.c_str());
    return;
  }

  const char* name   = doc["name"];
  const char* action = doc["action"];

  if (name == nullptr || action == nullptr) {
    Serial.println("Mensagem invalida.");
    return;
  }

  Serial.println("fst");
  bool pressed = strcmp(action, "press") == 0;

  handleButton(String(name), pressed);
}

void webSocketStreamEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {

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
  webSocketStream.begin();
  webSocketStream.onEvent(webSocketStreamEvent);
  Serial.println("=== WebSocket Server initialized!=== ");  
}

void WebSocketBroadcastStream(){
  PacketData pkt;
  while (xQueueReceive(frameSlicesQueue, &pkt, 0) == pdTRUE) {

    if (pkt.len >= sizeof(PacketHeader)) {
      webSocketStream.broadcastBIN(pkt.data, pkt.len);
      
      PacketHeader* header = (PacketHeader*)pkt.data;
      Serial.printf("[DEBUG after broadcast] frameId=%u, totalSize=%u, chunkId=%u/%u\n",
                    header->frameId, header->totalSize, header->chunkId+1, header->totalChunks);
    }

    free(pkt.data); 
  }
}

void WebSocketBroadcastMessage(String message){
  webSocket.broadcastTXT(message);
}

#endif
