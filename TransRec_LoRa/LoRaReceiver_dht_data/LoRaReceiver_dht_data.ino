#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>


StaticJsonDocument<256> doc;


void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Gate way");

  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  char new_message[256];
  String recvPacket = "";
  
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
//    Serial.print("Received packet: ");

    // read packet
    while (LoRa.available()) {
      recvPacket += ((char)LoRa.read());
    }
    deserializeJson(doc, recvPacket);
    
    int lora_rssi = LoRa.packetRssi();
    doc["rssi"] = lora_rssi;
    serializeJson(doc, Serial);
    Serial.println();
  }
}
