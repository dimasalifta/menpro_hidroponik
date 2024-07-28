#define ph_pin 32
float ph_value;
 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(ph_pin,INPUT);
  delay(1000);
}
 void loop(){
  float analog_ph_value = analogRead(ph_pin);
  Serial.print(analog_ph_value);
  Serial.print(" | ");
  ph_value = ((0.0062*(analog_ph_value)) - 2.1421);
  Serial.print(ph_value);
  Serial.println(" pH");
  delay(500);
 }