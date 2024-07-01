int TdsSensorPin = 35;

void setup(){
    Serial.begin(115200);
    pinMode(TdsSensorPin, INPUT);
}
 
void loop(){
    int sensor_val = analogRead(TdsSensorPin);
    Serial.println(sensor_val);
}
