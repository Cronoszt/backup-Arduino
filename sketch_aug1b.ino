#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Poco F6";
const char* password = "abcdef2/";
const char* mqtt_server = "broker.emqx.io";
const char* mqtt_client_id = "mqttx_0ed2be54";
const char* topic = "esp/sensor";

WiFiClient espClient;
PubSubClient client(espClient);

#define RELAY_PIN 16
#define BUZZER_PIN 21

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(mqtt_client_id)) {
      Serial.println("connected");
      client.subscribe(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(message);

  float distance = message.substring(message.indexOf("Distance: ") + 10, message.indexOf("cm")).toFloat();

  if (distance < 30) {
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
