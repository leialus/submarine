#include <SPI.h>
#include <EthernetESP32.h>

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

// Cria o servidor na porta 8080
EthernetServer server(8080);
W5500Driver driver(W5500_CS); // Inicializa o driver com o pino CS

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

  // Inicia o servidor
  server.begin();
  Serial.print("Servidor rodando em: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  // Verifica se há um novo cliente
  EthernetClient client = server.available();
  
  if (client) {
    Serial.println("Novo cliente conectado!");

    // Enquanto o cliente estiver conectado, processa as mensagens
    while (client.connected()) {
      if (client.available()) {
        // Lê a mensagem completa (até quebra de linha)
        String msg = client.readStringUntil('\n');
        msg.trim();
        Serial.print("Mensagem recebida: ");
        Serial.println(msg);

        // Responde imediatamente
        client.println("msg recebida");
        Serial.println("Resposta enviada: msg recebida");
      }
      // Pequeno delay para não sobrecarregar o loop
      delay(10);
    }

    // Quando o cliente se desconectar
    Serial.println("Cliente desconectado.");
    client.stop();
  }
}