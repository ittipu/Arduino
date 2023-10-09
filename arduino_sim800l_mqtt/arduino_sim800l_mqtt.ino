#define TINY_GSM_MODEM_SIM800

#include <SoftwareSerial.h>
#include <TinyGsmClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <TimeLib.h>

SoftwareSerial SerialAT(3, 2); // RX, TX

//Network details
const char apn[]  = "internet";
const char user[] = "";
const char pass[] = "";

// MQTT details
const char* broker = "172.104.56.240";
const char* topicOut = "test/out";
const char* topicIn = "test/in";

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
PubSubClient mqtt(client);
uint32_t lastReconnectAttempt = 0;
long lastMsg = 0;
float lat = 0;
float lng = 0;

StaticJsonDocument<256> doc;
unsigned long timestamp;



boolean mqttConnect() {
  Serial.print("Connecting to ");
  Serial.print(broker);

  boolean status = mqtt.connect("GsmClientN");

  if (status == false) {
    Serial.println(" fail");
    return false;
  }
  Serial.println(" success");
  mqtt.subscribe(topicIn);
  return mqtt.connected();
}

void mqttCallback(char* topic, byte* message, unsigned int len) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < len; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
}

void setup()
{
  Serial.begin(115200);
  SerialAT.begin(9600);

  Serial.println("System start.");
  modem.restart();
  Serial.println("Modem: " + modem.getModemInfo());
  Serial.println("Searching for telco provider.");
  if (!modem.waitForNetwork())
  {
    Serial.println("fail");
    while (true);
  }
  Serial.println("Connected to telco.");
  Serial.println("Signal Quality: " + String(modem.getSignalQuality()));

  Serial.println("Connecting to GPRS network.");
  if (!modem.gprsConnect(apn, user, pass))
  {
    Serial.println("fail");
    while (true);
  }
  Serial.println("Connected to GPRS: " + String(apn));

  mqtt.setServer(broker, 1883);
  mqtt.setCallback(mqttCallback);
}

void loop() {
  if (!mqtt.connected()) {
    Serial.println("MQTT NOT CONNECTED");
    // Reconnect every 10 seconds
    uint32_t t = millis();
    if (t - lastReconnectAttempt > 1000L) {
      lastReconnectAttempt = t;
      if (mqttConnect()) {
        lastReconnectAttempt = 0;
      }
    }
    delay(100);
    return;
  }
  long now = millis();
  if (now - lastMsg > 10000) {
    lastMsg = now;
    char buffer[256];
    doc["timestamp"] = lastMsg;
    Serial.print("Publish to broker: ");
    serializeJson(doc, Serial);
    Serial.println();
    serializeJson(doc, buffer);
    mqtt.publish(topicOut, buffer);
  }
  mqtt.loop();
}



int get_timestamp() {
  int   year3    = 0;
  int   month3   = 0;
  int   day3     = 0;
  int   hour3    = 0;
  int   min3     = 0;
  int   sec3     = 0;
  float timezone = 0;
  for (int8_t i = 5; i; i--) {
    DBG("Requesting current network time");
    if (modem.getNetworkTime(&year3, &month3, &day3, &hour3, &min3, &sec3,
                             &timezone)) {
      DBG("Year:", year3, "\tMonth:", month3, "\tDay:", day3);
      DBG("Hour:", hour3, "\tMinute:", min3, "\tSecond:", sec3);
      DBG("Timezone:", timezone);
      break;
    } else {
      DBG("Couldn't get network time, retrying in 15s.");
      delay(15000L);
    }
  }

  setTime(hour3, min3, sec3, day3, month3, year3);
  Serial.print("Timestamp: ");
  int ct = now();
  Serial.println(ct);
  return ct;
}
