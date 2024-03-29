# MQTT Relay

In this last example we hook up a relay to the ESP32 module. A relay is an electrically operated switch. We can use this to turn regular electronic devices on or off.

**Please note** that the relay modules we are using have a active low input and *ONLY WORK ON 3.3V!*. This means that making the IN input pin LOW, will turn 'on' the relay (this will connect COM and NO).
The relay has three output terminals. COM is the common wire and *NC* and *NO* are *N*ormally *C*losed and *N*ormally *O*pen.

The ESP32 module connects to the MQTT server (topic hmm-inc) and waits for the command:
`server:relay:on`. When this message is recieved the relay turns on for 3 seconds.


There are comment in the code, also included below, that describe how this works. 

Wire the led between pin D26 and GND on the arduino. In the images below you see how this should look on a breadboard and what it looks like as a circuit diagram. 

![wiring](MQTT_relay_bb.png)
![wiring](MQTT_relay_sch.png)

## Chat messages + parameters Relay

In the example only an ON state is described. The relay will turn on for 3 seconds and automatically turns off. 

* `/relay:on`

You can only turn a relay on or off (HIGH or LOW) in the code. 

You can also use the relay to power long LED strips (which require a lot of power). See also [alternating ledstrips with a relay](http://www.techydiy.org/keyes-sr1y-relay-module/)

## Required libraries

Install via Arduino IDE
> Sketch > Include Library.... > Library manager

Then you can search for these and install them:

>>![ArduinoJSON](../../assets/Wifi.png)
>>![MQTT library](../../assets/ArduinoJSON_lib.png)
>>![ArduinoJSON](../../assets/MQTT_lib.png)
>>![StringSplitter](../../assets/stringsplitter_lib.png)

## Code example

```arduino
#include <WiFi.h>
#include <MQTT.h>
#include <ArduinoJson.h>
// Install this library by going to menu Sketch > Include Libary > Manage Libraries...
// Search for StringSplitter and click Install.
#include "StringSplitter.h"

char ssid[] = "wifi network name";  // Replace with your network SSID
char pass[] = "wifi password";  // Replace with your network password
char host[] = "test.mosquitto.org"; // don't change this.
char topic[] = "inc-hmm";						// don't change this.
String client_id = "esp1";					// but please DO change this :)

WiFiClient wifiClient;
MQTTClient client;

#define RELAY_PIN 26

void setup()
{
	Serial.begin(115200);				// serial communication for debugging
	pinMode(RELAY_PIN, OUTPUT); // configure pin 26 as a output.
  digitalWrite(RELAY_PIN, HIGH); // The relays are 'inverted', the connection closes when the 
                                  // input is LOW. So we set it to HIGH on startup.

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
 */
void loop()
{
	client.loop();
	// check if connected to the MQTT relay server
	if (!client.connected())
	{
		Serial.println("lost connection");
		connectMqtt();
		delay(5000); // prevent flooding the server
	}
}

/*
 * The messageReceived function is called when we receive a message over MQTT.
 * Each message is formatted like this:
 * `origin:command:parameter`
 * `origin` is the sender, here we're looking for messages received from `server`.
 * `command` is the command to be executed, in this example it's the command `relay` we're waiting for.
 * `parameter` can contain extra information, here it can be `on` to turn the relay on and
 * anything else in the parameter will turn the led off.
 */
void messageReceived(String &topic, String &payload)
{
	Serial.println(topic + ": " + payload);

	StringSplitter *splitter = new StringSplitter(payload, ':', 3);
	int itemCount = splitter->getItemCount();

	if (itemCount == 3)
	{ // we have properly formatted command
		String origin = splitter->getItemAtIndex(0);
		String command = splitter->getItemAtIndex(1);
		String parameter = splitter->getItemAtIndex(2);

		if (origin == "server")
		{
			if (command == "relay")
			{
				if (parameter == "on")
				{
					digitalWrite(RELAY_PIN, LOW); // turn on the relay, by making the input LOW
					delay(3000);                  // wait for 3 seconds,
					digitalWrite(RELAY_PIN, HIGH); // and turn the relay off again.
					// Please note that the relay modules we are using have a active low input. 
					// This means that making the input pin LOW, will turn 'on' the relay.
				}
			}
		}
	}
}

/* ---------------------- you can ignore code below this line --------------------- */

/*
 * Wait for wifi connection
 */
void connectWifi()
{
	Serial.print("Connecting to wifi...");
	while (WiFi.status() != WL_CONNECTED)
	{
		Serial.print(".");
		delay(1000);
	}
	Serial.println("\nWifi connected!");
	delay(1000);
}

/*
 * Wait for MQTT connection and subscribe to the topic
 */
void connectMqtt()
{
	Serial.print("\nConnecting to MQTT server...");
	while (!client.connect(client_id.c_str()))
	{
		Serial.print(".");
		delay(1000);
	}
	Serial.println("\nMQTT connected!");
	delay(1000);
	client.subscribe(topic);
}

/*
 * Concat the command and parameter into a colon separated string
 * and publish the message
 */
void sendMessage(String message, String parameter)
{
	String msg = client_id + ":" + message + ":" + parameter;
	client.publish(topic, msg);
}

/*
 * Overloaded sendMessage function without the parameter
 */
void sendMessage(String message)
{
	sendMessage(message, "");
}
```
