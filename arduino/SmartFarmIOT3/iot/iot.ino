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

    /*int i_data = NodeSerial.parseInt(); 
    int i_data2 = NodeSerial.parseInt(); 
   // int i_data3 = NodeSerial.parseInt(); 

    float f_data = NodeSerial.parseFloat(); 

    if (NodeSerial.read() == '\n') 

    {

          Serial.print("NodeMCU or ESP8266"); Serial.print(" : "); 

          Serial.print(i_data); Serial.print(" : "); 
          Serial.print(i_data2); Serial.print(" : "); 
         // Serial.print(i_data3); Serial.print(" : "); 

          Serial.println(f_data);  

    }*/

    String result = NodeSerial.readStringUntil('\n');
    Serial.println(result); 

    delay(100);

  }

  delay(1000);

}
