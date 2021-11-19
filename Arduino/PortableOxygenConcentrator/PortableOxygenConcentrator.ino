#include <SPI.h>
#include "Adafruit_MAX31855.h"
#include <Arduino.h>
#include "Adafruit_SHT31.h"
#include <HoneywellZephyrI2C.h>  //NEEDS TO BE DOWNLOADED FROM LIBRARY idk why it is in <>
#include <Adafruit_ADS1X15.h>

#define threeWay_column1      2 
#define threeWay_inlet        3 
#define threeWay_column2      4 
#define twoWay_column1        5 
#define twoWay_column2        6 
#define thermo_do_column1     7
#define thermo_cs_column1     8
#define thermo_clk_column1    9
#define thermo_do_column2    10
#define thermo_cs_column2    11
#define thermo_clk_column2   12
#define inletHumidityAddr  0x45
#define outletHumidityAddr 0x44
#define dacAddr            0x48
#define inletFlowAddr      0x49
#define columnUptime         10 //(in seconds)

Adafruit_SHT31 inletHumidity = Adafruit_SHT31();
Adafruit_SHT31 outletHumidity = Adafruit_SHT31();
Adafruit_MAX31855 thermoColumn1(thermo_do_column1, thermo_cs_column1, thermo_clk_column1);
Adafruit_MAX31855 thermoColumn2(thermo_do_column2, thermo_cs_column2, thermo_clk_column2);
ZephyrFlowRateSensor inletFlow(inletFlowAddr, 100, ZephyrFlowRateSensor::SLPM);
Adafruit_ADS1115 dac;

bool inletHumidityEnabled = true;
bool outletHumidityEnabled = true;
bool inletFlowEnabled = true;
bool thermoColumn1Enabled = true;
bool thermoColumn2Enabled = true;
bool dacEnabled = true;
int loopT = 0;
int initialmV1;
int initialmV2;
int initialmV3;
int initialmV4;


void setup() 
{
  Serial.begin(9600);

  Serial.println("here");

  while(!Serial)
    delay(10);

  Serial.println("now here");
  //let sensors stabilize
//  delay(1000);

  
  //Start inlet humidity sensor
  if( !inletHumidity.begin(inletHumidityAddr))
  {
    Serial.println("Continuing without inlet humidity and temperature sensor");
    inletHumidityEnabled = false;
    
  }
    Serial.println("Should hit");
  //Start outlet humidity sensor
  if (!outletHumidity.begin(outletHumidityAddr))
  {
    Serial.println("Continuing without outlet humidity and temperature sensor");
    outletHumidityEnabled = false;
  }

  
  //start Flow Sensor
//  inletFlow.begin();
//
//  Serial.println("Now here");
  //start thermocouple for column 1

  Serial.println("Here");
  if(!thermoColumn1.begin())
  {
    Serial.println("Continuing without column 1 thermocouple");
    thermoColumn1Enabled = false;
  }

  //start thermocouple for column 2
  if(!thermoColumn2.begin())
  {
    Serial.println("Continuing without column 2 thermocouple");
    thermoColumn2Enabled = false;
  }

  //start DAC for pressure sensors
  //1x Gain = 0.125mV per bit
  //2x Gain = 0.0625mV per bit
  if(!dac.begin(0x48))
  { 
    Serial.println("Continuing without DAC pressure sensors");
    dacEnabled = false;
  }
  else
  {
  //Getting inital values for each pressure sensor
    dac.setGain(GAIN_ONE);
    calibratePressure();
  }

  Serial.println("HERE");

  //Set relay pins all to oupout
  pinMode(threeWay_column1, OUTPUT);
  pinMode(threeWay_inlet,   OUTPUT);
  pinMode(threeWay_column2, OUTPUT);
  pinMode(twoWay_column1,   OUTPUT);
  pinMode(twoWay_column2,   OUTPUT);

}

//Grabs sensor data for all sensors
//Grabs data every second for columnUptime seconds
//this also holds the column uptime delay
void sensorLoop()
{
  int dacDecimalCount;
  float inletT = -1;
  float inletH = -1;
  float outletT = -1;
  float outletH = -1;
  float inletF = -1;
  double column1T = -1;
  double column2T = -1;
  float psi1 = -1;
  float psi2 = -1;
  float psi3 = -1;
  float psi4 = -1;
  
  for(int i = 0; i < columnUptime; i++)
  {
    //reads inletHumidity sensor and grabs temp in C and humidity as %
    if(inletHumidityEnabled)
    {
      inletT = inletHumidity.readTemperature();
      inletH = inletHumidity.readHumidity();
    }

    //reads outlet humidity sensor and grabs temp in C and humidity as %
    if(outletHumidityEnabled)
    {
      outletT = outletHumidity.readTemperature();
      outletH = outletHumidity.readHumidity();
    }

    //reads flow sensor and gets flow in SLPM
    if(inletFlowEnabled)
    {
      inletFlow.readSensor();
      inletF = inletFlow.flow();
    }

    //Reads thermoCouple 1 (should be tied to column 1)
    //reads in Celsius
    if(thermoColumn1Enabled)
    {
      column1T = thermoColumn1.readCelsius();
    }

    //Reads thermoCouple 2 in celsius
    if(thermoColumn2Enabled)
    {
      column2T = thermoColumn2.readCelsius();
    }

    //Reads DAC and caluclates psis
    if(dacEnabled)
    {
      dacDecimalCount = dac.readADC_SingleEnded(0);
      psi1 = calculatePressure(dacDecimalCount, initialmV1);

//      dacDecimalCount = dac.readADC_SingleEnded(1);
//      psi2 = calculatePressure(dacDecimalCount, initialmV2);
//
//      dacDecimalCount = dac.readADC_SingleEnded(2);
//      psi3 = calculatePressure(dacDecimalCount, initialmV3);
//
//      dacDecimalCount = dac.readADC_SingleEnded(3);
//      psi4 = calculatePressure(dacDecimalCount, initialmV4);
      
    }


//    //Following is all to print to CSV
//    //basically prints -1 for values if it is NAN which would indicate a sensor not working
//    Serial.print(loopT);
//    Serial.print(",");
//
//    if (!isnan(inletT) || !isnan(inletH)) {  // check if 'is not a number'
//      Serial.print(inletT); Serial.print(","); Serial.print(inletH);Serial.print(",");
//    } else { 
//      Serial.print("-1"); Serial.print(","); Serial.print("-1"); Serial.print(",");
//    }
//
//    if (!isnan(outletT) || !isnan(outletH)) {  // check if 'is not a number'
//      Serial.print(outletT); Serial.print(","); Serial.print(outletH); Serial.print(",");
//    } else { 
//      Serial.print("-1"); Serial.print(","); Serial.print("-1"); Serial.print(",");
//    }
//
//    if (!isnan(inletF))
//    {
//      Serial.print(inletF); Serial.print(",");
//    } else {
//      Serial.print("-1"); Serial.print(",");
//    }
//
//    
//    if (!isnan(column1T))
//    {
//      Serial.print(column1T); Serial.print(",");
//    } else {
//      Serial.print("-1"); Serial.print(",");
//    }
//
//    
//    if (!isnan(column2T))
//    {
//      Serial.print(column2T); Serial.print(",");
//    } else {
//      Serial.print("-1"); Serial.print(",");
//    }
//
//    if (!isnan(psi1))
//    {
//      Serial.print(psi1); Serial.print(","); Serial.print(psi2);
//      Serial.print(","); Serial.print(psi3); Serial.print(",");
//      Serial.print(psi4);
//    } 
//    else 
//    {
//      Serial.print("-1"); Serial.print(","); Serial.print("-1");
//      Serial.print(","); Serial.print("-1"); Serial.print(",");
//      Serial.print("-1");
//    }
//    
//    Serial.println();
//   
    loopT = loopT + 1;
    delay(1000);
  }
  
}

//Calibrates initial decimal count for pressure sensors
//This is necessary because each sensor could be slightly different
//The sensors have an offset voltage so we will need to use these to caluclate change
void calibratePressure()
{
  delay(5000);
  int initialPDC1 = dac.readADC_SingleEnded(0);
  int initialPDC2 = dac.readADC_SingleEnded(1);
  int initialPDC3 = dac.readADC_SingleEnded(2);
  int initialPDC4 = dac.readADC_SingleEnded(3);

  initialmV1 = initialPDC1 * 0.125;
  Serial.println("**INITIAL MV**");
  Serial.println(initialmV1);
  initialmV2 = initialPDC2 * 0.125;
  initialmV3 = initialPDC3 * 0.125;
  initialmV4 = initialPDC4 * 0.125;
}

//0-100mV output for sensors
//each decimal count = 0.125mV
//100 mV = max PSI (250)
//0mV = min PSI (0)
//decimal count * 0.125 to get value in milivolts
//then subtract initialPDC (pressure decimal count)
//that will be the change in milivolts
// change in milivolts divided by 250 gets you the ratio
// multiply that ratio by 100 to get the value in psi 
float calculatePressure(float decimalCount, float initialmV)
{
  float psi, mvTotal, ratio, mvTrue;
  mvTotal = decimalCount * 0.125;
  Serial.println("**mvTotal**");
  Serial.println(mvTotal);
  mvTrue = mvTotal - initialmV;

  Serial.println("**mvTrue**");
  Serial.println(mvTrue);
  ratio = mvTrue / 100;
  psi = ratio * 250;
 
  return psi;
}

void loop() 
{
  //delay that runs to make sure everything is in "default" state off the start
  //additionally this prints the headers for the csv
  if(loopT == 0)
  {
    //delay(5000);
    Serial.println("Time,inletT,inletH,outletT,outletH,inletF,column1T,column2T,ps1,psi2,psi3,psi4");
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
