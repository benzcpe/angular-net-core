#include <ESP8266WiFi.h>


#include "dht.h"

dht DHT;

#define D0 16
#define D1 5 // I2C Bus SCL (clock)
#define D2 4 // I2C Bus SDA (data)
#define D3 0
#define D4 2 // Same as "LED_BUILTIN", but inverted logic
#define D5 14 // SPI Bus SCK (clock)
#define D6 12 // SPI Bus MISO 
#define D7 13 // SPI Bus MOSI
#define D8 15 // SPI Bus SS (CS)
#define D9 3 // RX0 (Serial console)
#define D10 1 // TX0 (Serial console)

#define turn_On 0
#define turn_Off 1

#define DHT11_PIN D4

int sensorPin = A0;
int sensorValue;

int RELAY1 = D1;
int RELAY2 = D2;


// WIFI
const char* ssid     = "BENZ";    // SSID Wifi
const char* password = "0850299990";   // Password Wifi

const char* host = "api.thingspeak.com";    // Host ของ thingspeak ไม่ต้องแก้ไข
const char* api   = "26CNRNMR7HEABR4K";  //API Key ที่เราจำไว้ ในขั้นต้นเมื่อกี้


void setup()
{
  Serial.begin(9600);
  
   // We start by connecting to a WiFi network
  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
   Serial.println();
   
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  


  Serial.println();
  Serial.println("Smart Farm IOT");
  Serial.println("Type,\tstatus,\tHumidity (%),\tTemperature (C),\tSoil moisture (%)");
}

void loop()
{
  delay(5000);
    
  // READ DATA
  Serial.print("DHT11, \t");
  int chk = DHT.read11(DHT11_PIN);
  switch (chk)
  {
    case DHTLIB_OK:  
    Serial.print("OK,\t"); 
    break;
    case DHTLIB_ERROR_CHECKSUM: 
    Serial.print("Checksum error,\t"); 
    break;
    case DHTLIB_ERROR_TIMEOUT: 
    Serial.print("Time out error,\t"); 
    break;
    default: 
    Serial.print("Unknown error,\t"); 
    break;
  }
  // DISPLAY DATA
  Serial.print("Humidity (%): ");
  Serial.print(DHT.humidity, 1);
  Serial.print(",\t");
  Serial.print("Temperature (C): ");
  Serial.print(DHT.temperature, 1);
  Serial.print(",\t");
  
  sensorValue = analogRead(sensorPin);
  sensorValue = map(sensorValue, 0, 1023, 100, 0);
  Serial.print("Soil moisture (%): ");
  Serial.println(sensorValue);

 
  if(sensorValue < 50 && DHT.temperature > 30){
    digitalWrite(RELAY1, turn_On);
    digitalWrite(RELAY2, turn_On);
  }
  else{
    if(sensorValue < 50){
      digitalWrite(RELAY1, turn_On);
    }
    else{
      digitalWrite(RELAY1, turn_Off);
    }
    
    if(DHT.temperature > 30){
      digitalWrite(RELAY2, turn_On);
    }
    else{
      digitalWrite(RELAY2, turn_Off);
    }
  }

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
   
    delay(10000);
  }
  else
  {
    // We now create a URI for the request
    String url = "/update?api_key=";
    url += api;
    url += "&field1=";
    url += DHT.humidity;
    url += "&field2=";  
    url += DHT.temperature;
    url += "&field3=";  
    url += sensorValue;
  
    // เราจะส่งข้อมูล https://api.thingspeak.com/update?api_key=26CNRNMR7HEABR4K&field1=(ความชื่น)&field2=(อุณภูมิ)&fields=(ความชื้นของดิน)
    Serial.print("Requesting URL: ");
    Serial.println(url);  
  
    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
      
    // Read all the lines of the reply from server and print them to Serial
    while(client.available()){
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }  
  
    Serial.println();
    Serial.println("closing connection"); 
  }  

  delay(5000);
}
