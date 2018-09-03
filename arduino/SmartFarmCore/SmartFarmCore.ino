#include "DHT.h"
 
#define DHTPIN 2 
 
#define DHTTYPE DHT11  
 
int sensorPin = A0;
int RELAY1 = 7;
int RELAY2 = 8;
 
DHT dht(DHTPIN, DHTTYPE);
 
void setup() {
  Serial.begin(9600);
  dht.begin();
  
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
}
 
void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int sensorValue;
  
  sensorValue = analogRead(sensorPin);
  sensorValue = map(sensorValue, 0, 1023, 100, 0);
  Serial.print("Soil moisture: ");
  Serial.print(sensorValue);
  Serial.println(" %");
  
  Serial.print("Humidity: "); 
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(t);
  Serial.println(" *C");
 
  if(sensorValue < 50 && t > 29){
    digitalWrite(RELAY1, HIGH);
    digitalWrite(RELAY2, HIGH);
  }
  else{
    digitalWrite(RELAY1, LOW);
    digitalWrite(RELAY2, LOW);
  }
  if(sensorValue < 50){
    digitalWrite(RELAY1, HIGH);
  }
  else{
    digitalWrite(RELAY1, LOW);
  }
  if(t > 30){
    digitalWrite(RELAY2, HIGH);
  }
  else{
    digitalWrite(RELAY2, LOW);
  }
  
  delay(1000); //wait for half a second, so it is easier to read
}