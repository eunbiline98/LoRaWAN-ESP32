#include <SPI.h> // include libraries
#include <LoRa.h>

// #define csPin 15     // LoRa radio chip select
// #define resetPin 26 // LoRa radio reset
// #define irqPin 15   // change for your board; must be a hardware interrupt pin (DIO0)

#define csPin 5     // LoRa radio chip select
#define resetPin 21 // LoRa radio reset
#define irqPin 22   // change for your board; must be a hardware interrupt pin (DIO0)

#define BAUD_RATE 115200

#define led_data 2

const long frequency = 433E6; // LoRa Frequency

void setupSerial()
{
      Serial.begin(BAUD_RATE);
      pinMode(led_data, OUTPUT);
      Serial.println("[SERIAL] Setup Serial. Done.");
}

void testSerial()
{
      Serial.println("[Serial] If you see this, serial is PASSED.");
}

void setupLoRa()
{
      while (!Serial)
            ;

      LoRa.setPins(csPin, resetPin, irqPin);

      if (!LoRa.begin(frequency))
      {
            Serial.println("LoRa init failed. Check your connections.");
            digitalWrite(led_data, HIGH);
            while (true)
                  ; // if failed, do nothing
      }
      Serial.println("LoRa init succeeded.");
}

void LoRa_rxMode()
{
      LoRa.enableInvertIQ(); // active invert I and Q signals
      LoRa.receive();        // set receive mode
}

void LoRa_txMode()
{
      LoRa.idle();            // set standby mode
      LoRa.disableInvertIQ(); // normal mode
}

void LoRa_sendMessage(String message)
{
      LoRa_txMode();       // set tx mode
      LoRa.beginPacket();  // start packet
      LoRa.print(message); // add payload
      LoRa.endPacket();    // finish packet and send it
      LoRa_rxMode();       // set rx mode
}