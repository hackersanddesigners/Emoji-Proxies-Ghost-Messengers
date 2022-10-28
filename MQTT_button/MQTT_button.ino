#include <WiFi.h>
#include <MQTT.h>
#include <ArduinoJson.h>

char ssid[] = "Yellow Submarine";  // your network SSID
char pass[] = "@chterhoek!";  // your network password
char host[] = "test.mosquitto.org"; // don't change this.
char topic[] = "inc-hmm"; // don't change this.
String client_id = "esp1"; // but please change this :)

WiFiClient wifiClient;
MQTTClient client;

#define BTN_PIN 27
#define LED_BUILTIN 2

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);

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
    delay(5000); // prevent flooding the server
  }
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  if ( digitalRead( BTN_PIN  ) == LOW ) {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    sendMessage("emoji", "woof");
    delay(200);
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
  // we're only sending in this example, so nothing happens here.
}

void sendMessage(String message) {
  sendMessage(message, "");
}

void sendMessage(String message, String parameter) {
  String msg = client_id + ":" + message + ":" + parameter;
  client.publish(topic, msg);
}
