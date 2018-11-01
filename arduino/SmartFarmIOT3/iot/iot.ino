#include <ESP8266WiFi.h>

//const char* ssid = "BENZ";
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
char ssid[] = "BENZ";
char pass[] = "0850299990";

BlynkTimer timer;


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

    if(forceOnWater <= 0)
    {
      if(M < 20)
      {
        digitalWrite(RELAY1, turn_On);
        Blynk.virtualWrite(V6, 1);
      }
      else
      {
         digitalWrite(RELAY1, turn_Off);
         Blynk.virtualWrite(V6, 0);
      }
    }

    Serial.println(serialResponse);
    delay(100);

  }

  delay(1000);
  forceOnWater--;

  Blynk.run();
  timer.run(); 
}

// Blynk
// This function will be called every time Slider Widget
// in Blynk app writes values to the Virtual Pin 6
 BLYNK_WRITE(V6)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("V6 Slider value is: ");
  Serial.println(pinValue);

  if(pinValue == 1){
    digitalWrite(RELAY1, turn_On);
    Blynk.virtualWrite(V6, 1);
  }
  else
  {
    digitalWrite(RELAY1, turn_Off);
    Blynk.virtualWrite(V6, 0);
  }

  // Stop auto on/off water if user force to open/close it from Blynk
  forceOnWater = 60 * 60;
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
