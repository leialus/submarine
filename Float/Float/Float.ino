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
byte macServer[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x01 };

// Configuração de IP Estático (Servidor)
IPAddress ipServer(192, 168, 1, 10);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

W5500Driver driver(W5500_CS);
EthernetUDP Udp;
unsigned int localPort = 8888;  // Porta para escutar

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
  Ethernet.begin(macServer, ipServer, gateway, subnet);

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
  if (Udp.begin(localPort)) {
    Serial.print("Servidor UDP iniciado na porta ");
    Serial.println(localPort);
    Serial.print("IP: ");
    Serial.println(Ethernet.localIP());
  } else {
    Serial.println("Falha ao iniciar UDP.");
    while (1);
  }
}

void loop() {
  // Verifica se chegou um pacote UDP
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    Serial.print("Recebido pacote de ");
    Serial.print(Udp.remoteIP());
    Serial.print(":");
    Serial.println(Udp.remotePort());

    // Lê o pacote para um buffer
    char buffer[255];
    int len = Udp.read(buffer, 255);
    if (len > 0) {
      buffer[len] = 0; // finaliza a string
      Serial.print("Conteúdo: ");
      Serial.println(buffer);
    }

    // Envia uma resposta de volta para o remetente
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.print("Mensagem recebida!");
    Udp.endPacket();
  }
}