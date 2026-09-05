#ifndef UDPConnection
#define UDPConnection

#include <SPI.h>
#include <EthernetESP32.h>
#include <EthernetUdp.h>
#include "BoardConfig.h"

byte myMacAddress[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x02 };
IPAddress localIP(192, 168, 1, 20);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// IP e porta do servidor
IPAddress clientIP(192, 168, 1, 10);

W5500Driver driver(W5500_CS);
EthernetUDP Udp;

const int UDPPort = 8888;

void UDPInit()
{
  Serial.println();
  Serial.println("===  W5500 initializing... ===");

  // W5500 reset
  pinMode(W5500_RST, OUTPUT);
  digitalWrite(W5500_RST, LOW);
  delay(10);
  digitalWrite(W5500_RST, HIGH);
  delay(100);

  // Inicialize SPI with desired pins
  SPI.begin(W5500_SCLK, W5500_MISO, W5500_MOSI, W5500_CS);

  // Set ethernet driver on ethernet class
  Ethernet.init(driver);
  
  Serial.println("===  Ethernet and UDP connecting... ===");

  // initialize static IP ethernet
  Ethernet.begin(myMacAddress, localIP, gateway, subnet);

  // Check ethernet hardware
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("ERRO: Hardware W5500 not finded!");
    while (true) { delay(1); }
  }

  // Check cable connection
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("ERRO: Ethernet not connected!");
  } else {
    Serial.println("Ethernet connected.");
  }

  // Initialize UDP service
  if (Udp.begin(UDPPort)) {
    Serial.print("Servidor UDP started at Port:");
    Serial.println(UDPPort);
    Serial.print("IP: ");
    Serial.println(Ethernet.localIP());
  } else {
    Serial.println("Fail to inicialize UDP.");
    while (1);
  }

  Serial.println("===  Ethernet and UDP connected ===");
}

void UDPReceiver(){
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    // Lê a mensagem
    char buffer[255];
    int len = Udp.read(buffer, 255);
    if (len > 0) {
      buffer[len] = 0;
      Serial.print("Recebido de ");
      Serial.print(Udp.remoteIP());
      Serial.print(":");
      Serial.print(Udp.remotePort());
      Serial.print(" -> ");
      Serial.println(buffer);
    }

    //// to send an answer
    // Udp.beginPacket(Udp.remoteIP(), Udp.remotePort);
    // Udp.print("Mensagem recebida do submarine!");
    // Udp.endPacket();
    // Serial.println("Resposta enviada.");
  }

}

unsigned long lastPingTime = 0;
void UDPSender(){
  if ( millis() - lastPingTime >= 5000) {
    lastPingTime = millis();
    
    Udp.beginPacket(clientIP, UDPPort);
    Udp.print("Ping do submarine!");
    Udp.endPacket();
    Serial.print("Ping enviado para ");
    Serial.print(clientIP);
    Serial.print(":");
    Serial.println(UDPPort);
  }
}

#endif