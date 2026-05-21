#define DO_PIN 13  // ESP32's pin GPIO13 connected to DO pin of the ldr module

void setup() {
  // initialize serial communication
  Serial.begin(9600);
  // initialize the ESP32's pin as an input
  pinMode(DO_PIN, INPUT);
}

void loop() {
  int lightState = digitalRead(DO_PIN);

  if (lightState == HIGH)
    Serial.println("It is dark");
  else
    Serial.println("It is light");
}
