#include <ESP8266WiFi.h>

const char* ssid = "BENZ";
const char* password = "0850299990";

#include <SoftwareSerial.h>

SoftwareSerial NodeSerial(D2,D3); // RX | TX

#define turn_On 0
#define turn_Off 1
int RELAY1 = D5;
int RELAY2 = D6;

String a;

void setup() {

  Serial.begin(115200);

  pinMode(D2, INPUT); 
  pinMode(D3, OUTPUT); 

  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  

  NodeSerial.begin(57600);

}

void loop() {

  while (NodeSerial.available() > 0) 
  {
  
    String serialResponse = NodeSerial.readStringUntil('\n');
  // Convert from String Object to String.
  char sz[] = "100;100;100;7.0;100;1000;20.00";

    char buf[sizeof(sz)];
    serialResponse.toCharArray(buf, sizeof(buf));
    char *p = buf;
    char *str;
    int i = 0;
    int m = 0;
    while ((str = strtok_r(p, ";", &p)) != NULL) // delimiter is the semicolon
    {
     // Serial.println(str);

      if(i == 2){
        m = String(str).toInt();
      }
      

      i++;
    }


    if(m < 20)
    {
   // Serial.println(result); 
    digitalWrite(RELAY1, turn_On);
    digitalWrite(RELAY2, turn_On);
    }
    else
    {
       digitalWrite(RELAY1, turn_Off);
      digitalWrite(RELAY2, turn_Off);
    }
  

    Serial.println(serialResponse);
    delay(100);

  }

  delay(1000);

}
