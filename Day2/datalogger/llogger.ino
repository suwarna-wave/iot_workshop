#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
// Install: DHT sensor library by Adafruit
#include <ArduinoJson.h>
// Install: ArduinoJson by Benoit Blanchon
#define DHTPIN 4
#define DHTTYPE DHT11
// GPIO 4 — DATA pin of DHT11
// DHT11 or DHT22
const char* ssid
= "YourSSID";
const char* password = "YourPassword";
// httpbin.org echoes POST body back — perfect for testing without a server
const char* serverUrl = "https://httpbin.org/post";
DHT dht(DHTPIN, DHTTYPE);
void postSensorData(float temp, float hum) {
HTTPClient http;
http.begin(serverUrl);
http.addHeader("Content-Type", "application/json");
// Build JSON payload
StaticJsonDocument<256> doc;
doc["device"]
= "ESP32_DHT11";
doc["temperature"] = serialized(String(temp, 1));
doc["humidity"]
= serialized(String(hum, 1));
doc["unit"]
= "celsius";
String jsonStr;
serializeJson(doc, jsonStr);
Serial.println("Sending JSON: " + jsonStr);
int code = http.POST(jsonStr);
if (code > 0) {
Serial.println("POST success! Response code: " + String(code));
String response = http.getString();
// httpbin returns the payload under 'data' field
Serial.println("Server echoed: " + response.substring(0, 200));
} else {
Serial.println("POST failed. Error: " + String(code));
}
http.end();
}
void setup() {
Serial.begin(115200);
dht.begin();
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
Serial.println("\nConnected! IP: " + WiFi.localIP().toString());
}
void loop() {
delay(2000);
// DHT11 needs 2 sec between readings
float temp = dht.readTemperature();
float hum = dht.readHumidity();
// Check if readings are valid
if (isnan(temp) || isnan(hum)) {
Serial.println("ERROR: Failed to read DHT11. Check wiring!");
return; // Skip this cycle
}
Serial.printf("Temp: %.1f°C
Humidity: %.1f%%\n", temp, hum);
if (WiFi.status() == WL_CONNECTED) {
postSensorData(temp, hum);
} else {
Serial.println("WiFi disconnected! Reconnecting...");
WiFi.reconnect();
}
delay(8000);
// Wait 8 more seconds (total 10s interval)
}