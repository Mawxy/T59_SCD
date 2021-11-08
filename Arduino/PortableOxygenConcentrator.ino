#include <SPI.h>
#include "Adafruit_MAX31855.h"
#include <Arduino.h>
#include "Adafruit_SHT31.h"

#define threeWay_column1 2 
#define threeWay_inlet   3 
#define threeWay_column2 4 
#define twoWay_column1   5 
#define twoWay_column2   6 
#define inletHumidityAddr 0x45
#define columnUptime (10 * 1000) //first number is the number of seconds desired


Adafruit_SHT31 inletHumidity = Adafruit_SHT31();

void setup() {
  Serial.begin(9600);

//  while(!Serial)
//    delay(10);

//  if( !inletHumidity.begin(inletHumidityAddr) )
//  {
//    Serial.println("Could not find inlet humidity sensor");
//    while(1) delay(1);
//  }
//
//  if (inletHumidity.isHeaterEnabled())
//  {
//    inletHumidity.heater(false);
//  }
  
  pinMode(threeWay_column1, OUTPUT);
  pinMode(threeWay_inlet,   OUTPUT);
  pinMode(threeWay_column2, OUTPUT);
  pinMode(twoWay_column1,   OUTPUT);
  pinMode(twoWay_column2,   OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  
  digitalWrite(threeWay_column1,  LOW); //needs to be low
  digitalWrite(threeWay_inlet,   LOW); 
  digitalWrite(threeWay_column2,  LOW);
  digitalWrite(twoWay_column1,   LOW); //closed
  digitalWrite(twoWay_column2,    HIGH); //open
  Serial.println("COLUMN 1 HIGH");
  
//  for(int i = 0; i < columnUptime; i++)
//  {
//    float inletT = inletHumidity.readTemperature();
//    float inletH = inletHumidity.readHumidity();
//
//    if (!isnan(inletT) || !isnan(inletH) {  // check if 'is not a number'
//      Serial.print("Temp *C = ");
//      Serial.print(inletT);
//      Serial.print("\t\t");
//      Serial.print("Hum. % = ");
//      Serial.println(inletH);
//    } else { 
//      Serial.println("Failed to read temperature");
//    }
//
//    delay(1000);
//  }
  delay(columnUptime);
  
  digitalWrite(threeWay_column1,  HIGH);
  digitalWrite(threeWay_inlet,    HIGH);
  digitalWrite(threeWay_column2, HIGH);
  digitalWrite(twoWay_column1,    HIGH);
  digitalWrite(twoWay_column2,   LOW);
  Serial.println("COLUMN 2 HIGH");
  delay(columnUptime);

}
