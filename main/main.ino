#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// GPIO where the DS18B20 is connected to
const int oneWireBus = 33;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);

#include <Wire.h>             // Include the Wire library for I2C
#include <PCF8574.h>          // Include the PCF8574 library
#define PCF8574_ADDRESS 0x20  // Replace withu your PCF8574 I2C address
#include "setup_mqtt.h"

#include <DHT.h>
#define DHT22_PIN 32

DHT dht22(DHT22_PIN, DHT22);

const int tdsPin = 34;  // Setup tds sensor pin
const int phPin = 35;   // Setup ph sensor pin

// Setup waterflow sensor pin and variable
#define flowPin 4  // Sensor pin
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

float ph_value;
float tds_value;

void IRAM_ATTR pulseCounter() {
  pulseCount++;
}

float readPH() {
  float analog_ph_value = analogRead(phPin);
  Serial.print(analog_ph_value);
  Serial.print(" | ");
  ph_value = ((0.0062 * (analog_ph_value)) - 2.1421);
  Serial.print(ph_value);
  Serial.println(" pH");
  return ph_value;
}

float readDSB() {
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);
  // float temperatureF = sensors.getTempFByIndex(0);
  Serial.print(temperatureC);
  Serial.println("ºC");
  // Serial.print(temperatureF);
  // Serial.println("ºF");
  return temperatureC;
}

float readTDS() {
  float tds_analog = analogRead(tdsPin);
  tds_value = (0.0005 * (tds_analog * tds_analog) - 1.5179 * (tds_analog) + 1416.9);  // then get the value
  Serial.print(tds_analog);
  Serial.print(" | ");
  Serial.print(tds_value);
  Serial.println(" ppm");
  return tds_value;
}

float readDHTtempC() {
  float tempC = dht22.readTemperature();
  Serial.print("Temperature: ");
  Serial.print(tempC);
  return tempC;
}

float readDHThumi() {
  // read humidity
  float humi = dht22.readHumidity();
  // // read temperature in Celsius
  // float tempC = dht22.readTemperature();
  // // read temperature in Fahrenheit
  // float tempF = dht22.readTemperature(true);

  // check whether the reading is successful or not
  // if (isnan(tempC) || isnan(tempF) || isnan(humi)) {
  //   Serial.println("Failed to read from DHT22 sensor!");
  // } else {
  Serial.print("Humidity: ");
  Serial.print(humi);
  //   Serial.print("%");

  //   Serial.print("  |  ");

  // Serial.print("Temperature: ");
  // Serial.print(tempC);
  //   Serial.println("°C");
  // }
  return humi;
}

float readFLOW() {
  currentMillis = millis();
  if (currentMillis - previousMillis > interval) {

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
    Serial.print("\t");  // Print tab space

    // Print the cumulative total of litres flowed since starting
    Serial.print("Output Liquid Quantity: ");
    Serial.print(totalMilliLitres);
    Serial.print("mL / ");
    Serial.print(totalLitres);
    Serial.println("L");
  }
  return float(flowRate);
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("LEMIGAS", "reconnect");
      // ... and resubscribe
      // client.subscribe("#");
      client.subscribe("phup/relay");
      client.subscribe("phdown/relay");
      client.subscribe("nuta/relay");
      client.subscribe("nutb/relay");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  relay_begin();
  connect_mqtt();
  client.setCallback(callback);
  dht22.begin();
  sensors.begin();


  pinMode(phPin, INPUT);
  pinMode(flowPin, INPUT_PULLUP);
  pinMode(tdsPin, INPUT);

  pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  previousMillis = 0;

  attachInterrupt(digitalPinToInterrupt(flowPin), pulseCounter, FALLING);
  client.subscribe("phup/relay");
  client.subscribe("phdown/relay");
  client.subscribe("nuta/relay");
  client.subscribe("nutb/relay");
  delay(1000);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  send_mqtt(readPH(), readTDS(), readFLOW(), readDSB(), readDHTtempC(), readDHThumi());
  delay(1000);
  client.loop();
}
