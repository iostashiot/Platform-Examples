/*
 IOStash Platform Example for Arduino on ESP8266
 Copyright(c) 2016 IOStash
 http://iostash..com

 Replace all variables to match your own values
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Update these with values suitable for your network.
const char* ssid = "YOUR_WIFI_SSID_HERE";
const char* password = "YOUR_WIFI_PASSWORD_HERE";
const char* mqtt_server = "api.iostash.io";
const char* mqtt_username = "X-ACCESS-TOKEN_HERE";
const char* mqtt_password = "123";
const char* mqtt_topic = "/X-ACCESS-TOKEN_HERE/DEVICE_ID_HERE/custom";
const char* mqtt_pub_topic = "/X-ACCESS-TOKEN_HERE/DEVICE_ID_HERE/";

String dataString;
char charBuf[100];
int value;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
DynamicJsonBuffer jsonBuffer;

void setup() {
  //Bulb is connected to Pin 16 which is D0 on Lolin NodeMCU v3
  pinMode(16, OUTPUT);  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
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

void callback(char* topic, byte* payload, unsigned int length) {
   char json[250];
  for (int i = 0; i < length; i++) {
    json[i] = (char)payload[i];
  }
  JsonObject& root = jsonBuffer.parseObject(json);
  value = root["publishData"]["msg"];
  
Serial.println(sensor);

  // Switch on the bulb if an 1 was received as msg
  if (value == 1) {
    digitalWrite(16, HIGH);   
    Serial.println("System ON!");
    
  } else {
    digitalWrite(16, LOW);  // Turn the bulb off by making the voltage LOW
    Serial.println("System OFF");
  }
  dataString = String("{\"state\":") + value + String("}");
  dataString.toCharArray(charBuf, 150);
  if(client.publish(mqtt_pub_topic, charBuf )) {
    Serial.println("State published");
  }
  else
   Serial.println("Publish Failed");
  
}

// Reconnection logic in case of disconnection
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266_Client", mqtt_username, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// Main loop()
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

}