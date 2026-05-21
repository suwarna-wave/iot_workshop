#include <WiFi.h>
#include "DHT.h"

// ===== WiFi Credentials =====
const char* ssid = "Wi-Fi Name";
const char* password = "Password";

// ===== DHT Setup =====
#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

WiFiServer server(80);

void setup() {
  Serial.begin(115200);

  // Start DHT Sensor
  dht.begin();

  // Connect to WiFi
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  // Start Web Server
  server.begin();
}

void loop() {

  // Read Sensor Data
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check if sensor works
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read DHT sensor");
    return;
  }

  // Check for client
  WiFiClient client = server.available();

  if (client) {

    // Read browser request
    client.readStringUntil('\r');

    // Send webpage
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();

    client.println("<html><body style='font-family:Arial;text-align:center;'>");

    client.println("<h1>ESP32 DHT11 Monitor</h1>");

    client.print("<h2>Temperature: ");
    client.print(temperature);
    client.println(" °C</h2>");

    client.print("<h2>Humidity: ");
    client.print(humidity);
    client.println(" %</h2>");

    client.println("</body></html>");

    client.stop();
  }

  delay(2000);
}

