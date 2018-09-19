
// CO2
#include "CO2Sensor.h"
CO2Sensor co2Sensor(A3, 0.99, 100);

// O2
const float VRefer =  5.0; // 3.3;       // voltage of adc reference
#define PIN_O2 2

// Temperature and Humidity
#include "DHT.h"
#define DHTPIN 4  
#define DHTTYPE DHT11  

// Moisture
#define PIN_Moisture 0
#define PIN_PH 1

// Node MCU connection
#include <SoftwareSerial.h>
SoftwareSerial NanoSerial(3, 2); // RX | TX
DHT dht(DHTPIN, DHTTYPE);

// LCD
#include <LiquidCrystal_I2C.h>
#define I2C_ADDR 0x27 // <
#define BACKLIGHT_PIN 3

LiquidCrystal_I2C lcd(0x27, 20, 4);

// Sensor variable
int CO2, T, H, Moisture, PH;
float O2;


void setup()
{  
  
  Serial.begin(115200);
  
  lcd.begin();
  // Print a message to the LCD.
  lcd.print(""); //ฟังก์ชั่นในการกำหนดข้อความที่ต้องการแสดงผล
  lcd.setCursor(0, 1); //ฟังก์ชั่นในการกำหนดตำแหน่ง Cursor
  lcd.print(" -- Smart Farm -- ");


  // Connect to nodemcu
  pinMode(3,INPUT);
  pinMode(2,OUTPUT);  
  NanoSerial.begin(57600);

  co2Sensor.calibrate();
}

void loop() {

  // Moisture and PH
  unsigned char  i;
 
  Moisture = 0; 
   for(i=0;i<10;i++){Moisture = Moisture + analogRead(PIN_Moisture);delay(1);}
   Moisture = Moisture / 10; 

   if(Moisture >= 530){Moisture = ((Moisture - 550)/15) + 90;}else
   if(Moisture >= 430){Moisture = ((Moisture - 450)/10) + 80;}else
   if(Moisture >= 130){Moisture = ((Moisture - 130)/6) + 30;}else
   //if(Moisture >= 100){Moisture = ((Moisture - 120)/5) + 30;}else
   if(Moisture >=   0){Moisture = ((Moisture)/5);}   
   
   if(Moisture > 100){Moisture = 100;}

    PH = 0;
    for(i=0;i<10;i++){PH  = PH + analogRead(PIN_PH);delay(1); } 
    PH = PH / 10;    
    if(PH >= 450){PH = 40-((PH - 450)/50);}else
    if(PH >= 280){PH = 50-((PH - 280)/17);}else
    if(PH >= 150){PH = 60-((PH - 150)/13);}else
    if(PH >=  20){PH = 70-((PH -  20)/13);}else
    if(PH >=   0){PH = 80-((PH -  0 )/2); }
    
    // Gas sensor
    CO2 = co2Sensor.read();
  
    float Vout = readO2Vout();  
    O2 = readConcentration();
  
    H = dht.readHumidity();
    T = dht.readTemperature();
  
    Serial.print("Humidity: "); 
    Serial.print(H);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(T);
    Serial.println(" *C");  

   Serial.print("Soil moisture: ");
   Serial.print(Moisture);
   Serial.println(" %");

   Serial.print("PH: ");
   Serial.print(PH/10);Serial.print(".");Serial.println(PH%10);


  Serial.print("Vout =");
  Serial.print(Vout);
  Serial.print(" V, Concentration of O2 is ");
  Serial.print(O2);
  Serial.println(" %");
    
  Serial.print( "CO2: " );
  Serial.print( CO2 );
  // Serial.print( " : " );
  // Serial.print( analogRead(3));
  Serial.println( "ppm" );
  Serial.println( "" );

  NanoSerial.print(T); NanoSerial.print(" ");
  NanoSerial.print(H); NanoSerial.print(" ");
  NanoSerial.print(Moisture); NanoSerial.print(" ");
  NanoSerial.print(PH/10);NanoSerial.print(".");NanoSerial.print(PH%10); NanoSerial.print(" ");
  NanoSerial.print(CO2); NanoSerial.print(" ");
  NanoSerial.print(O2); NanoSerial.print("\n");

  lcd.clear();
  lcd.home();
  String sMoisture = "Soil Moi:";
  sMoisture += Moisture;
  sMoisture += "% PH:";
  sMoisture +=PH/10;
  sMoisture += ".";
  sMoisture += PH%10;

  String sTemp = "Temp:";
  sTemp += T;
  sTemp += "C Hum:";
  sTemp += H;
  sTemp +="%";

   String sGas = "O2:";
   sGas += O2;
   sGas += "% CO2:";

    sGas += CO2;
    sGas += "ppm";
   
  String sWater = "Water: ON/OFF";
  
  lcd.print(sMoisture); //ฟังก์ชั่นในการกำหนดข้อความที่ต้องการแสดงผล
  lcd.setCursor(0, 1); //ฟังก์ชั่นในการกำหนดตำแหน่ง Cursor
  lcd.print(sTemp);
  lcd.setCursor(0, 2);
  lcd.print(sGas);
  lcd.setCursor(0, 3);
  lcd.print(sWater);

  delay(3000);

}

/************************ O2 ************************/
float readO2Vout()
{
    long sum = 0;
    for(int i=0; i<32; i++)
    {
        sum += analogRead(PIN_O2);
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
