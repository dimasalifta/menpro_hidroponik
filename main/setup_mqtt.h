
// Establish and Assigning MQTT Topics
const char* ssid = "KANTIN-LEMIGAS";               // Ganti dengan SSID WiFi Anda
const char* password = "lemigas2022";              // Ganti dengan password WiFi Anda lemigas2023
const char* mqtt_server = "dimasalifta.tech";      // Ganti dengan alamat mqtt broker Anda
const int mqtt_port = 1883;                        // Port default untuk mqtt
const char* mqtt_username = "YOUR_MQTT_USERNAME";  // Ganti dengan username mqtt Anda (jika diperlukan)
const char* mqtt_password = "YOUR_MQTT_PASSWORD";  // Ganti dengan password mqtt Anda (jika diperlukan)
const char* topic = "menpro/esp32/ph_sensor";                   // Ganti dengan topik mqtt Anda
const char* topic1 = "menpro/esp32/tds_sensor";                 // Ganti dengan topik mqtt Anda
const char* topic2 = "menpro/esp32/waterflow_sensor"; // Ganti dengan topik mqtt Anda
// const char* topic3 = "suhu_f"; // Ganti dengan topik mqtt Anda
const char* topic4 = "menpro/esp32/suhu_air";  // Ganti dengan topik mqtt Anda
const char* topic5 = "menpro/esp32/suhu_lingkungan";  // Ganti dengan topik mqtt Anda
const char* topic6 = "menpro/esp32/kelembaban";  // Ganti dengan topik mqtt Anda
// Initialize the PCF8574 with the I2C address and appropriate SDA/SCL pins
PCF8574 pcf8574(PCF8574_ADDRESS);  // Replace with your actual SDA and SCL pins
WiFiClient espClient;
PubSubClient client(espClient);
void relay_begin() {
  Wire.begin();            // Initialize the I2C communication
  pcf8574.write(0, HIGH);  // ph up
  pcf8574.write(1, HIGH);  // ph down
  pcf8574.write(2, HIGH);  // nutrisi a
  pcf8574.write(3, HIGH);  // nutrisi b
  pcf8574.write(4, HIGH);  // terserah
  pcf8574.write(5, HIGH);  // dah
  pcf8574.write(6, HIGH);  // isinya
  pcf8574.write(7, HIGH);  // apa
}
void relay_ah(int in, bool stats) {
  pcf8574.write(in, stats);  // Turn pin in LOW
  delay(5000);               // Wait for 1 second
  pcf8574.write(in, 1);      // Turn pin 0 HIGH
  // delay(5000);
}
void relay_ih() {
  pcf8574.write(0, 1);  // Turn pin 0 HIGH
  pcf8574.write(1, 1);  // Turn pin 0 HIGH
  pcf8574.write(2, 1);  // Turn pin 0 HIGH
  pcf8574.write(3, 1);  // Turn pin 0 HIGH
  // delay(5000);
}
void connect_mqtt() {
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

void send_mqtt(float ph, float tds, float flow, float dsb, float suhu, float kelembaban) {
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
  client.publish(topic, String(ph).c_str());
  client.publish(topic1, String(tds).c_str());
  client.publish(topic2, String(flow).c_str());
  client.publish(topic4, String(dsb).c_str());
  client.publish(topic5, String(suhu).c_str());
  client.publish(topic6, String(kelembaban).c_str());  
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off".
  // Changes the output state according to the message
  if (String(topic) == "phup/relay") {
    Serial.print("Changing output to ");
    if (messageTemp == "LOW") {
      // Serial.println("on");
      client.publish("phup/relay", "PH UP ON");
      relay_ah(0, 0);
      client.publish("phup/relay", "PH UP SELESAI");
    } else {
      relay_ih();
      client.publish("phup/relay", "PH UP OFF");
    }
  } else if (String(topic) == "phdown/relay") {
    Serial.print("Changing output to ");
    if (messageTemp == "LOW") {
      // Serial.println("on");
      client.publish("phdown/relay", "PH DOWN ON");
      relay_ah(1, 0);
      client.publish("phdown/relay", "PH DOWN SELESAI");
    } else {
      relay_ih();
      client.publish("phdown/relay", "PH DOWN OFF");
    }
  } else if (String(topic) == "nuta/relay") {
    Serial.print("Changing output to ");
    if (messageTemp == "LOW") {
      // Serial.println("on");
      client.publish("nuta/relay", "nutrisi A ON");
      relay_ah(2, 0);
      client.publish("nuta/relay", "nutrisi A SELESAI");
    } else {
      relay_ih();
      client.publish("nuta/relay", "nutrisi A OFF");
    }
  } else if (String(topic) == "nutb/relay") {
    Serial.print("Changing output to ");
    if (messageTemp == "LOW") {
      // Serial.println("on");
      client.publish("nutb/relay", "nutrisi B ON");
      relay_ah(3, 0);
      client.publish("nutb/relay", "nutrisi B SELESAI");
    } else {
      relay_ih();
      client.publish("nutb/relay", "nutrisi B OFFF");
    }
  }
}


// void callback(char* topic, byte* message, unsigned int length) {
//   Serial.print("Message arrived on topic: ");
//   Serial.print(topic);
//   Serial.print(". Message: ");
//   String messageTemp;

//   for (int i = 0; i < length; i++) {
//     Serial.print((char)message[i]);
//     messageTemp += (char)message[i];
//   }
//   Serial.println();

//   // Feel free to add more if statements to control more GPIOs with MQTT

//   // If a message is received on the topic esp32/output, you check if the message is either "on" or "off".
//   // Changes the output state according to the message
//   if (String(topic) == "esp32/output") {
//     Serial.print("Changing output to ");
//     if(messageTemp == "on"){
//       Serial.println("on");
//     }
//     else if(messageTemp == "off"){
//       Serial.println("off");
//     }
//   }
// }