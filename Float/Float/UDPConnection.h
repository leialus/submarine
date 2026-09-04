#ifndef UDPConnection
#define UDPConnection

#include <SPI.h> 
#include <ETH.h>
#include <WiFiUdp.h>
#include "BoardConfig.h"

//My IP
IPAddress local_IP(192, 168, 10, 1);
IPAddress gateway(192, 168, 10, 254);
IPAddress subnet(255, 255, 255, 0);

//conect to
IPAddress remote_IP(192, 168, 10, 2);

WiFiUDP udp;
const uint16_t UDP_PORT = 5000;

void UDPInit()
{
  Serial.println();
  Serial.println("===  W5500 initializing... ===");

  // GPIO
  pinMode(W5500_CS, OUTPUT);
  digitalWrite(W5500_CS, HIGH);

  pinMode(W5500_RST, OUTPUT);
  digitalWrite(W5500_RST, HIGH);


  // SPI
  SPI.begin(W5500_SCLK, W5500_MISO, W5500_MOSI, W5500_CS);

  Serial.println("SPI initialized.");

  bool resultado = ETH.begin(ETH_PHY_W5500, 1, W5500_CS, -1, W5500_RST, SPI);
 
  unsigned long startAttempt = millis();
  while (!ETH.started() && (millis() - startAttempt < 5000)) {
    delay(100);
  }
  Serial.print("ETH.begin(): ");
  Serial.println(resultado ? "OK" : "Fail to initialize");

  ETH.config(local_IP, gateway, subnet);
  startAttempt = millis();
  while (!ETH.linkUp() && (millis() - startAttempt < 5000)) {
    delay(100);
  }
  
  delay(1000);

  Serial.println();
  Serial.println("=== Check link ===");

  Serial.print("ETH started: ");
  Serial.println(ETH.started() ? "YES" : "NO");

  Serial.print("IP: ");
  Serial.println(ETH.localIP());

  Serial.print("Link: ");
  Serial.println(ETH.linkUp() ? "UP" : "DOWN");

  Serial.print("MAC: ");
  Serial.println(ETH.macAddress());

  // Start UDP
  if (udp.begin(UDP_PORT))
  {
    Serial.print("Initialized UDP at port: ");
    Serial.println(UDP_PORT);
  }
  else
  {
    Serial.println("Fail to UDP initiale");
  }

  Serial.println("===  UDP Conected ===");
}

void UDPReceiver(){
  int packetSize = udp.parsePacket();
  if (packetSize > 0)
  {
    char buffer[128];

    int len = udp.read(
      buffer,
      sizeof(buffer) - 1
    );

    if (len > 0)
      buffer[len] = '\0';
    else
      buffer[0] = '\0';

    Serial.println();
    Serial.println("====== UDP RECEBIDO ======");

    Serial.print("De: ");
    Serial.println(udp.remoteIP());

    Serial.print("Porta: ");
    Serial.println(udp.remotePort());

    Serial.print("Mensagem: ");
    Serial.println(buffer);

    Serial.println("==========================");
  }

}

void UDPSender(){
//send
  static unsigned long ultimoEnvio = 0;

  if (millis() - ultimoEnvio >= 2000)
  {   
    ultimoEnvio = millis();

    const char *mensagem = "OLA ESP32 Submarine";

    if (udp.beginPacket(remote_IP, UDP_PORT)){

      udp.write((const uint8_t *)mensagem, strlen(mensagem));

      int resultadoEnvio = udp.endPacket();

      Serial.print("Resultado UDP: ");
      Serial.println(resultadoEnvio == 1 ? "Sended" : "Fail to send");
    } else {
      Serial.println("beginPacket() FALHOU");
    }
  }
}

#endif