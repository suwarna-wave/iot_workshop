#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// ===== Wi-Fi Credentials =====
const char* ssid = "sudipnext";        
const char* password = "sudip12345";   

// Create AsyncWebServer on port 80
AsyncWebServer server(80);

// JSON variables
float temperature = 0;
float humidity = 0;
float wind_speed = 0;
int wind_direction = 0;
String sunrise = "";
String sunset = "";
float sunshine_duration = 0;

// ===== Function to connect to Wi-Fi =====
void connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// ===== Function to fetch weather data =====
void fetchWeatherData() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure(); // <-- disable certificate validation

    HTTPClient http;
    if (http.begin(client, "https://api.open-meteo.com/v1/forecast?latitude=26.4805&longitude=87.854&daily=sunrise,sunset,sunshine_duration&current=temperature_2m,relative_humidity_2m,apparent_temperature,wind_speed_10m,wind_direction_10m&timezone=auto")) {
      int httpCode = http.GET();
      if (httpCode > 0) {
        String payload = http.getString();
        Serial.println(payload);

        // Parse JSON
        DynamicJsonDocument doc(8192);  // larger buffer
        DeserializationError error = deserializeJson(doc, payload);
        if (!error) {
          JsonObject current = doc["current"];
          temperature = current["temperature_2m"];
          humidity = current["relative_humidity_2m"];
          wind_speed = current["wind_speed_10m"];
          wind_direction = current["wind_direction_10m"];

          JsonObject daily = doc["daily"];
          sunrise = daily["sunrise"][0].as<String>();
          sunset = daily["sunset"][0].as<String>();
          sunshine_duration = daily["sunshine_duration"][0];
        } else {
          Serial.println("Failed to parse JSON");
        }
      } else {
        Serial.printf("HTTP GET failed, code: %d\n", httpCode);
      }
      http.end();
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(10);

  connectToWiFi();

  // Serve HTML dashboard
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html = "<!DOCTYPE html><html lang='en'><head>";
    html += "<meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<title>ESP32 Weather Dashboard</title>";
    html += "<style>";
    html += "body{font-family:Arial;text-align:center;background:#f4f4f4;}";
    html += ".card{background:white;padding:20px;margin:20px;border-radius:10px;box-shadow:0 2px 5px rgba(0,0,0,0.3);display:inline-block;}";
    html += "</style></head><body>";
    html += "<h1>Weather Dashboard - Jhapa</h1>";

    html += "<div class='card'><h2>Temperature</h2><p>" + String(temperature) + " °C</p></div>";
    html += "<div class='card'><h2>Humidity</h2><p>" + String(humidity) + " %</p></div>";
    html += "<div class='card'><h2>Wind Speed</h2><p>" + String(wind_speed) + " km/h</p></div>";
    html += "<div class='card'><h2>Wind Direction</h2><p>" + String(wind_direction) + " °</p></div>";
    html += "<div class='card'><h2>Sunrise</h2><p>" + sunrise + "</p></div>";
    html += "<div class='card'><h2>Sunset</h2><p>" + sunset + "</p></div>";
    html += "<div class='card'><h2>Sunshine Duration</h2><p>" + String(sunshine_duration) + " s</p></div>";

    html += "<p>Data updates every 10 seconds</p>";
    html += "<script>setTimeout(()=>{location.reload();},10000);</script>";
    html += "</body></html>";

    request->send(200, "text/html", html);
  });

  server.begin();
}

void loop() {
  fetchWeatherData(); 
  delay(10000);
}
