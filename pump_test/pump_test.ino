#define NUM_PUMPS 4
#define tds_pin 32
#define ph_pin 33

const int pumpPins[NUM_PUMPS] = {10,11,12,13}; // nuta, nutb, phup, phdown

float tds_value;
float ph_value;

int min_tds_treshold = 300;
int max_tds_treshold = 1000;

float min_ph_treshold = 5.3;
float max_ph_treshold = 6.7;
void pump_status (){
  pinMode (pumpPins[NUM_PUMPS], OUTPUT);
  if (tds_value <= min_tds_treshold) {
    digitalWrite(pumpPins[0], HIGH);
    digitalWrite(pumpPins[1], LOW);
    digitalWrite(pumpPins[2], LOW);
    digitalWrite(pumpPins[3], LOW);
    Serial.println("pump 1 turned on");
  } else if (tds_value >= max_tds_treshold) {
    digitalWrite(pumpPins[0], LOW);
    digitalWrite(pumpPins[1], HIGH);
    digitalWrite(pumpPins[2], LOW);
    digitalWrite(pumpPins[3], LOW);
    Serial.println("pump 2 turned on");
  } else if (ph_value >= min_ph_treshold) {
    digitalWrite(pumpPins[0], LOW);
    digitalWrite(pumpPins[1], LOW);
    digitalWrite(pumpPins[2], HIGH);
    digitalWrite(pumpPins[3], LOW);
    Serial.println("pump 3 turned on");
  } else if (ph_value >= max_ph_treshold) {
    digitalWrite(pumpPins[0], LOW);
    digitalWrite(pumpPins[1], LOW);
    digitalWrite(pumpPins[2], LOW);
    digitalWrite(pumpPins[3], HIGH);
    Serial.println("pump 4 turned on");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(tds_pin, INPUT);
}

void loop() {
  tds_value = analogRead(tds_pin);
  ph_value = analogRead(ph_pin);
  pump_status();
  // put your main code here, to run repeatedly:
  delay(1000);
}
