#include <WiFi.h>
#include <PubSubClient.h>

// ===== Wi-Fi Credentials =====
const char* ssid = "sudipnext";
const char* password = "sudip12345";

// ===== MQTT Broker Info =====
const char* mqtt_server = "172.25.99.184";  // Or your own broker IP
const int mqtt_port = 1883;
const char* sub_topic = "esp/control";
const char* pub_topic = "esp/ack";

// ===== Pin =====
#define RELAY_PIN 2   // D2 = GPIO2 on ESP32

// Objects
WiFiClient espClient;
PubSubClient client(espClient);

// ===== WiFi Connect =====
void connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// ===== MQTT Callback =====
void callback(char* topic, byte* message, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)message[i];
  }
  Serial.printf("Message arrived [%s]: %s\n", topic, msg.c_str());

  if (msg == "on") {
    digitalWrite(RELAY_PIN, HIGH);
  } else if (msg == "off") {
    digitalWrite(RELAY_PIN, LOW);
  }

  // Publish acknowledgment
  client.publish(pub_topic, msg.c_str());
}

// ===== MQTT Connect =====
void connectToMQTT() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      client.subscribe(sub_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5s...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  connectToWiFi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}


void loop() {
  if (!client.connected()) {
    connectToMQTT();
  }
  client.loop();
}
