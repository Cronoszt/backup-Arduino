#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

#define RELAY3_PIN 26  // IN3 for channel 3 relay (assumed active-low)
#define BUZZER_PIN 32  // Buzzer (assumed active-high)

const char* ssid = "Poco F6";
const char* password = "abcdef2/";

WebServer server(80);

int receivedDistance = 0;
float receivedHumidity = 0.0;

void handleRoot() {
  String html = "<html><body><h1>ESP-WROOM-32 Sensor Data</h1>";
  html += "<p>Distance: " + String(receivedDistance) + " cm</p>";
  html += "<p>Humidity: " + String(receivedHumidity) + " %</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleData() {
  if (server.hasArg("plain") == false) {
    server.send(400, "text/plain", "Body not received");
    return;
  }

  String body = server.arg("plain");
  StaticJsonDocument<256> doc;
  deserializeJson(doc, body);

  receivedDistance = doc["distance"];
  receivedHumidity = doc["humidity"];
  Serial.print("Distance received: ");
  Serial.println(receivedDistance);
  Serial.print("Humidity received: ");
  Serial.println(receivedHumidity);

  // Inverted logic for active-low relay
  if (receivedDistance < 30) {
    digitalWrite(RELAY3_PIN, LOW);  // Turn ON the relay (active-low)
    digitalWrite(BUZZER_PIN, HIGH); // Turn ON the buzzer (active-high)
  } else {
    digitalWrite(RELAY3_PIN, HIGH); // Turn OFF the relay (active-low)
    digitalWrite(BUZZER_PIN, LOW);  // Turn OFF the buzzer (active-high)
  }

  server.send(200, "application/json", "{\"status\":\"success\"}");
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, handleRoot);
  server.on("/data", HTTP_POST, handleData);

  server.begin();
  Serial.println("HTTP server started");

  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Ensure both relay and buzzer start in OFF state
  digitalWrite(RELAY3_PIN, HIGH);  // Relay off (active-low)
  digitalWrite(BUZZER_PIN, LOW);   // Buzzer off (active-high)
}

void loop() {
  server.handleClient();
}
