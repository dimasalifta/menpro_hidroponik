#include <DHT.h>
#define DHT22_PIN 32

DHT dht22(DHT22_PIN, DHT22);

void setup() {
  Serial.begin(115200);
  dht22.begin();
}

void loop() {
  // read humidity
  float humi  = dht22.readHumidity();
  // read temperature in Celsius
  float tempC = dht22.readTemperature();
  // read temperature in Fahrenheit
  float tempF = dht22.readTemperature(true);

  // check whether the reading is successful or not
  if ( isnan(tempC) || isnan(tempF) || isnan(humi)) {
    Serial.println("Failed to read from DHT22 sensor!");
  } else {
    Serial.print("Humidity: ");
    Serial.print(humi);
    Serial.print("%");

    Serial.print("  |  ");

    Serial.print("Temperature: ");
    Serial.print(tempC);
    Serial.print("°C");
  }
  // wait a 2 seconds between readings
  delay(2000);
}
