#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

const char* ssid = "Poco F6";
const char* password = "abcdef2/";
const char* serverUrl = "http://192.168.167.47/data";

#define TRIG_PIN 14 // D5
#define ECHO_PIN 12 // D6
#define DHT_PIN 13  // D7
#define DHT_TYPE DHT11

DHT dht(DHT_PIN, DHT_TYPE);

WiFiClient wifiClient;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  dht.begin();
}

long measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  long distance = duration * 0.034 / 2;

  return distance;
}

void loop() {
  long distance = measureDistance();
  float humidity = dht.readHumidity();

  Serial.print("Distance: ");
  Serial.println(distance);
  Serial.print("Humidity: ");
  Serial.println(humidity);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(wifiClient, serverUrl);  // Gunakan WiFiClient sebagai parameter
    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<256> doc;
    doc["distance"] = distance;
    doc["humidity"] = humidity;

    String requestBody;
    serializeJson(doc, requestBody);

    int httpResponseCode = http.POST(requestBody);
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }

  delay(5000); // Send data every 5 seconds
}
