#include <WiFi.h>
#include <MQTT.h>
#include <ArduinoJson.h>
// Install this library by going to menu Sketch > Include Libary > Manage Libraries...
// Search for StringSplitter and click Install.
#include "StringSplitter.h" 

char ssid[] = "Yellow Submarine";  // your network SSID
char pass[] = "@chterhoek!";  // your network password
char host[] = "test.mosquitto.org"; // don't change this.
char topic[] = "inc-hmm"; // don't change this.
String client_id = "esp1"; // but please DO change this :)

WiFiClient wifiClient;
MQTTClient client;

#define LED_PIN 26

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  // start wifi and mqtt
  WiFi.begin(ssid, pass);
  client.begin(host, wifiClient);
  client.onMessage(messageReceived);

  connectWifi();
  connectMqtt();

  sendMessage("connect");
}

void loop() {
  client.loop();
  // check if connected
  if (!client.connected()) {
    Serial.println("lost connection");
    connectMqtt();
  }
}

void connectWifi() {
  Serial.print("Connecting to wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nWifi connected!");
  delay(1000);
}

void connectMqtt() {
  Serial.print("\nConnecting to MQTT server...");
  while (!client.connect(client_id.c_str())) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nMQTT connected!");
  delay(1000);
  client.subscribe(topic);
}

void messageReceived(String &topic, String &payload) {
  Serial.println(topic + ": " + payload);
  
  StringSplitter *splitter = new StringSplitter(payload, ':', 3);
  int itemCount = splitter->getItemCount();

  if ( itemCount == 3 ){ // we have properly formatted command
    String origin = splitter->getItemAtIndex(0);
    String command = splitter->getItemAtIndex(1);
    String parameter = splitter->getItemAtIndex(2);

    if( origin == "server" ){
      if( command == "led" ) {
        if ( parameter == "on" ){
          digitalWrite( LED_PIN, HIGH );
        } else {
          digitalWrite( LED_PIN, LOW );
        }
      }
    }
  }
}

void sendMessage(String message) {
  sendMessage(message, "");
}

void sendMessage(String message, String parameter) {
  String msg = client_id + ":" + message + ":" + parameter;
  client.publish(topic, msg);
}
