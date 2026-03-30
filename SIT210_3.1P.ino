#include <WiFiNINA.h>
#include <PubSubClient.h>


#define WIFI_SSID "Starlink"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"


#define MQTT_SERVER "d83b06ae45974bdea2e0adfb8248d669.s1.eu.hivemq.cloud"
#define MQTT_PORT 8883
#define MQTT_TOPIC "sensor/motion"
#define MQTT_USER "YOUR_MQTT_USERNAME"
#define MQTT_PASSWORD "YOUR_MQTT_PASSWORD"


const int pirSensorPin = 2;
int motionDetected = 0;
int previousMotionState = LOW;


WiFiSSLClient wifiSSLClient;
PubSubClient mqttClient(wifiSSLClient);

void connectMQTT() {
  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT...");
    if (mqttClient.connect("ArduinoNanoIoT", MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("Connected to MQTT Broker!");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(pirSensorPin, INPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  connectMQTT();
}

void loop() {
  if (!mqttClient.connected()) {
    connectMQTT();
  }
  mqttClient.loop();

  motionDetected = digitalRead(pirSensorPin);

  if (motionDetected != previousMotionState) {
    if (motionDetected == HIGH) {
      Serial.println("Motion Detected!");
      mqttClient.publish(MQTT_TOPIC, "Motion detected");
    } else {
      Serial.println("No motion");
      mqttClient.publish(MQTT_TOPIC, "No motion detected");
    }

    previousMotionState = motionDetected;
  }

  delay(500);
}