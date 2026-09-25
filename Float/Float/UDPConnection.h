#ifndef UDPConnection
#define UDPConnection

#include <SPI.h>
#include <EthernetESP32.h>
#include <EthernetUdp.h>
#include "BoardConfig.h"

byte myMacAddress[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x01 };
IPAddress localIP(192, 168, 1, 10);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

IPAddress targetIP(192, 168, 1, 20);

W5500Driver driver(W5500_CS);
EthernetUDP Udp;

const int UDPPort = 8888;

inline void (*onToQueueSlice)(const uint8_t* data, size_t len) = nullptr;
inline void (*onToWebSendTelemetry)(UDPPkt pkt) = nullptr;

void UDPConnectionCallback(void (*ToQueueSlice)(const uint8_t* data, size_t len), void (*ToWebSendTelemetryCallback)(UDPPkt pkt)) {
  onToQueueSlice = ToQueueSlice;
  onToWebSendTelemetry = ToWebSendTelemetryCallback;
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

void UDPSenderComand(const ActionProt& actionProt) {
  Udp.beginPacket(targetIP, UDPPort);
  Udp.write((uint8_t*)&actionProt, sizeof(actionProt));
  Udp.endPacket();
}

void UDPReceiver() {
  
  while (true) {
    int packetSize = Udp.parsePacket();
    if (packetSize > 0) {
      UDPPkt pkt;
    
      pkt.len = Udp.read(pkt.data, sizeof(pkt.data));

      if (pkt.len > 0) {
        uint8_t protType = pkt.data[0];

        if (protType == PROT_TELEMETRY) {
          //if packet is correct and have some data
          if (pkt.len > sizeof(TelemetryProt)) {
          
            if (onToWebSendTelemetry != nullptr){
              onToWebSendTelemetry(pkt);
            }

          } else {
            Serial.print("UDP package size does not match: ");
            Serial.println(packetSize);
          }

        } else if (protType == PROT_VIDEO) {
          //if packet is correct and have some data
          if (packetSize > sizeof(FrameProt)) {
            if (onToQueueSlice != nullptr) {
              //FrameProt* prot = (FrameProt*)pkt.data;
              //DebugPacketHeader(*prot);
              onToQueueSlice(pkt.data, pkt.len);
            }
          } else {
            Serial.print("UDP package size does not match: ");
            Serial.println(packetSize);
          }
        } else {
          Serial.println("Unknown UDP protocol: ");
          Serial.print(protType);
        }
      }
    }

    vTaskDelay(pdMS_TO_TICKS(1));
  }


/*
  uint8_t buffer[2048];

  while (true) {
    int packetSize = Udp.parsePacket();
    if (packetSize) {
      int len = Udp.read(buffer, sizeof(buffer));

      if (buffer[0] == '#') {
        //telemetry message

        int textSize = (len < sizeof(buffer)) ? len : sizeof(buffer) - 1;
        buffer[textSize] = '\0'; 

        String stringRecebida = String((char*)buffer);

        if (onToWebSendTelemetry != nullptr){
          onToWebSendTelemetry(stringRecebida);
        }

      }else if (len >= sizeof(FrameProt)) {
        //frame data
        if (onToQueueSlice != nullptr) {
          //FrameProt* prot = (FrameProt*)buffer;
          //DebugPacketHeader(*header);
          onToQueueSlice(buffer, len);
        }

      } else {
        Serial.println("Header is too short or incomplete");
      }
    }

    vTaskDelay(pdMS_TO_TICKS(1));
  }*/
}

#endif