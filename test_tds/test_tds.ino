int TdsSensorPin = 35;

void setup(){
    Serial.begin(115200);
    pinMode(TdsSensorPin, INPUT);
}
 
void loop(){
    int analog_tds = analogRead(TdsSensorPin);
    float tds_value =  (0.0005*(analog_tds*analog_tds) - 1.5179*(analog_tds) + 1416.9);  // then get the value
    Serial.println(tds_value);
}
