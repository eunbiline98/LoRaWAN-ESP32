#include "LoRaGWPhiloin.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

const char *SSID = "BUSOL";
const char *PASS = "$ut0h0m312";
const char *MQTT_SERVER = "192.168.8.144";
const char *MQTT_STATUS = "tele/loraWAN/eth/LWT";

const char *DATA_CMD = "loraWAN/nodes/cmnd";
const char *DATA_STATE = "loraWAN/nodes/stat";

const char *MQTT_CLIENT_ID = "LoRaWAN-01PaditechSBY";
const char *MQTT_USERNAME = "padinet";
const char *MQTT_PASSWORD = "p4d1n3t";

// counter reset ESP
int ResetCounter = 0;

String id, Data, Data1, Data2;

IPAddress ip(192, 168, 8, 205);
IPAddress gateway(19, 168, 8, 1);   // Sesuaikan dengan gateway AP
IPAddress subnet(255, 255, 255, 0); // Sesuaikan dengan subnet AP\

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASS);
  WiFi.config(ip, gateway, subnet);

  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(led_eth, HIGH);
    delay(100);
    digitalWrite(led_eth, LOW);
    delay(100);
  }
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    digitalWrite(led_eth, HIGH);
    delay(5000);
    ESP.restart();
  }
  digitalWrite(led_eth, LOW);
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
    Serial.print("001, ");
    Serial.println(Data1);
  }
  if (id == "002")
  {
    Data2 = Data;
    Serial.print("002, ");
    Serial.println(Data2);
  }
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
      client.publish(MQTT_STATUS, "Online", true);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      digitalWrite(led_eth, HIGH);
      delay(1000);
      digitalWrite(led_eth, LOW);
      delay(1000);
      // Wait 5 seconds before retrying
      ResetCounter++;
      if (ResetCounter >= 200)
      {
        ESP.restart();
        ResetCounter = 0;
      }
    }
  }
}

void setup()
{
  setupSerial();
  setup_wifi();

  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  // ArduinoOTA.setHostname("myesp32");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]()
                     {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type); })
      .onEnd([]()
             { Serial.println("\nEnd"); })
      .onProgress([](unsigned int progress, unsigned int total)
                  { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
      .onError([](ota_error_t error)
               {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed"); });

  ArduinoOTA.begin();

  client.setServer(MQTT_SERVER, 1883);
  setupLoRa();
  LoRa.onReceive(onReceive);
  LoRa_rxMode();
  // client.setCallback(callback);
}

void loop()
{
  ArduinoOTA.handle();
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
  if (runEvery(2500))
  { // repeat every 5000 millis
    Serial.println("LoRa-Gateway:01,Site->Paditech SBY");
    Serial.print("01: ");
    Serial.println(Data1);
    Serial.print("02: ");
    Serial.println(Data2);
  }
}
