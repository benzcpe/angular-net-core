#include <Boards.h>
#include <Firmata.h>
#include <FirmataConstants.h>
#include <FirmataDefines.h>
#include <FirmataMarshaller.h>
#include <FirmataParser.h>

#include <ESP8266WiFi.h>

#include <SoftwareSerial.h>

SoftwareSerial NodeSerial(D2,D3); // RX | TX

#define turn_On 0
#define turn_Off 1
int RELAY1 = D5;
int RELAY2 = D6;

// Line config
#define LINE_TOKEN ""


//#include <time.h>

// Blynk
#include <BlynkSimpleEsp8266.h>
char auth[] = "";
char ssid[] = "";
char pass[] = "";

BlynkTimer timer;

// Configuration data from blynk
int v6_autoWater = 0;
int v7_waterIndicator = 0;
int v30_turnOnWaterPoint = 20;
int v31_turnOffWaterPoint = 80;

int v32_tempNotifyLow = 20;
int v33_tempNotifyHigh = 50;
bool tempNotify = false;

float v34_phNotifyLow = 5.0f;
float v35_phNotifyHigh = 7.0f;
bool phNotify = false;


int v36_humNotifyLow = 10;
int v37_humNotifyHigh = 80;
bool humNotify = false;


int v38_co2NotifyLow = 100;
int v39_co2NotifyHigh = 700;
bool co2Notify = false;


float v40_o2NotifyLow = 15.0f;
float v41_o2NotifyHigh = 20.0f;
bool o2Notify = false;


String a;

// Data from sensor
int T, M, H, CO2;
float PH, O2;
int forceOnWater = 0;

void setup() {

  Blynk.begin(auth, ssid, pass);
  // Setup a function to be called every second
  timer.setInterval(5000L, myTimerEvent);

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

        Line_Notify("เปิดน้ำ (อัตโนมัติ) 1");
      }
      else if(M >= v31_turnOffWaterPoint && v7_waterIndicator == 1)
      {
         digitalWrite(RELAY1, turn_Off);
         Blynk.virtualWrite(V7, 0);

         v7_waterIndicator = 0;

         Serial.print("Water is auto off at M: ");
         Serial.println(M);

           Line_Notify("ปิดน้ำ (อัตโนมัติ) 1");
      }

    
   }

   if(T < v32_tempNotifyLow  || T > v33_tempNotifyHigh){  
    if(!tempNotify){
      tempNotify = true;
      Line_Notify("อุณภูมิ 1: " + String(T) +" °C");
    }      
   }
   else
  {
    tempNotify = false;
  }

  if(PH < v34_phNotifyLow || PH > v35_phNotifyHigh){  
    if(!phNotify){
      phNotify = true;
      Line_Notify("PH 1: " + String(PH));
    }      
   }
   else
  {
    phNotify = false;
  }

  if(H < v36_humNotifyLow || H > v37_humNotifyHigh){  
    if(!humNotify){
      humNotify = true;
      Line_Notify("ความชื้นสัมพัทธ์ 1: " + String(H) + "%");
    }      
   }
   else
  {
    humNotify = false;
  }

  if(CO2 < v38_co2NotifyLow || CO2 > v39_co2NotifyHigh){  
    if(!co2Notify){
      co2Notify = true;
      Line_Notify("CO2 1: " + String(CO2) + " ppm");
    }      
   }
   else
  {
    co2Notify = false;
  }

  if(O2 < v40_o2NotifyLow || O2 > v41_o2NotifyHigh){  
    if(!o2Notify){
      o2Notify = true;
      Line_Notify("O2 1: " + String(O2) + "%");
    }      
   }
   else
  {
    o2Notify = false;
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

   Line_Notify("เปิดน้ำ (กำหนดเอง) 1");
  }
  else
  {
    digitalWrite(RELAY1, turn_Off);
   // Blynk.virtualWrite(V6, 0);
    Line_Notify("ปิดน้ำ (กำหนดเอง) 1");
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

BLYNK_WRITE(V32)
{
   v32_tempNotifyLow = param.asInt(); 
   Serial.print("V32 Temp notify low: ");
   Serial.println(v32_tempNotifyLow);
}


BLYNK_WRITE(V33)
{
   v33_tempNotifyHigh  = param.asInt(); 
   Serial.print("V33 Temp notify high: ");
   Serial.println(v33_tempNotifyHigh);
}

BLYNK_WRITE(V34)
{
   v34_phNotifyLow   = param.asFloat(); 
   Serial.print("V34 PH notify low: ");
   Serial.println(v34_phNotifyLow);
}

BLYNK_WRITE(V35)
{
   v35_phNotifyHigh   = param.asFloat(); 
   Serial.print("V35 PH notify high: ");
   Serial.println(v35_phNotifyHigh);
}

BLYNK_WRITE(V36)
{
   v36_humNotifyLow   = param.asInt(); 
   Serial.print("V36 Hum notify low: ");
   Serial.println(v36_humNotifyLow);
}

BLYNK_WRITE(V37)
{
   v37_humNotifyHigh   = param.asInt(); 
   Serial.print("V37 Hum notify high: ");
   Serial.println(v37_humNotifyHigh);
}


BLYNK_WRITE(V38)
{
   v38_co2NotifyLow   = param.asInt(); 
   Serial.print("V38 CO2 notify low: ");
   Serial.println(v38_co2NotifyLow);
}

BLYNK_WRITE(V39)
{
   v39_co2NotifyHigh   = param.asInt(); 
   Serial.print("V39 CO2 notify high: ");
   Serial.println(v39_co2NotifyHigh);
}

BLYNK_WRITE(V40)
{
   v40_o2NotifyLow   = param.asFloat(); 
   Serial.print("V40 O2 notify low: ");
   Serial.println(v40_o2NotifyLow);
}

BLYNK_WRITE(V41)
{
   v41_o2NotifyHigh   = param.asFloat(); 
   Serial.print("V41 O2 notify high: ");
   Serial.println(v41_o2NotifyHigh);
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

  //////////////// LINE NOTIFY ////////////////////////////
 void Line_Notify(String message) {
  WiFiClientSecure client;

  if (!client.connect("notify-api.line.me", 443)) {
    Serial.println("connection failed");
    return;   
  }

  String req = "";
  req += "POST /api/notify HTTP/1.1\r\n";
  req += "Host: notify-api.line.me\r\n";
  req += "Authorization: Bearer " + String(LINE_TOKEN) + "\r\n";
  req += "Cache-Control: no-cache\r\n";
  req += "User-Agent: ESP8266\r\n";
  req += "Connection: close\r\n";
  req += "Content-Type: application/x-www-form-urlencoded\r\n";
  req += "Content-Length: " + String(String("message=" + message).length()) + "\r\n";
  req += "\r\n";
  req += "message=" + message;
  // Serial.println(req);
  client.print(req);
    
  delay(20);

  // Serial.println("-------------");
  while(client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
    //Serial.println(line);
  }
  // Serial.println("-------------");
}
