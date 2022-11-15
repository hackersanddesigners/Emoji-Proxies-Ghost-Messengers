#include <WiFi.h>
#include <MQTT.h>
#include <ArduinoJson.h>

char ssid[] = "Yellow Submarine";  // Replace with your network SSID
char pass[] = "@chterhoek!";  // Replace with your network password
char host[] = "test.mosquitto.org"; // don't change this.
char topic[] = "inc-hmm"; // don't change this.
String client_id = "esp1-button"; // but please change this :)

WiFiClient wifiClient;
MQTTClient client;

#define BTN_PIN 27
#define LED_BUILTIN 26

/*
Setup get run when the ESP32 starts up. 
*/
void setup() {
  Serial.begin(115200); // serial communication for debugging
  pinMode(BTN_PIN, INPUT_PULLUP); // configure pin 27 as an input.
	// we're using INPUT_PULLUP mode, this makes connecting a button 
	// very easy. No resistors needed. 
	// More info: https://roboticsbackend.com/arduino-input_pullup-pinmode/ 

  // start wifi and mqtt
  WiFi.begin(ssid, pass);
  client.begin(host, wifiClient);
  client.onMessage(messageReceived); // call the messageReceived function when a message is received
	// connect wifi and mqtt
  connectWifi();
  connectMqtt();
	// say hi
  sendMessage("connect");
}
  
/*
* The loop function continuously check for new MQTT messages
* 
*/
void loop() {
  client.loop();
  // check if connected to the MQTT relay server
  if (!client.connected()) {
    Serial.println("lost connection");
    connectMqtt();
    delay(5000); // prevent flooding the server
  }
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  if ( digitalRead( BTN_PIN  ) == LOW ) { // if the button is pressed it will read as LOW here
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    sendMessage("rotate"); // send the message
    delay(200); // debounce the button and prevent flooding the server.
  }
}


/* ---------------------- you can ignore code below this line --------------------- */

/* 
* Wait for wifi connection 
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
* Wait for MQTT connection and subscribe to the topic
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
* In this example we're only sending, but recieved messages
* would be handled by this method.
*/
void messageReceived(String &topic, String &payload) {
  Serial.println(topic + ": " + payload);
  // we're only sending in this example, so nothing happens here.
}


/* 
* Concat the command and parameter into a colon separated string 
* and publish the message 
*/
void sendMessage(String message, String parameter) {
  String msg = client_id + ":" + message + ":" + parameter;
  client.publish(topic, msg);
}

/* 
* Overloaded sendMessage function without the parameter 
*/
void sendMessage(String message) {
  sendMessage(message, "");
}
