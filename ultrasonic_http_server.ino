#include <WiFi.h>
#include <WebServer.h>

// WiFi Credentials
const char* ssid = "sudipnext";
const char* password = "sudip12345";

// Ultrasonic pins
const int trigPin = 5;
const int echoPin = 18;

WebServer server(80);

long duration;
float distance;

// WiFi connect
void connectWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());
}

// Measure distance
float getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2; // cm
}

// Dashboard page
void handleRoot() {
  distance = getDistance();

  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>ESP32 Distance Dashboard</title>";
  html += "<style>body{font-family:Arial;text-align:center;} .box{margin-top:50px;font-size:2em;color:#333;} .bar{width:80%;height:30px;background:#ddd;margin:20px auto;border-radius:15px;overflow:hidden;} .fill{height:100%;background:#4CAF50;text-align:right;color:white;padding-right:10px;line-height:30px;}</style>";
  html += "<meta http-equiv='refresh' content='1'>"; // refresh every 1s
  html += "</head><body>";
  html += "<h1>ESP32 Ultrasonic Dashboard</h1>";
  html += "<div class='box'>Distance: " + String(distance) + " cm</div>";
  int barValue = (int)min(distance, 100.0f);  // max bar 100cm
  html += "<div class='bar'><div class='fill' style='width:" + String(barValue) + "%'>" + String((int)distance) + " cm</div></div>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  connectWiFi();

  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
