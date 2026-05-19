const int NUMBER_OF_CHOICES = 4;

const int ledPins[NUMBER_OF_CHOICES] = {8, 9, 10, 11};
const int buttonPins[NUMBER_OF_CHOICES] = {2, 3, 4, 5};
const int buzzerPin = 6;

const char* colorNames[NUMBER_OF_CHOICES] = {
  "Red", "Yellow", "Green", "Blue"
};

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < NUMBER_OF_CHOICES; i++) {
    pinMode(ledPins[i], OUTPUT);
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  pinMode(buzzerPin, OUTPUT);

  Serial.println("Circuit Test Started");
  Serial.println("Press each button. Matching LED and buzzer should respond.");
}

void loop() {
  for (int i = 0; i < NUMBER_OF_CHOICES; i++) {
    if (digitalRead(buttonPins[i]) == LOW) {
      Serial.print(colorNames[i]);
      Serial.println(" button pressed");

      digitalWrite(ledPins[i], HIGH);
      tone(buzzerPin, 400 + i * 200);

      delay(200);

      digitalWrite(ledPins[i], LOW);
      noTone(buzzerPin);

      while (digitalRead(buttonPins[i]) == LOW) {
        delay(10);
      }
    }
  }
}
