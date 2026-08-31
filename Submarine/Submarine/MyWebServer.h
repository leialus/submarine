#ifndef MyWebServer
#define MyWebServer

#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

const char *ssid = "Rove_NET";
const char *password = "12345678";
const char WEBPAGE_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32-CAM Stream</title>
    <style>
        body { margin: 0; background: #1a1a1a; display: flex; justify-content: center; align-items: center; height: 100vh; }
        #container { max-width: 100%; max-height: 100%; }
        img { max-width: 100%; max-height: 100%; border: 2px solid #333; border-radius: 8px; }
    </style>
</head>
<body>
    <div id="container">
        <img id="stream" src="" alt="Aguardando stream...">
    </div>

    <script>
        // Conecta ao WebSocket (porta 81, padrão da biblioteca)
        const ws = new WebSocket(`ws://${location.hostname}:81/`);
        ws.binaryType = 'arraybuffer';  // Recebemos dados binários

        ws.onopen = function() {
            console.log('Conectado ao WebSocket');
        };

        const stream = document.getElementById("stream");

        ws.binaryType = "blob";

        let displaying = false;

        ws.onmessage = async (event) => {

            if (displaying) {
                return;
            }

            displaying = true;

            const url = URL.createObjectURL(event.data);

            stream.onload = () => {
                URL.revokeObjectURL(url);
                displaying = false;
            };

            stream.src = url;
        };

        ws.onclose = function() {
            console.log('Desconectado do WebSocket');
            document.getElementById('stream').src = '';
        };

        ws.onerror = function(error) {
            console.error('WebSocket error:', error);
        };
    </script>
</body>
</html>
)rawliteral";

// web server at 80 (HTTP)
WebServer server(80);

//web Sockets at 81
WebSocketsServer webSocket = WebSocketsServer(81);

void handleRoot() {
  server.send_P(200, "text/html", WEBPAGE_HTML);
}

void handleButton(const String& button) {
  Serial.print(button);
  
  //TODO: camera general config (e.g set resolution)
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

  if (name == nullptr) {
    Serial.println("Mensagem invalida.");
    return;
  }

  handleButton(String(name));
}

void WebServerInit(){
  Serial.println("Connection Setup");
  WiFi.softAP(ssid, password);
  Serial.println("Rede criada: " + String(ssid));
  Serial.print("ESP IP Address: ");
  Serial.println(WiFi.softAPIP()); // print ESP IP Address

  // WebServer acesses
  server.on("/", handleRoot);

  // serever initialized
  server.begin();
  Serial.println("HTTP Server initialized!");

  // Websockets initialized
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("WebSocket Server initialized!"); 
}

void WebSocketBroadcastStream(uint8_t * buf, size_t len){
  webSocket.broadcastBIN(buf, len);
}

#endif
