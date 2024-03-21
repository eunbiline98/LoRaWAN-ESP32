#include "LoRaPhiloin.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <DHT_U.h>

// DHT PIN
#define DHTPIN 25
#define DHTTYPE DHT22

float tempRead;
float humdRead;

char buffer[256];

DHT dht(DHTPIN, DHTTYPE);

void onReceive(int packetSize)
{
  String message = "";
  while (LoRa.available())
  {
    message += (char)LoRa.read();
  }
  Serial.print("Node Receive: ");
  Serial.println(message);
}
boolean runEvery(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}

void DHT_config()
{
  tempRead = dht.readTemperature();
  humdRead = dht.readHumidity();

  if (isnan(tempRead) || isnan(humdRead))
  {
    return;
  }
  JsonDocument doc;
  JsonArray data = doc["DHT"].to<JsonArray>();
  data.add(tempRead);
  data.add(humdRead);

  serializeJson(doc, buffer);
  serializeJsonPretty(doc, Serial);
}

void setup()
{
  setupSerial();
  testSerial();
  dht.begin();
  setupLoRa();
  LoRa.onReceive(onReceive);
  LoRa_rxMode();
  Serial.println("LoRa Nodes-1");
}

void loop()
{
  DHT_config();
  if (runEvery(2500))
  {                            // repeat every 1000 millis
    String message = "001,";   // id
    message += buffer;         // kirim data millis,
    message += "#";            // tanda akhir data
    LoRa_sendMessage(message); // send a message
  }
}
