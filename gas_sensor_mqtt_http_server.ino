#include <WiFi.h>
#include <PubSubClient.h>
#include <WebServer.h>

// ===== Wi-Fi Credentials =====
const char* ssid = "sudipnext";
const char* password = "sudip12345";

// ===== MQTT Broker Credentials =====
const char* mqtt_server = "172.25.99.184";  // e.g., 192.168.1.100
const int mqtt_port = 1883; 
const char* mqtt_pub_topic = "sensor/gas";

// ===== Gas Sensor Pin =====
#define MQ_PIN 34  // AO pin connected to ESP32 ADC

WiFiClient espClient;
PubSubClient client(espClient);
WebServer server(80);

int gasValue = 0;

// ===== Connect to WiFi =====
void connectWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());
}

// ===== Connect to MQTT =====
void connectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32GasPublisher")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 2 seconds");
      delay(2000);
    }
  }
}

// ===== HTTP Handler =====
void handleRoot() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<title>MQ Gas Sensor Dashboard</title>";
  html += "<style>body{font-family:Arial;text-align:center;} .box{margin-top:50px;font-size:2em;} .bar{width:80%;height:30px;background:#ddd;margin:20px auto;border-radius:15px;overflow:hidden;} .fill{height:100%;background:#f44336;text-align:right;color:white;padding-right:10px;line-height:30px;}</style>";
  html += "<meta http-equiv='refresh' content='1'>";  // refresh every 1s
  html += "</head><body>";
  html += "<h1>MQ Gas Sensor Dashboard</h1>";
  html += "<div class='box'>Sensor Value: " + String(gasValue) + "</div>";

  int barValue = map(gasValue, 0, 4095, 0, 100);  // scale ADC 0-4095 → 0-100%
  html += "<div class='bar'><div class='fill' style='width:" + String(barValue) + "%'>" + String(gasValue) + "</div></div>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  pinMode(MQ_PIN, INPUT);

  // Connect WiFi
  connectWiFi();

  // Setup MQTT
  client.setServer(mqtt_server, mqtt_port);

  // Setup HTTP server
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  if (!client.connected()) {
    connectMQTT();
  }
  client.loop();

  // Read gas sensor
  gasValue = analogRead(MQ_PIN);
  Serial.print("MQ Sensor Value: ");
  Serial.println(gasValue);

  // Publish to MQTT
  String msg = String(gasValue);
  client.publish(mqtt_pub_topic, msg.c_str());

  // Handle HTTP server
  server.handleClient();

  delay(1000);  // update every second
}
