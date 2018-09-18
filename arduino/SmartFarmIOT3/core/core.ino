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

 
/************************Hardware Related Macros************************************/
#define         MG_PIN                       (3)     //define which analog input channel you are going to use
#define         BOOL_PIN                     (3)
#define         DC_GAIN                      (8.5)   //define the DC gain of amplifier
 
/***********************Software Related Macros************************************/
#define         READ_SAMPLE_INTERVAL         (5)//(50)    //define how many samples you are going to take in normal operation
#define         READ_SAMPLE_TIMES            (5)     //define the time interval(in milisecond) between each samples in 
                                                     //normal operation
 
/**********************Application Related Macros**********************************/
//These two values differ from sensor to sensor. user should derermine this value.
#define         ZERO_POINT_VOLTAGE           (0.220) //define the output of the sensor in volts when the concentration of CO2 is 400PPM
#define         REACTION_VOLTGAE             (0.020) //define the voltage drop of the sensor when move the sensor from air into 1000ppm CO2
 
/*****************************Globals***********************************************/
float           CO2Curve[3]  =  {2.602,ZERO_POINT_VOLTAGE,(REACTION_VOLTGAE/(2.602-3))};   
                                                     //two points are taken from the curve. 
                                                     //with these two points, a line is formed which is
                                                     //"approximately equivalent" to the original curve.
                                                     //data format:{ x, y, slope}; point1: (lg400, 0.324), point2: (lg4000, 0.280) 
                                                     //slope = ( reaction voltage ) / (log400 –log1000) 
                                                     

int  PH, sensorValue, CO2;
int Moisture;

const float VRefer =  5.0; // 3.3;       // voltage of adc reference


DHT dht(DHTPIN, DHTTYPE);


int a = 13;

int b = 25;

#include <SoftwareSerial.h>

SoftwareSerial chat(3, 2); // RX, TX

int i;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(BOOL_PIN, INPUT);                        //set pin to input
  digitalWrite(BOOL_PIN, HIGH);                    //turn on pullup resistors

  lcd.begin();
  // Print a message to the LCD.
  lcd.print(""); //ฟังก์ชั่นในการกำหนดข้อความที่ต้องการแสดงผล
  lcd.setCursor(0, 1); //ฟังก์ชั่นในการกำหนดตำแหน่ง Cursor
  lcd.print(" Smart Farm Project ");

  co2Sensor.calibrate();

  chat.begin(4800);
}

void loop() {

  
  //Serial.print("CO2 value: ");
  //Serial.println(val);
  
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

  /*int  lettura = MGRead2(MG_PIN);
  lettura = map(lettura,0,9650,350,10000);
   //lettura = map(lettura,0,1023,350,10000);
  // Serial.print("Vecchia lettura: "); //old reading
   //Serial.print(letturaOld);
    Serial.print("CO2:");

        Serial.print(lettura);

+
    Serial.println( " ppm " );  
    Serial.println( "" ); */

    /*int percentage;
    float volts;
 
    volts = MGRead(MG_PIN);
    Serial.print( "SEN-00007:" );
    Serial.print(volts); 
    Serial.print( "V           " );
 
    percentage = MGGetPercentage(volts,CO2Curve);
    Serial.print("CO2:");
    if (percentage == -1) {
        Serial.print( "<400" );
    } else {
        Serial.print(percentage);
    }
 
    Serial.print( " ppm " );  
 
    if (digitalRead(BOOL_PIN) ){
        Serial.println( "=====BOOL is HIGH======" );
    } else {
        Serial.println( "=====BOOL is LOW======" );
    }*/



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
   /*if (percentage <= 0) {
       sGas += "<400";
    } else {
        sGas += percentage;
    }
    */
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

   chat.print("Hello World!!!");

  /*if (chat.readString()){

     // chat.print(1);

     if(chat.readString()== "Question1"){ //มีการถามคำถาม Question1 ส่งข้อมูลตัวแปร a ออกไป

      chat.print(sTemp);

     }

     if(chat.readString()== "Question2"){ //มีการถามคำถาม Question2 ส่งข้อมูลตัวแปร b ออกไป

      chat.print(sGas);

     }

     // Serial.print("Send = ");

    //  Serial.println(i);

  }

 // i++;
  */
  
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

/*****************************  MGRead *********************************************
Input:   mg_pin - analog channel
Output:  output of SEN-000007
Remarks: This function reads the output of SEN-000007
************************************************************************************/ 
float MGRead(int mg_pin)
{
    int i;
    float v=0;
 
    for (i=0;i<READ_SAMPLE_TIMES;i++) {
        v += analogRead(mg_pin);
        delay(READ_SAMPLE_INTERVAL);
    }
    v = (v/READ_SAMPLE_TIMES) *5/1024 ;
    return v;  
}


 float MGRead2(int mg_pin)
{
    int i;
    float v=0;
 
    for (i=0;i<READ_SAMPLE_TIMES;i++) {
        v += analogRead(mg_pin);
        delay(READ_SAMPLE_INTERVAL);
    }
    v = (v/READ_SAMPLE_TIMES);
    return v;  
}
 
/*****************************  MQGetPercentage **********************************
Input:   volts   - SEN-000007 output measured in volts
         pcurve  - pointer to the curve of the target gas
Output:  ppm of the target gas
Remarks: By using the slope and a point of the line. The x(logarithmic value of ppm) 
         of the line could be derived if y(MG-811 output) is provided. As it is a 
         logarithmic coordinate, power of 10 is used to convert the result to non-logarithmic 
         value.
************************************************************************************/ 
int  MGGetPercentage(float volts, float *pcurve)
{
   if ((volts/DC_GAIN )>=ZERO_POINT_VOLTAGE) {
      return -1;
   } else { 
      return pow(10, ((volts/DC_GAIN)-pcurve[1])/pcurve[2]+pcurve[0]);
   }
} 
