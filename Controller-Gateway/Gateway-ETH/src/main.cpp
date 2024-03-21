#include "LoRaGWPhiloin.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <EthernetSPI2.h>
#include <SPI.h>

const char *MQTT_SERVER = "192.168.8.144";
const char *MQTT_STATUS = "tele/loraWAN/eth/LWT";

const char *DATA1_STATE = "loraWAN/nodes1/stat";
const char *DATA2_STATE = "loraWAN/nodes2/stat";
const char *DATA_STATE = "loraWAN/Gateway/sby/stat";
const char *POWER_KILL_CMD = "loraWAN/rst/cmnd";

const char *MQTT_CLIENT_ID = "LoRaWAN-01PaditechSBY";
const char *MQTT_USERNAME = "padinet";
const char *MQTT_PASSWORD = "p4d1n3t";

int ResetCounter = 0;

String id, Data, Data1, Data2;
String rssi_1, rssi_2;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {0x02, 0xAB, 0xCD, 0xEF, 0x00, 0x01};
IPAddress ip(192, 168, 8, 159);
IPAddress dns(8, 8, 8, 8);
IPAddress gateway(192, 168, 8, 1);
IPAddress subnet(255, 255, 255, 0);

EthernetClient ethClient;
PubSubClient client(ethClient);

void setup_eth()
{
  Ethernet.init(15);
  Ethernet.begin(mac, ip, dns, gateway, subnet);
  // Ethernet.begin(mac);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware)
  {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true)
    {
      Serial.print(".");
      digitalWrite(led_eth, HIGH);
      delay(100);
      digitalWrite(led_eth, LOW);
      delay(100);
    }
  }
  while (Ethernet.linkStatus() == LinkOFF)
  {
    Serial.println("Ethernet cable is NOT connected.");
    digitalWrite(led_eth, HIGH);
    delay(250);
    digitalWrite(led_eth, LOW);
    delay(250);
  }

  Serial.println("Ethernet cable is now connected.");
  digitalWrite(led_eth, HIGH);
  delay(500);
  // print ip address
  Serial.print("Local IP : ");
  Serial.println(Ethernet.localIP());
  Serial.print("Subnet Mask : ");
  Serial.println(Ethernet.subnetMask());
  Serial.print("Gateway IP : ");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("DNS Server : ");
  Serial.println(Ethernet.dnsServerIP());
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

void onReceive(int packetSize)
{
  int len_ID, len_DT;
  String message = "";

  while (LoRa.available())
  {
    message += (char)LoRa.read();
  }

  len_ID = message.indexOf(",");
  len_DT = message.indexOf("#");

  id = message.substring(0, len_ID);            // parsing id
  Data = message.substring(len_ID + 1, len_DT); // parsing data

  if (id == "001")
  {
    Data1 = Data;
    rssi_1 = String(LoRa.packetRssi()).c_str();
  }
  if (id == "002")
  {
    Data2 = Data;
    rssi_2 = String(LoRa.packetRssi()).c_str();
  }
}

void callback(char *topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  String payload;

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    Serial.println();
    if ((char)message[i] != '"')
      payload += (char)message[i];
  }

  if (String(topic) == POWER_KILL_CMD)
  {
    if (payload == "0")
    {
    }
    else if (payload == "1")
    {
      ESP.restart();
    }
  }
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD, MQTT_STATUS, 1, 1, "Offline"))
    {
      Serial.println("connected");
      // subscribe
      client.subscribe(POWER_KILL_CMD);
      client.publish(MQTT_STATUS, "Online", true);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      ResetCounter++;
      if (ResetCounter >= 200)
      {
        ESP.restart();
        ResetCounter = 0;
      }
      digitalWrite(led_eth, HIGH);
      delay(2500);
      digitalWrite(led_eth, LOW);
      delay(2500);
    }
  }
}

void setup()
{
  setupSerial();
  testSerial();
  setupLoRa();
  LoRa.onReceive(onReceive);
  LoRa_rxMode();
  delay(1000);

  setup_eth();
  client.setCallback(callback);
  client.setServer(MQTT_SERVER, 1883);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
  if (runEvery(2500))
  { // repeat every 5000 millis
    Serial.print("LoRa-Gateway:");
    Serial.print(Ethernet.localIP());
    Serial.println(",Site->Paditech SBY");

    JsonDocument doc;

    doc["data_1"] = Data1;
    doc["data_2"] = Data2;
    doc["rssi_1"] = rssi_1;
    doc["rssi_2"] = rssi_2;

    char buffer[256];
    size_t n = serializeJson(doc, buffer);
    serializeJsonPretty(doc, Serial);

    if (client.publish(DATA_STATE, buffer, n) == true)
    {
      Serial.println("Success sending message");
      digitalWrite(led_data, HIGH);
      delay(100);
    }
    else
    {
      Serial.println("Error sending message");
      digitalWrite(led_data, HIGH);
      delay(50);
      digitalWrite(led_data, LOW);
      delay(50);
    }
  }
  digitalWrite(led_data, LOW);
}