#include <EEPROM.h>
#include "GravityTDS.h"
const int tdsPin = 35;
float temperature = 25, tds_value = 0;
#define EEPROM_SIZE 512
GravityTDS gravityTds;
#include <Arduino.h>
//#include "util/OneWire_direct_gpio.h"
#include <OneWire.h>

#include <DallasTemperature.h>
// GPIO where the DS18B20 is connected to
#define oneWireBus 4
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);
// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature dsb(&oneWire);
const int phPin = 34;


#define flowPin  36

long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;
boolean ledState = LOW;
float calibrationFactor = 4.5;
volatile byte pulseCount;
byte pulse1Sec = 0;
float flowRate;
unsigned long flowMilliLitres;
unsigned int totalMilliLitres;
float flowLitres;
float totalLitres;

void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}

void readPH() {
  float analog_ph_value = analogRead(phPin);
  Serial.print(analog_ph_value);
  Serial.print(" | ");
  float voltage = analog_ph_value * (3.3 / 4095.0);
  float ph_value = (3.3 * voltage);
  Serial.print(ph_value);
  Serial.println(" pH");
}

void readTDS() {
  gravityTds.setTemperature(temperature);  // set the temperature and execute temperature compensation
  gravityTds.update();  //sample and calculate
  tds_value = gravityTds.getTdsValue();  // then get the value
  Serial.print(tds_value);
  Serial.println(" ppm");
}

void readDSB() {
  dsb.requestTemperatures();
  float temperatureC = dsb.getTempCByIndex(0);
  float temperatureF = dsb.getTempFByIndex(0);
  Serial.print(temperatureC);
  Serial.println("ºC");
  Serial.print(temperatureF);
  Serial.println("ºF");
}


void readFLOW() {
  currentMillis = millis();
  if (currentMillis - previousMillis > interval)
  {

    pulse1Sec = pulseCount;
    pulseCount = 0;

    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
    previousMillis = millis();

    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flowMilliLitres = (flowRate / 60) * 1000;
    flowLitres = (flowRate / 60);

    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres += flowMilliLitres;
    totalLitres += flowLitres;

    // Print the flow rate for this second in litres / minute
    Serial.print("Flow rate: ");
    Serial.print(float(flowRate));  // Print the integer part of the variable
    Serial.print("L/min");
    Serial.print("\t");       // Print tab space

    // Print the cumulative total of litres flowed since starting
    Serial.print("Output Liquid Quantity: ");
    Serial.print(totalMilliLitres);
    Serial.print("mL / ");
    Serial.print(totalLitres);
    Serial.println("L");

  }
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  // Start the DS18B20 sensor
  dsb.begin();
  pinMode(phPin, INPUT);
  pinMode(flowPin, INPUT_PULLUP);
  gravityTds.setPin(tdsPin);
  gravityTds.setAref(3.3);  //reference voltage on ADC, default 5.0V on  Arduino UNO
  gravityTds.setAdcRange(4096);  //1024 for 10bit ADC;4096 for 12bit ADC
  gravityTds.begin();  //initialization

  pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  previousMillis = 0;

  attachInterrupt(digitalPinToInterrupt(flowPin), pulseCounter, FALLING);
  delay(1000);
}
void loop() {
  readPH();
  readTDS();
  readFLOW();
  readDSB();
}
