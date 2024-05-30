#include <WiFi.h>
#include <PubSubClient.h>

// Establish and Assigning MQTT Topics
const char* ssid = "Galaxy A05s 15a9"; // Ganti dengan SSID WiFi Anda
const char* password = "Kianakaslana2407aru"; // Ganti dengan password WiFi Anda
const char* mqtt_server = "dimasalifta.tech"; // Ganti dengan alamat mqtt broker Anda
const int mqtt_port = 1883; // Port default untuk mqtt
const char* mqtt_username = "YOUR_MQTT_USERNAME"; // Ganti dengan username mqtt Anda (jika diperlukan)
const char* mqtt_password = "YOUR_MQTT_PASSWORD"; // Ganti dengan password mqtt Anda (jika diperlukan)
const char* topic = "ph_sensor"; // Ganti dengan topik mqtt Anda
const char* topic1 = "tds_sensor"; // Ganti dengan topik mqtt Anda
const char* topic2 = "suhu_c"; // Ganti dengan topik mqtt Anda
const char* topic3 = "suhu_f"; // Ganti dengan topik mqtt Anda
const char* topic4 = "waterflow_sensor"; // Ganti dengan topik mqtt Anda

WiFiClient espClient;
PubSubClient client(espClient);

void connect_mqtt(){
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
  }
}

void send_mqtt(){
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
  client.publish(topic, String(ph_value).c_str());
  client.publish(topic1, String(tds_value).c_str());
  // client.publish(topic2, String(temperatureC).c_str());
  // client.publish(topic3, String(temperatureF).c_str());
  client.publish(topic4, String(flowRate).c_str());
}