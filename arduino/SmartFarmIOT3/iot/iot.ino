#include <ESP8266WiFi.h>

const char* ssid = "BENZ";
const char* password = "0850299990";

#include <SoftwareSerial.h>

SoftwareSerial NodeSerial(D2,D3); // RX | TX

String a;

void setup() {

  Serial.begin(115200);

  pinMode(D2, INPUT); 
  pinMode(D3, OUTPUT); 


  NodeSerial.begin(57600);

}

void loop() {

  while (NodeSerial.available() > 0) 
  {

   float f_data = NodeSerial.parseFloat(); 

    if (NodeSerial.read() == '\n') 
    {

          Serial.print("NodeMCU or ESP8266"); Serial.print(" : "); 
          Serial.println(f_data);  

    }

    delay(50);

  }

  delay(100);

}
