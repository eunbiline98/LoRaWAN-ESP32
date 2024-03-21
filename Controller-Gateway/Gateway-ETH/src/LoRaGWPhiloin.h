#include <SPI.h> // include libraries
#include <LoRa.h>

// #define csPin 15     // LoRa radio chip select
// #define resetPin 26 // LoRa radio reset
// #define irqPin 15   // change for your board; must be a hardware interrupt pin (DIO0)

#define csPin 5     // LoRa radio chip select
#define resetPin 21 // LoRa radio reset
#define irqPin 22   // change for your board; must be a hardware interrupt pin (DIO0)

// Led Indicator
#define led_data 2
#define led_eth 4

#define BAUD_RATE 115200

const long frequency = 433E6; // LoRa Frequency

void setupSerial()
{
      Serial.begin(BAUD_RATE);
      Serial.println("[SERIAL] Setup Serial. Done.");
      pinMode(led_data, OUTPUT);
      pinMode(led_eth, OUTPUT);
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
            // Serial.println("LoRa init failed. Check your connections.");
            digitalWrite(led_data, HIGH);
            while (true)
                  ; // if failed, do nothing
      }
      digitalWrite(led_data, LOW);
      Serial.println("LoRa init succeeded.");
      Serial.println();
      Serial.println("LoRa Gateway");
      Serial.println("Only receive messages from nodes");
      Serial.println("Tx: invertIQ enable");
      Serial.println("Rx: invertIQ disable");
      Serial.println();
}

void LoRa_rxMode()
{
      LoRa.disableInvertIQ(); // normal mode
      LoRa.receive();         // set receive mode
}

void LoRa_txMode()
{
      LoRa.idle();           // set standby mode
      LoRa.enableInvertIQ(); // active invert I and Q signals
}

void LoRa_sendMessage(String message)
{
      LoRa_txMode();       // set tx mode
      LoRa.beginPacket();  // start packet
      LoRa.print(message); // add payload
      LoRa.endPacket();    // finish packet and send it
      LoRa_rxMode();       // set rx mode
}