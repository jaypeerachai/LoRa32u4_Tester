#include <SPI.h>
#include <RH_RF95.h>

// Set pin for Feather32u4 
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7

#define trigPin 2
#define echoPin 3

// set frequency of transmission to 434.0, must match RX's frequency.
#define RF95_FREQ 434.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() 
{
  Serial.begin(9600);
  
  pinMode(RFM95_RST, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(echoPin, HIGH);
  digitalWrite(RFM95_RST, HIGH);
  
  Serial.println("Feather LoRa TX Test!");

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

  // check Transmitter's frequency
  while(!rf95.setFrequency(RF95_FREQ)) 
  {
    Serial.println("set frequency failed, try again!");
  }
  Serial.print("Set Freq to: "); 
  Serial.println(RF95_FREQ);
  
  // Sets the transmitter power output level
  rf95.setTxPower(23, false);
}

int i = 1; // counter

void loop()
{
  delay(1000); // Wait 1 second between transmits, could also 'sleep' here!
  char charBuf[25];
  float distance = distanceCal();
  String ii = String(i);
  String temp = "distance "+ii;
  String temp2 = " : ";
  String cm = " cm";
  String package = temp+temp2+String(distance)+cm;
  package.toCharArray(charBuf,package.length()+1);
  Serial.print("Sending \""); Serial.println(package+"\"");

  // If there is no ack back, it will send current package again.
  while(true)
  {
    Serial.println("Sending...");
    delay(100);
    rf95.send((uint8_t *)charBuf, sizeof(charBuf));
    delay(100);
    rf95.waitPacketSent();
    
    // Now wait for an ACK
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    Serial.println("Waiting for ACK...");
    if (rf95.waitAvailableTimeout(10000))
    { 
      // check ACK
      if (rf95.recv(buf, &len))
     {
        Serial.print("Got ACK: ");
        Serial.println((char*)buf);
        i++;
        break;
      }
      else
      {
        Serial.println("Receive ACK failed");
      }
    }
    else
    {
      Serial.println("No ACK, Send "+temp+" again");
    }
  }
}

// find distance
float distanceCal()
{
  float duration, distance;
  digitalWrite(trigPin, LOW);     
  delayMicroseconds(2);           
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);          
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2.0) / 29.1;
  return distance;
}

