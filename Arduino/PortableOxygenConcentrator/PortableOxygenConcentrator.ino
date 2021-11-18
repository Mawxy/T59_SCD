#include <SPI.h>
#include "Adafruit_MAX31855.h"
#include <Arduino.h>
#include "Adafruit_SHT31.h"
#include <HoneywellZephyrI2C.h>  //NEEDS TO BE DOWNLOADED FROM LIBRARY idk why it is in <>

#define threeWay_column1      2 
#define threeWay_inlet        3 
#define threeWay_column2      4 
#define twoWay_column1        5 
#define twoWay_column2        6 
#define inletHumidityAddr  0x45
#define outletHumidityAddr 0x44
#define inletFlowAddr      0x49
#define columnUptime         10 //(in seconds)


Adafruit_SHT31 inletHumidity = Adafruit_SHT31();
Adafruit_SHT31 outletHumidity = Adafruit_SHT31();
ZephyrFlowRateSensor inletFlow(inletFlowAddr, 100, ZephyrFlowRateSensor::SLPM);

bool inletHumidityEnabled = true;
bool outletHumidityEnabled = true;
bool inletFlowEnabled = true;
int loopT = 0;


void setup() 
{
  Serial.begin(9600);

  while(!Serial)
    delay(10);

  if( !inletHumidity.begin(inletHumidityAddr))
  {
    Serial.println("Continuing without inlet humidity and temperature sensor");
    inletHumidityEnabled = false;
    
  }
  
  if (!outletHumidity.begin(outletHumidityAddr))
  {
    Serial.println("Continuing without outlet humidity and temperature sensor");
    outletHumidityEnabled = false;
  }

  inletFlow.begin();
  
  pinMode(threeWay_column1, OUTPUT);
  pinMode(threeWay_inlet,   OUTPUT);
  pinMode(threeWay_column2, OUTPUT);
  pinMode(twoWay_column1,   OUTPUT);
  pinMode(twoWay_column2,   OUTPUT);

}

void sensorLoop()
{
  float inletT = -1;
  float inletH = -1;
  float outletT = -1;
  float outletH = -1;
  float inletF = -1;
  
  for(int i = 0; i < columnUptime; i++)
  {
    if(inletHumidityEnabled)
    {
      inletT = inletHumidity.readTemperature();
      inletH = inletHumidity.readHumidity();
    }

    if(outletHumidityEnabled)
    {
      outletT = outletHumidity.readTemperature();
      outletH = outletHumidity.readHumidity();
    }

    if(inletFlowEnabled)
    {
      inletFlow.readSensor();
      inletF = inletFlow.flow();
    }

    Serial.print(loopT);
    Serial.print(",");

    if (!isnan(inletT) || !isnan(inletH)) {  // check if 'is not a number'
      Serial.print(inletT); Serial.print(","); Serial.print(inletH);Serial.print(",");
    } else { 
      Serial.print("-1"); Serial.print(","); Serial.print("-1"); Serial.print(",");
    }

    if (!isnan(outletT) || !isnan(outletH)) {  // check if 'is not a number'
      Serial.print(outletT); Serial.print(","); Serial.print(outletH); Serial.print(",");
    } else { 
      Serial.print("-1"); Serial.print(","); Serial.print("-1"); Serial.print(",");
    }

    if (!isnan(inletF))
    {
      Serial.print(inletF);
    } else {
      Serial.print("-1");
    }
    
    Serial.println();
   
    loopT = loopT + 1;
    delay(1000);
  }
  
}

void loop() 
{
  //delay that runs to make sure everything is in "default" state off the start
  //additionally this prints the headers for the csv
  if(loopT == 0)
  {
    delay(10000);
    Serial.println("Time,inletT,inletH,outletT,outletH,inletF");
  }

  
  digitalWrite(threeWay_inlet,   HIGH); 

  delay(5000);
  
  digitalWrite(threeWay_column1,  LOW); 
  digitalWrite(threeWay_column2,  LOW);
  digitalWrite(twoWay_column1,   LOW); 
  digitalWrite(twoWay_column2,    HIGH); 
  
  sensorLoop();

  digitalWrite(threeWay_inlet,    LOW);

  delay(5000);
    
  digitalWrite(threeWay_column1,  HIGH);
  digitalWrite(threeWay_column2, HIGH);
  digitalWrite(twoWay_column1,    HIGH);
  digitalWrite(twoWay_column2,   LOW);
  
  sensorLoop();

}
