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
const int serverPort = 8080;

W5500Driver driver(W5500_CS);

// Cliente global para manter a conexão
EthernetClient client;
bool conectado = false;

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

  // Tenta conectar pela primeira vez
  conectarAoServidor();
}

void loop() {
  // Se não estiver conectado, tenta reconectar
  if (!conectado || !client.connected()) {
    Serial.println("Conexão perdida. Tentando reconectar...");
    client.stop();
    conectarAoServidor();
    delay(2000);
    return;
  }

  // Se está conectado, envia a mensagem
  enviarMensagem("oi servidor");

  // Aguarda a resposta do servidor (com timeout de 2 segundos)
  String resposta = "";
  unsigned long timeout = millis() + 2000;
  while (client.connected() && millis() < timeout) {
    if (client.available()) {
      resposta = client.readString();
      break;
    }
  }

  if (resposta.length() > 0) {
    Serial.print("Resposta do servidor: ");
    Serial.println(resposta);
  } else {
    Serial.println("Timeout: servidor não respondeu.");
  }

  // Aguarda 2 segundos antes de enviar novamente
  delay(2000);
}

// Função para tentar conectar ao servidor
void conectarAoServidor() {
  Serial.print("Conectando ao servidor ");
  Serial.print(serverIp);
  Serial.print(":");
  Serial.println(serverPort);

  if (client.connect(serverIp, serverPort)) {
    Serial.println("Conectado ao servidor!");
    conectado = true;
  } else {
    Serial.println("Falha na conexão. Tentando novamente em 2s...");
    conectado = false;
    delay(2000);
  }
}

// Função para enviar uma mensagem
void enviarMensagem(String msg) {
  if (client.connected()) {
    client.println(msg);
    Serial.print("Enviado: ");
    Serial.println(msg);
  } else {
    Serial.println("Erro: cliente não conectado.");
    conectado = false;
  }
}