#include "CO2Sensor.h"

CO2Sensor co2Sensor(A3, 0.99, 100);

#include "DHT.h"

#include <Wire.h>
//#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#define I2C_ADDR 0x27 // <
#define BACKLIGHT_PIN 3

LiquidCrystal_I2C lcd(0x27, 20, 4);


#define DHTPIN 2 
 
#define DHTTYPE DHT11  

 

int  PH, sensorValue, CO2;
int Moisture;

const float VRefer =  5.0; // 3.3;       // voltage of adc reference


DHT dht(DHTPIN, DHTTYPE);

#include <SoftwareSerial.h>

SoftwareSerial NanoSerial(3, 2); // RX | TX

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  lcd.begin();
  // Print a message to the LCD.
  lcd.print(""); //ฟังก์ชั่นในการกำหนดข้อความที่ต้องการแสดงผล
  lcd.setCursor(0, 1); //ฟังก์ชั่นในการกำหนดตำแหน่ง Cursor
  lcd.print(" Smart Farm Project ");

  co2Sensor.calibrate();

  pinMode(3,INPUT);
  pinMode(2,OUTPUT);
  NanoSerial.begin(57600);
}

void loop() {


   // put your main code here, to run repeatedly:
   unsigned char  i;
 
   Moisture = 0; 
   for(i=0;i<10;i++){Moisture = Moisture + analogRead(0);delay(1);}
   Moisture = Moisture / 10; 
   //Serial.print("Soil moisture real: ");
  // Serial.println(Moisture);
     
   if(Moisture >= 530){Moisture = ((Moisture - 550)/15) + 90;}else
   if(Moisture >= 430){Moisture = ((Moisture - 450)/10) + 80;}else
   if(Moisture >= 130){Moisture = ((Moisture - 130)/6) + 30;}else
   //if(Moisture >= 100){Moisture = ((Moisture - 120)/5) + 30;}else
   if(Moisture >=   0){Moisture = ((Moisture)/5);}   
   
   if(Moisture > 100){Moisture = 100;}

   PH = 0;
    for(i=0;i<10;i++){PH  = PH + analogRead(1);delay(1); } 
    PH       = PH / 10;    
    if(PH >= 450){PH = 40-((PH - 450)/50);}else
    if(PH >= 280){PH = 50-((PH - 280)/17);}else
    if(PH >= 150){PH = 60-((PH - 150)/13);}else
    if(PH >=  20){PH = 70-((PH -  20)/13);}else
    if(PH >=   0){PH = 80-((PH -  0 )/2); }


   Serial.print("Soil moisture: ");
   Serial.print(Moisture);
   Serial.println(" %");

    Serial.print("PH: ");
   Serial.print(PH/10);Serial.print(".");Serial.println(PH%10);

  int h = dht.readHumidity();
  int t = dht.readTemperature();
  int sensorValue;
  
  Serial.print("Humidity: "); 
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(t);
  Serial.println(" *C");

  // put your main code here, to run repeatedly:
    float Vout =0;
    Serial.print("Vout =");

    Vout = readO2Vout();
    Serial.print(Vout);
    Serial.print(" V, Concentration of O2 is ");
    float o2 = readConcentration();
    Serial.print(readConcentration());
    Serial.println(" %");

  int co2 = co2Sensor.read();

  Serial.print( "CO2: " );
  Serial.print( co2 );
  Serial.println( "ppm" );
  Serial.println( "" );

  
  lcd.clear();
  lcd.home();
  String sMoisture = "Soil Moi:";
  sMoisture += Moisture;
  sMoisture += "% PH:";
  sMoisture +=PH/10;
  sMoisture += ".";
  sMoisture += PH%10;

  String sTemp = "Temp:";
  sTemp += t;
  sTemp += "C Hum:";
  sTemp += h;
  sTemp +="%";

   String sGas = "O2:";
   sGas += o2;
   sGas += "% CO2:";

    sGas += co2;
    sGas += "ppm";
   
  String sWater = "Water: ON/OFF";
  
  lcd.print(sMoisture); //ฟังก์ชั่นในการกำหนดข้อความที่ต้องการแสดงผล
  lcd.setCursor(0, 1); //ฟังก์ชั่นในการกำหนดตำแหน่ง Cursor
  lcd.print(sTemp);
  lcd.setCursor(0, 2);
  lcd.print(sGas);
  lcd.setCursor(0, 3);
  lcd.print(sWater);

   NanoSerial.print(o2); 
   NanoSerial.print("\n");
  
  delay(3000); //wait for half a second, so it is easier to read
}


float readO2Vout()
{
    long sum = 0;
    for(int i=0; i<32; i++)
    {
        sum += analogRead(2);
    }

    sum >>= 5;

    float MeasuredVout = sum * (VRefer / 1023.0);
    return MeasuredVout;
}

float readConcentration()
{
    // Vout samples are with reference to 3.3V
    float MeasuredVout = readO2Vout();

    //float Concentration = FmultiMap(MeasuredVout, VoutArray,O2ConArray, 6);
    //when its output voltage is 2.0V,
    float Concentration = MeasuredVout * 0.21 / 2.0;
    float Concentration_Percentage=Concentration*100;
    return Concentration_Percentage;
}
