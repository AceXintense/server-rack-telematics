//#include <M5Stack.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include <OneWire.h>
#include <DallasTemperature.h>
 
const char* ssid = "VM513630 ";
const char* password =  "VXy3z7*ssP9";
const char* mqttServer = "192.168.1.163";
const int mqttPort = 1883;
const char* mqttUser = "yourMQTTuser";
const char* mqttPassword = "yourMQTTpassword";

byte* temperature;
int temperatureLength;
 
WiFiClient espClient;
PubSubClient client(espClient);

// GPIO where the DS18B20 is connected to
const int oneWireBus = 15;     

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);
 
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  temperature = payload;
  temperatureLength = length;
}
 
void setup() {
 
  Serial.begin(115200);
  sensors.begin();

  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected");
 
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("ESP32Client")) {
 
      Serial.println("Connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
 
}
 
void loop() {
  sensors.requestTemperatures(); 

  float backTemperature = sensors.getTempCByIndex(0);
  float frontTemperature = sensors.getTempCByIndex(1);
  float bottomTemperature = sensors.getTempCByIndex(2);
  float topTemperature = sensors.getTempCByIndex(3);
  float ambientTemperature = sensors.getTempCByIndex(4);

  Serial.print("Back ");
  Serial.print(backTemperature);
  Serial.println("ºC");

  Serial.print("Front ");
  Serial.print(frontTemperature);
  Serial.println("ºC");

  Serial.print("Bottom ");
  Serial.print(bottomTemperature);
  Serial.println("ºC");

  Serial.print("Top ");
  Serial.print(topTemperature);
  Serial.println("ºC");

  Serial.print("Ambient ");
  Serial.print(ambientTemperature);
  Serial.println("ºC");

  char s[5];

  float averageInternalTemperature = ((backTemperature + frontTemperature + bottomTemperature + topTemperature) / 4);
  
  Serial.print("Average Internal ");
  Serial.print(averageInternalTemperature);
  Serial.println("ºC");
  
  float deltaTemperature = (ambientTemperature - averageInternalTemperature);

  Serial.print("Delta ");
  Serial.print(deltaTemperature);
  Serial.println("ºC");

  client.publish("cabinet/back", dtostrf(backTemperature, 6, 2, s));
  client.publish("cabinet/front", dtostrf(frontTemperature, 6, 2, s));
  client.publish("cabinet/bottom", dtostrf(bottomTemperature, 6, 2, s));
  client.publish("cabinet/top", dtostrf(topTemperature, 6, 2, s));
  client.publish("cabinet/ambient", dtostrf(ambientTemperature, 6, 2, s));
  client.publish("cabinet/delta", dtostrf(deltaTemperature, 6, 2, s));
  
  client.loop();

  delay(5000);
}
