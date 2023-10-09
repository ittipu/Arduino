#include <SPI.h>
#include <LoRa.h>
#include <dht.h>
#include <ArduinoJson.h>

#define DHT11_PIN 4


StaticJsonDocument<256> doc;

dht DHT;

int counter = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Sender");

  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  int chk = DHT.read11(DHT11_PIN);
  int hum = DHT.humidity;
  int temp = DHT.temperature;

  char packet[256];
  doc["temp"] = temp;
  doc["hum"] = hum;
  
  Serial.println("Sending packet: ");
  serializeJson(doc, Serial);
  Serial.println();
  serializeJson(doc, packet);

  // send packet
  LoRa.beginPacket();
  LoRa.print(packet);
  LoRa.endPacket();

  counter++;

  delay(5000);
}
