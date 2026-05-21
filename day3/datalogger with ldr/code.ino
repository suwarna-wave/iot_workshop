#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
// Install: ArduinoJson by Benoit Blanchon

// ===== LDR Module Pins =====
#define LDR_AO_PIN 34   // Analog output pin of LDR module
#define LDR_DO_PIN 27   // Digital output pin of LDR module

// ===== Wi-Fi Credentials =====
const char* ssid = "YourSSID";
const char* password = "YourPassword";

// httpbin.org echoes POST body back — perfect for testing without a server
const char* serverUrl = "https://httpbin.org/post";

void postSensorData(int analogValue, int digitalValue, int lightPercent, String lightStatus) {
  HTTPClient http;

  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");

  // Build JSON payload
  StaticJsonDocument<256> doc;

  doc["device"] = "ESP32_LDR_Module";
  doc["analog_value"] = analogValue;
  doc["digital_value"] = digitalValue;
  doc["light_percent"] = lightPercent;
  doc["light_status"] = lightStatus;

  String jsonStr;
  serializeJson(doc, jsonStr);

  Serial.println("Sending JSON: " + jsonStr);

  int code = http.POST(jsonStr);

  if (code > 0) {
    Serial.println("POST success! Response code: " + String(code));

    String response = http.getString();
    Serial.println("Server echoed: " + response.substring(0, 200));
  } else {
    Serial.println("POST failed. Error: " + String(code));
  }

  http.end();
}

void setup() {
  Serial.begin(115200);

  pinMode(LDR_DO_PIN, INPUT);

  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Connected! IP: " + WiFi.localIP().toString());
}

void loop() {
  delay(2000);

  // Read LDR values
  int analogValue = analogRead(LDR_AO_PIN);
  int digitalValue = digitalRead(LDR_DO_PIN);

  // ESP32 ADC range is normally 0 to 4095
  // Depending on your module, high value may mean dark or bright.
  int lightPercent = map(analogValue, 0, 4095, 0, 100);

  String lightStatus;

  if (digitalValue == HIGH) {
    lightStatus = "Bright";
  } else {
    lightStatus = "Dark";
  }

  Serial.println("===== LDR Reading =====");
  Serial.println("Analog Value: " + String(analogValue));
  Serial.println("Digital Value: " + String(digitalValue));
  Serial.println("Light Percent: " + String(lightPercent) + "%");
  Serial.println("Light Status: " + lightStatus);

  if (WiFi.status() == WL_CONNECTED) {
    postSensorData(analogValue, digitalValue, lightPercent, lightStatus);
  } else {
    Serial.println("WiFi disconnected! Reconnecting...");
    WiFi.reconnect();
  }

  delay(8000); // Total around 10 seconds interval
}
