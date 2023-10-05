#include <WiFi.h>
#include <MQTT.h>
#include <ArduinoJson.h>
// Install this library by going to menu Sketch > Include Libary > Manage Libraries...
// Search for StringSplitter and click Install.
#include "StringSplitter.h"

char ssid[] = "";  // Replace with your network SSID
char pass[] = "";       // Replace with your network password

char host[] = "test.mosquitto.org";  // don't change this.
char topic[] = "inc-hmm";            // don't change this.
String client_id = "Hmmosphere";           // but please DO change this :)

WiFiClient wifiClient;
MQTTClient client;

#define BTN_RELAY_PIN 5
#define PWR_RELAY_PIN 4

bool power_relay_state = HIGH;
bool scent_relay_state = HIGH;
unsigned long scent_relay_end_time = millis() + 200;

void setup() {
  Serial.begin(115200);            // serial communication for debugging
  pinMode(BTN_RELAY_PIN, OUTPUT);  // configure pin 26 as a output.
  pinMode(PWR_RELAY_PIN, OUTPUT);  // configure pin 27 as a output.
  digitalWrite(PWR_RELAY_PIN, HIGH);
  digitalWrite(BTN_RELAY_PIN, HIGH);

  // start wifi and mqtt
  WiFi.begin(ssid, pass);
  client.begin(host, wifiClient);
  client.onMessage(messageReceived);  // call the messageReceived function when a message is received
  // connect wifi and mqtt
  connectWifi();
  connectMqtt();
  // say hi
  sendMessage("connect");

}

/*
   The loop function continuously check for new MQTT messages
*/
void loop() {
  client.loop();
  // check if connected to the MQTT relay server
  if (!client.connected()) {
    Serial.println("lost connection");
    connectMqtt();
    delay(5000);  // prevent flooding the server
  }

  // update relay states based on current time
  unsigned long current_time = millis();
  if (scent_relay_state == LOW && current_time >= scent_relay_end_time) {
    scent_relay_state = HIGH; // turn scent relay off
  }

  // update relay outputs
  digitalWrite(PWR_RELAY_PIN, power_relay_state);
  digitalWrite(BTN_RELAY_PIN, scent_relay_state);
}

/*
   The messageReceived function is called when we receive a message over MQTT.
   Each message is formatted like this:
   `origin:command:parameter`
   `origin` is the sender, here we're looking for messages received from `server`.
   `command` is the command to be executed, in this example it's the command `relay` we're waiting for.
   `parameter` can contain extra information, here it can be `on` to turn the relay on and
   anything else in the parameter will turn the led off.
*/
void messageReceived(String &topic, String &payload) {
  Serial.println(topic + ": " + payload);

  StringSplitter *splitter = new StringSplitter(payload, ':', 3);
  int itemCount = splitter->getItemCount();

  if (itemCount == 3) {  // we have properly formatted command
    String origin = splitter->getItemAtIndex(0);
    String command = splitter->getItemAtIndex(1);
    String parameter = splitter->getItemAtIndex(2);

    Serial.println(parameter);

    if (origin == "server") {
      if (command == "scent_power") {
        if (parameter == "on" || parameter == "") {
          power_relay_state = LOW;  // turn power on
        } else if (parameter == "off") {
          power_relay_state = HIGH;
        }
      }
      if (command == "scent") {
        if (parameter == "on" || parameter == "") {
          scent_relay_state = LOW; // set scent relay to on
          scent_relay_end_time = millis() + 200; // set time to turn scent relay off
        } else if (parameter == "long") {
          scent_relay_state = LOW; // set scent relay to on
          scent_relay_end_time = millis() + 2500; // set time to turn scent relay off
        }
      }
    }
  }
}

/* ---------------------- you can ignore code below this line --------------------- */

/*
   Wait for wifi connection
*/
void connectWifi() {
  Serial.print("Connecting to wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nWifi connected!");
  delay(1000);
}

/*
   Wait for MQTT connection and subscribe to the topic
*/
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

/*
   Concat the command and parameter into a colon separated string
   and publish the message
*/
void sendMessage(String message, String parameter) {
  String msg = client_id + ":" + message + ":" + parameter;
  client.publish(topic, msg);
}

/*
   Overloaded sendMessage function without the parameter
*/
void sendMessage(String message) {
  sendMessage(message, "");
}
