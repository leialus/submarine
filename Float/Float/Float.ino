/*
pinos esp32s3
*/

#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h> 
#include <SPI.h> 

#include "Webpage.h"
#include "BoardConfig.h"

const char* ssid = "001ROV";
const char* password = "12345678";

const char* buttonNames[] = {
  "up", "down", "left", "right",
  "a", "b", "x", "y",
  "start", "select"
};

const int numButtons = sizeof(buttonNames) / sizeof(buttonNames[0]);

// web server at 80 (HTTP)
WebServer server(80);

//web Sockets at 81
WebSocketsServer webSocket = WebSocketsServer(81);

void w5500Reset() {
  Serial.println("Resetando W5500...");

  digitalWrite(W5500_RST, LOW);
  delay(100);
  digitalWrite(W5500_RST, HIGH);
  delay(200);

  Serial.println("Reset concluido.");
}

uint8_t w5500Read(uint16_t address, uint8_t block) {
  uint8_t data;

  digitalWrite(W5500_CS, LOW);

  SPI.transfer(address >> 8);
  SPI.transfer(address & 0xFF);

  // Read + VDM + bloco
  SPI.transfer((block << 3) | 0x00);

  data = SPI.transfer(0x00);

  digitalWrite(W5500_CS, HIGH);

  return data;
}

void setup() {
  Serial.begin(115200);

  // ESP Access Point
  WiFi.softAP(ssid, password);
  Serial.println("Rede criada: " + String(ssid));
  Serial.print("ESP IP Address: ");
  Serial.println(WiFi.softAPIP()); // print ESP IP Address

  // WebServer acesses
  // start
  server.on("/", handleRoot); 

  // serever initialized
  server.begin();
  Serial.println("HTTP Server initialized!");

  // Websockets initialized
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("WebSocket Server initialized!");  

  delay(1000);

  Serial.println();
  Serial.println("=== TESTE SPI W5500 ===");

  pinMode(W5500_CS, OUTPUT);
  digitalWrite(W5500_CS, HIGH);

  pinMode(W5500_RST, OUTPUT);
  digitalWrite(W5500_RST, HIGH);

  SPI.begin(W5500_SCLK, W5500_MISO, W5500_MOSI, W5500_CS);

  Serial.println("SPI inicializado.");

  w5500Reset();
  uint8_t mr = w5500Read(0x0000, 0x00);

  Serial.print("MR = 0x");
  if (mr < 0x10) Serial.print("0");
  Serial.println(mr, HEX);

  // PHYCFGR
  uint8_t phy = w5500Read(0x002E, 0x00);

  Serial.print("PHYCFGR = 0x");
  if (phy < 0x10) Serial.print("0");
  Serial.println(phy, HEX);
}

void loop() {
  server.handleClient();  // Listen for incoming requests

  // WebSocket
  webSocket.loop();

  //TODO: change to rj45 connection
  //receive arduino message
  /*if (Serial.available()) {
    String message = Serial.readStringUntil('\n');
    message.trim();
    if (message.length() > 0) {
      //send message to webpage
      webSocket.broadcastTXT(message);
    }
  }*/
}

void handleRoot() {
  server.send_P(200, "text/html", WEBPAGE_HTML);
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

  bool pressed = strcmp(action, "press") == 0;

  handleButton(String(name), pressed);
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

  //remove later------------
  String message = button;
  message += pressed ? " PRESSED" : " RELEASED";
  message.trim();
  if (message.length() > 0) {
    //send message to webpage
    webSocket.broadcastTXT(message);
  }
  //------------------------
}