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

IPAddress targetIP(192, 168, 1, 10);

W5500Driver driver(W5500_CS);
EthernetUDP Udp;

const int UDPPort = 8888;

inline void (*onActionUpdate)(const ActionPackage&) = nullptr;

void MyUDPConnectionCallbacks(void (*ActionCallback)(const ActionPackage&)) {
  onActionUpdate = ActionCallback;
}

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
  if (packetSize > 0) {
    ActionPackage actionPackage(0, 0, false);

    if (packetSize == sizeof(ActionPackage)) {
      
      Udp.read((char*)&actionPackage, sizeof(ActionPackage));
      if (onActionUpdate != nullptr){
        onActionUpdate(actionPackage);
      }

    } else {
      Serial.print("UDP package size does not match: ");
      Serial.println(packetSize);
    }

    //// to send an answer
    // Udp.beginPacket(Udp.remoteIP(), Udp.remotePort);
    // Udp.print("Mensagem recebida do submarine!");
    // Udp.endPacket();
    // Serial.println("Resposta enviada.");
  }

}

void UDPSenderTelemetry(String telemetry){
  if (telemetry.length() < 5) return;
  Udp.beginPacket(targetIP, UDPPort);
  Udp.print(telemetry);
  Udp.endPacket();
}

void sendFrameSlice(const PacketHeader& header, const uint8_t* data, size_t data_len) {
  Udp.beginPacket(targetIP, UDPPort);
  Udp.write((uint8_t*)&header, sizeof(header));
  Udp.write(data, data_len);
  Udp.endPacket();
}

#endif