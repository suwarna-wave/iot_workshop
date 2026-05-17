#include <ESP8266WiFi.h>

// ===== Wi-Fi Credentials =====
const char* ssid = "sudipnext";         // Replace with your Wi-Fi name
const char* password = "sudip12345"; // Replace with your Wi-Fi password

// ===== Function to connect to Wi-Fi =====
void connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int retry = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    retry++;
    if (retry > 60) { // Retry for 30 seconds
      Serial.println();
      Serial.println("Failed to connect. Restarting...");
      ESP.restart(); // Restart the ESP if unable to connect
    }
  }

  Serial.println();
  Serial.println("Wi-Fi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);  // Start serial communication
  delay(10);

  connectToWiFi();       // Connect to Wi-Fi on startup
}

void loop() {
  // ===== Keep checking Wi-Fi status =====
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected! Reconnecting...");
    connectToWiFi();
  }

  // ===== Example: print Wi-Fi signal strength every 5 seconds =====
  Serial.print("Wi-Fi RSSI: ");
  Serial.println(WiFi.RSSI());
  delay(5000);
}
