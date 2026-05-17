// MQ sensor AO pin
#define MQ_PIN 34   // ADC pin on ESP32

int sensorValue = 0;


void setup() {
  Serial.begin(115200);
  pinMode(MQ_PIN, INPUT);
}

void loop() {
  sensorValue = analogRead(MQ_PIN);  // 0 - 4095
  Serial.print("MQ Sensor Value (AO): ");
  Serial.println(sensorValue);
  delay(1000);
}
