#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include <GravityTDS.h>

#define SENSOR  D2            // Waterflow
#define SensorPin D4          // the pH meter Analog output is connected with the Arduino’s Analog
#define TdsSensorPin D5      // TDS
GravityTDS gravityTds;

unsigned long int avgValue;  //Store the average value of the sensor feedback
float b;
int buf[10], temp;

// Establish and Assigning MQTT Topics
const char* ssid = "Galaxy A05s 15a9"; // Ganti dengan SSID WiFi Anda
const char* password = "Kianakaslana2407aru"; // Ganti dengan password WiFi Anda
const char* mqtt_server = "dimasalifta.tech"; // Ganti dengan alamat mqtt broker Anda
const int mqtt_port = 1883; // Port default untuk mqtt
const char* mqtt_username = "YOUR_MQTT_USERNAME"; // Ganti dengan username mqtt Anda (jika diperlukan)
const char* mqtt_password = "YOUR_MQTT_PASSWORD"; // Ganti dengan password mqtt Anda (jika diperlukan)
const char* topic = "ph_sensor"; // Ganti dengan topik mqtt Anda
const char* topic1 = "tds_sensor"; // Ganti dengan topik mqtt Anda
const char* topic2 = "suhu_sensor"; // Ganti dengan topik mqtt Anda
const char* topic3 = "waterflow_sensor"; // Ganti dengan topik mqtt Anda

// Assigning Waterflow_sensor
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

// Assigning TDS
float temperature = 25, tdsValue = 0;

void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  // Assign Serial
  Serial.begin(115200);
  // Connect MQTT
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected!");

  client.setServer(mqtt_server, mqtt_port);

  if (mqtt_username != "") {
    client.setServer(mqtt_server, mqtt_port);
    pinMode(SENSOR, INPUT_PULLUP);
    // Assign Waterflow
    pulseCount = 0;
    flowRate = 0.0;
    flowMilliLitres = 0;
    totalMilliLitres = 0;
    previousMillis = 0;

    attachInterrupt(digitalPinToInterrupt(SENSOR), pulseCounter, FALLING);
  }
  gravityTds.setPin(TdsSensorPin);
  gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
  gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
  gravityTds.begin();  //initialization
}

void loop() {

  // Waterflow Sensor
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
  }

  // Sensor PH
  for (int i = 0; i < 10; i++) //Get 10 sample value from the sensor for smooth the value
  {
    buf[i] = analogRead(SensorPin);
    delay(10);
  }
  for (int i = 0; i < 9; i++) //sort the analog from small to large
  {
    for (int j = i + 1; j < 10; j++)
    {
      if (buf[i] > buf[j])
      {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  avgValue = 0;
  for (int i = 2; i < 8; i++)               //take the average value of 6 center sample
    avgValue += buf[i];
  float phValue = (float)avgValue * 5.0 / 1024 / 6; //convert the analog into millivolt
  phValue = 3.5 * phValue;                  //convert the millivolt into pH value
}


// TDS Sensor
gravityTds.setTemperature(temperature);  // set the temperature and execute temperature compensation
gravityTds.update();  //sample and calculate
tdsValue = gravityTds.getTdsValue();  // then get the value
//Serial.print(tdsValue,0);
//Serial.println("ppm");

// Sending data to MQTT
if (!client.connected()) {
  if (client.connect("ESP32Client")) {
    Serial.println("Connected to MQTT broker!");
  } else {
    Serial.print("Failed to connect to MQTT broker, rc=");
    Serial.print(client.state());
    Serial.println(" try again in 5 seconds");
    delay(5000);
  }
}
client.publish(topic, String(phValue).c_str());
client.publish(topic1, String(flowMilliLitres).c_str());
client.publish(topic2, String(temperature).c_str());
client.publish(topic3, String(tdsValue).c_str());
delay(2000);
}
