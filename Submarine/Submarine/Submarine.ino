#include <SPI.h>
#include <EthernetESP32.h>

// Definição dos pinos (iguais aos do servidor, mas pode ser outro ESP)
#define W5500_SCLK 14
#define W5500_CS   45
#define W5500_MOSI 21
#define W5500_MISO 47
#define W5500_RST  48

// MAC e IP do cliente (diferentes do servidor)
byte macClient[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x02 };
IPAddress ipClient(192, 168, 1, 20);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// IP e porta do servidor
IPAddress serverIp(192, 168, 1, 10);
const int localPort = 8888;

W5500Driver driver(W5500_CS);
EthernetUDP Udp;

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  Serial.println("\n=== Cliente W5500 ===");

  // Reset do W5500
  pinMode(W5500_RST, OUTPUT);
  digitalWrite(W5500_RST, LOW);
  delay(10);
  digitalWrite(W5500_RST, HIGH);
  delay(100);

  // Inicializa SPI
  SPI.begin(W5500_SCLK, W5500_MISO, W5500_MOSI, W5500_CS);

  // Inicializa Ethernet
  Ethernet.init(driver);

  // Configura IP estático
  Ethernet.begin(macClient, ipClient, gateway, subnet);

  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("ERRO: W5500 não encontrado!");
    while (true) delay(1);
  }

  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Cabo desconectado!");
  } else {
    Serial.println("Cabo conectado.");
  }

  Serial.print("IP do Cliente: ");
  Serial.println(Ethernet.localIP());

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
  // 1. Envia a mensagem
  Udp.beginPacket(serverIp, localPort);
  Udp.print("oi servidor");
  Udp.endPacket();
  Serial.println("Mensagem enviada");

  // 2. Aguarda a resposta (com timeout)
  unsigned long timeout = millis() + 2000;
  while (millis() < timeout) {
    int packetSize = Udp.parsePacket();
    if (packetSize) {
      char buffer[255];
      int len = Udp.read(buffer, 255);
      if (len > 0) {
        buffer[len] = 0;
        Serial.print("Resposta do servidor: ");
        Serial.println(buffer);
      }
      break;
    }
  }

  delay(2000);
}