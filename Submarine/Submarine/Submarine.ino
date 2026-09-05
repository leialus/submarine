#include "UDPConnection.h"


void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  UDPInit();
}

void loop() {
  UDPReceiver();
  UDPSender();

}
