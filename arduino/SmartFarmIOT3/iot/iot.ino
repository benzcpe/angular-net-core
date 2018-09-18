#include <ESP8266WiFi.h>

const char* ssid = "BENZ";
const char* password = "0850299990";

#include <SoftwareSerial.h>

SoftwareSerial chat(D2, D3); // RX, TX

String a;

void setup() {

  Serial.begin(9600);

  chat.begin(4800);

 

  delay(10);

}

void loop() {

Serial.println("Please wait Serial..");

  while (a == "") {

   // chat.print("Question1"); // ส่งหัวข้อคำถาม ว่า Question1 ไปยัง Arduino

    a = chat.readString();  // อ่าน Serial และนำไปเก็บในตัวแปร A

    if(a != ""){
    Serial.println(a);

  

   // Serial.print(".");
  }

   delay(100);

  }

  a = "";
  delay(1000);

}
