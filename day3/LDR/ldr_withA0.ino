#define AO_PIN 36  // ESP32's pin GPIO36 connected to AO pin of the ldr module

void setup() {
  // initialize serial communication
  Serial.begin(9600);

  // set the ADC attenuation to 11 dB (up to ~3.3V input)
  analogSetAttenuation(ADC_11db);
}

void loop() {
  int lightValue = analogRead(AO_PIN);

  Serial.print("The AO value: ");
  Serial.println(lightValue);
}
