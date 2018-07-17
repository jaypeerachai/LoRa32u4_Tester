#include <SPI.h>
#include <RH_RF95.h>
 
// Set pin for Feather32u4 
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
#define LED 13
 
// set frequency of transmission to 434.0, must match TX's frequency.
#define RF95_FREQ 434.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);
 
void setup()
{
  Serial.begin(9600);
  
  pinMode(LED, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  while (!Serial) {
    delay(1);
  }
  delay(100);
  
  Serial.println("Feather LoRa RX Test!");
 
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(100);
  digitalWrite(RFM95_RST, HIGH);
  delay(100);

  // check driver's initialisation
  while(!rf95.init()) 
  {
    Serial.println("LoRa radio init failed, try again!");
  }
  Serial.println("LoRa radio init OK!");

  // check receiver's frequency
  while(!rf95.setFrequency(RF95_FREQ)) 
  {
    Serial.println("set frequency failed, try again!");
  }
  Serial.print("Set Freq to: "); 
  Serial.println(RF95_FREQ);

  // Set the transmitter power output level
  rf95.setTxPower(23, false);
}

int i = 1; // counter
 
void loop()
{
  if (rf95.available())
  {
    // check package
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len))
    {
      digitalWrite(LED, HIGH);
      RH_RF95::printBuffer("Received: ", buf, len);
      Serial.print("Got: ");
      Serial.println((char*)buf);
 
      // Send an ACK back to transmitter
      Serial.println("Preparing ACK...");
      String ii = String(i);
      uint8_t ACK[] = "Thanks for package";
      rf95.send(ACK, sizeof(ACK));
      rf95.waitPacketSent();
      Serial.println("Sent an ACK");
      digitalWrite(LED, LOW);
      i++;
    }
    else
    {
      Serial.println("Receive package failed");
    }
  }
}
