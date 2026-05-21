#include <WiFi.h>

// ===== WiFi Credentials =====
const char* ssid = "Wi-Fi Name";
const char* password = "Password";

// ===== Pins =====
#define LDR_PIN 34      // Analog pin for LDR module
#define LED_PIN 2       // Built-in LED or external LED

WiFiServer server(80);

int ldrValue = 0;
String lightStatus = "";

void setup() {

  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);

  // Connect WiFi
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  // Start Server
  server.begin();
}

void loop() {

  // Read LDR Value
  ldrValue = analogRead(LDR_PIN);

  Serial.print("LDR Value: ");
  Serial.println(ldrValue);

  // Control LED based on light
  // Lower value = Dark (may vary by module)
  if (ldrValue < 2000) {
    digitalWrite(LED_PIN, HIGH);
    lightStatus = "Dark - LED ON";
  } else {
    digitalWrite(LED_PIN, LOW);
    lightStatus = "Bright - LED OFF";
  }

  // Web Client
  WiFiClient client = server.available();

  if (client) {

    client.readStringUntil('\r');

    // Send Webpage
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();

    client.println("<html>");
    client.println("<head>");
    client.println("<meta http-equiv='refresh' content='2'>");
    client.println("<title>ESP32 LDR Monitor</title>");
    client.println("</head>");

    client.println("<body style='font-family:Arial;text-align:center;'>");

    client.println("<h1>ESP32 LDR Web Server</h1>");

    client.print("<h2>LDR Value: ");
    client.print(ldrValue);
    client.println("</h2>");

    client.print("<h2>Status: ");
    client.print(lightStatus);
    client.println("</h2>");

    client.println("</body>");
    client.println("</html>");

    client.stop();
  }

  delay(500);
}
