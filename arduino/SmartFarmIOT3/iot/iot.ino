#include <Boards.h>
#include <Firmata.h>
#include <FirmataConstants.h>
#include <FirmataDefines.h>
#include <FirmataMarshaller.h>
#include <FirmataParser.h>

#include <ESP8266WiFi.h>

//const char* ssid = "Neb";
//const char* password = "0850299990";

#include <SoftwareSerial.h>

SoftwareSerial NodeSerial(D2,D3); // RX | TX

#define turn_On 0
#define turn_Off 1
int RELAY1 = D5;
int RELAY2 = D6;

// Blynk
#include <BlynkSimpleEsp8266.h>
char auth[] = "";
char ssid[] = "Neb";
char pass[] = "0850299990";

BlynkTimer timer;

// Configuration data from blynk
int v6_autoWater = 0;
int v7_waterIndicator = 0;
int v30_turnOnWaterPoint = 20;
int v31_turnOffWaterPoint = 80;


String a;

// Data from sensor
int T, M, H, CO2;
float PH, O2;
int forceOnWater = 0;

void setup() {

  Blynk.begin(auth, ssid, pass);
  // Setup a function to be called every second
  timer.setInterval(5000L, myTimerEvent);
  
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
  
  Serial.begin(115200);

  pinMode(D2, INPUT); 
  pinMode(D3, OUTPUT); 

  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);

  digitalWrite(RELAY1, turn_Off);
  digitalWrite(RELAY2, turn_Off);

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
  
    while ((str = strtok_r(p, ";", &p)) != NULL) // delimiter is the semicolon
    {
     // Serial.println(str);

      if(i == 0){
        T = String(str).toInt();
      }
      else if(i == 1){
        H =  String(str).toInt();
      }
      else if(i == 2){
        M = String(str).toInt();
      }
      else if(i == 3){
        PH = String(str).toFloat();
      }
      else if(i == 4){
        CO2 = String(str).toFloat();
      }
      else if(i == 5){
        O2 = String(str).toInt();
      }

      i++;
    }

   

    Serial.println(serialResponse);
    delay(100);

  }

   // Test data
    T = 26;
    H = 40;
    M = 50;
    PH = 6.7;
    CO2 = 412;
    O2 = 20;

   if(forceOnWater <= 0 && v6_autoWater == 1)
   {
      if(M <= v30_turnOnWaterPoint && v7_waterIndicator == 0)
      {
        digitalWrite(RELAY1, turn_On);
        Blynk.virtualWrite(V7, 1);

        v7_waterIndicator = 1;

        Serial.print("Water is auto on at M: ");  
        Serial.println(M);       
      }
      else if(M >= v31_turnOffWaterPoint && v7_waterIndicator == 1)
      {
         digitalWrite(RELAY1, turn_Off);
         Blynk.virtualWrite(V7, 0);

         v7_waterIndicator = 0;

         Serial.print("Water is auto off at M: ");
         Serial.println(M);
      }

    
   }

  delay(1000);
  forceOnWater--;

  Blynk.run();
  timer.run(); 
}

// Blynk
BLYNK_CONNECTED() {
    Blynk.syncAll();
}

// This function will be called every time Slider Widget
// in Blynk app writes values to the Virtual Pin 7
 BLYNK_WRITE(V7)
{
  v7_waterIndicator = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("V7 Turn on/off water manual: ");
  Serial.println(v7_waterIndicator);

  if(v7_waterIndicator == 1){
    digitalWrite(RELAY1, turn_On);
   // Blynk.virtualWrite(V6, 1);
  }
  else
  {
    digitalWrite(RELAY1, turn_Off);
   // Blynk.virtualWrite(V6, 0);
  }

  // Stop auto on/off water for 10 seconds if user force to open/close it from Blynk
  forceOnWater = 10;
}

BLYNK_WRITE(V6)
{
   v6_autoWater = param.asInt(); 
   Serial.print("V6 Auto water: ");
   Serial.println(v6_autoWater);
}

BLYNK_WRITE(V30)
{
   v30_turnOnWaterPoint = param.asInt(); 
   Serial.print("V30 Turn on water point: ");
   Serial.println(v30_turnOnWaterPoint);
}

BLYNK_WRITE(V31)
{
   v31_turnOffWaterPoint = param.asInt(); 
   Serial.print("V31 Turn off water point: ");
   Serial.println(v31_turnOffWaterPoint);
}



// This function sends Arduino's up time every second to Virtual Pin (0).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void myTimerEvent()
{
  //Serial.println("Test");
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V0, T);
  Blynk.virtualWrite(V1, H);
  Blynk.virtualWrite(V2, PH);
  Blynk.virtualWrite(V3, M);
  Blynk.virtualWrite(V4, CO2);
  Blynk.virtualWrite(V5, O2);

  
}
