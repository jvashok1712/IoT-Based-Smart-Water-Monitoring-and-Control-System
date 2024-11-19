#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <Wire.h>

#include "GravityTDS.h"
#include<SoftwareSerial.h>
SoftwareSerial mySerial(8,9);//rx,tx
int k=0;
#define TdsSensorPin A1
#define turbidity_pin A3
const int trigPin = 10;
const int echoPin = 11;
//ph sensor pin is A2.
//tds sensor pin is A1  and it operates on 3.3 to 5v
//Flow sensor pin is A0
GravityTDS gravityTds;
long duration;
int distance;
 
float temperature = 25,tdsValue = 0;
int X;
int Y;
float TIME = 0;
float FREQUENCY = 0;
float WATER = 0;
float TOTAL = 0;
float LS = 0;
const int input = A0;
float calibration_value = 9.34;
int phval = 0; 
int tds=0;
unsigned long int avgval; 
int buffer_arr[10],temp;
void setup()
{
Serial.begin(9600);
mySerial.begin(9600);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);
    gravityTds.setPin(TdsSensorPin);
    gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
    gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
    gravityTds.begin(); 
delay(1000);
pinMode(input,INPUT);
//sendsms();
}
void loop()
{
X = pulseIn(input, HIGH);
Y = pulseIn(input, LOW);
TIME = X + Y;
FREQUENCY = 1000000/TIME;
WATER = FREQUENCY/7.5;
LS = WATER/60;
Serial.println(TIME);
if(TIME==0){
  k++;
  Serial.println("hello");
  delay(1000);
}
//if(k==4){Serial.println("hellohjhj");
//  sendsms(TOTAL);
//}
if(FREQUENCY >= 0)
{
if(isinf(FREQUENCY))
{
Serial.println("V:");
Serial.print(TOTAL);
Serial.println("L");
}
else
{
TOTAL = TOTAL + LS;
Serial.println(FREQUENCY);
Serial.println("V:");
Serial.print(TOTAL);
Serial.println("L");

}

}

gravityTds.setTemperature(temperature);  // set the temperature and execute temperature compensation
    gravityTds.update();  //sample and calculate
    tdsValue = gravityTds.getTdsValue(); 
    tds=int(tdsValue);
    Serial.println("TDS:");
Serial.print(tdsValue);
Serial.println("ppm");


  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);


 float tot1;
 int tot2,flows_iot;
 flows_iot=int(WATER*100);
 int ph=0;
 tot1=TOTAL*10;
 tot2=(int)tot1;
 int costa=TOTAL*10;
char buf[50];
Serial.println(TOTAL);
Serial.println(ph);
Serial.println(tds);
Serial.println(costa);
Serial.println(int(distance));
Serial.println(flows_iot/100);
sprintf(buf,"the %d and %d and %d and %d and %d",tot2,int(distance),tds,costa,flows_iot);
mySerial.print(buf);

 
 delay(1000);
}