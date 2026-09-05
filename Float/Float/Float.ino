#include <SPI.h>
#include <EthernetESP32.h>
#include <EthernetUdp.h>

// Definição explícita de todos os pinos SPI para o ESP32-S3
#define W5500_SCLK 46
#define W5500_CS   9
#define W5500_MOSI 10
#define W5500_MISO 11
#define W5500_RST  12
#define W5500_INT  13

// Endereço MAC do Servidor (DEVE ser único)
byte myMacAddress[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x01 };

// Configuração de IP Estático (Servidor)
IPAddress localIP(192, 168, 1, 10);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// IP e porta do servidor
IPAddress clientIp(192, 168, 1, 20);
unsigned int UDPPort = 8888;

W5500Driver driver(W5500_CS);
EthernetUDP Udp;

unsigned long lastPingTime = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  Serial.println("\nInicializando Servidor W5500...");

  // Reset do W5500
  pinMode(W5500_RST, OUTPUT);
  digitalWrite(W5500_RST, LOW);
  delay(10);
  digitalWrite(W5500_RST, HIGH);
  delay(100);

  // Inicializa o barramento SPI com os pinos desejados (opcional, mas recomendado)
  SPI.begin(W5500_SCLK, W5500_MISO, W5500_MOSI, W5500_CS);

  // Passa o driver para a biblioteca Ethernet
  Ethernet.init(driver);
  
  // Inicia a Ethernet com IP estático
  Ethernet.begin(myMacAddress, localIP, gateway, subnet);

  // Verifica se o hardware foi encontrado
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("ERRO: Hardware W5500 não encontrado!");
    while (true) { delay(1); }
  }

  // Verifica se o cabo está conectado
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("ERRO: Cabo Ethernet desconectado!");
  } else {
    Serial.println("Cabo Ethernet conectado.");
  }

    // Inicia o serviço UDP
  if (Udp.begin(UDPPort)) {
    Serial.print("Servidor UDP iniciado na porta ");
    Serial.println(UDPPort);
    Serial.print("IP: ");
    Serial.println(Ethernet.localIP());
  } else {
    Serial.println("Falha ao iniciar UDP.");
    while (1);
  }
}

void loop() {
   // --- 1. Processa pacotes recebidos ---
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

    // Responde imediatamente (como já fazia)
    // Udp.beginPacket(clientIp, UDPPort);
    // Udp.print("Mensagem recebida do submarine!");
    // Udp.endPacket();
    // Serial.println("Resposta enviada.");
  }

  // --- 2. Envia uma mensagem ativa para o cliente a cada 5 segundos ---
  if ( millis() - lastPingTime >= 5000) {
    lastPingTime = millis();
    
    Udp.beginPacket(clientIp, UDPPort);
    Udp.print("Ping do submarine!");
    Udp.endPacket();
    Serial.print("Ping enviado para ");
    Serial.print(clientIp);
    Serial.print(":");
    Serial.println(UDPPort);
  }
}
