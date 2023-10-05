#include <WiFi.h>
#include <MQTT.h>
#include <ArduinoJson.h>
// Install this library by going to menu Sketch > Include Libary > Manage Libraries...
// Search for StringSplitter and click Install.
#include "StringSplitter.h"
// Install this library by going to menu Sketch > Include Libary > Manage Libraries...
// Search for Adafruit Neopixel and click Install.
#include <Adafruit_NeoPixel.h>

char ssid[] = "wifi network name";	// Replace with your network SSID
char pass[] = "wifi password";			// Replace with your network password
char host[] = "test.mosquitto.org"; // don't change this.
char topic[] = "inc-hmm";						// don't change this.
String client_id = "esp1";					// but please DO change this :)

WiFiClient wifiClient;
MQTTClient client;

#define LED_PIN 26
#define NUMPIXELS 8 // change this to the number of pixels on the strip

Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
	Serial.begin(115200);			// serial communication for debugging
	pinMode(LED_PIN, OUTPUT); // configure pin 26 as a output.

	// start wifi and mqtt
	WiFi.begin(ssid, pass);
	client.begin(host, wifiClient);
	client.onMessage(messageReceived); // call the messageReceived function when a message is received
																		 // connect wifi and mqtt
	connectWifi();
	connectMqtt();
	// say hi
	sendMessage("connect");
	// initialize NeoPixel strip object
	pixels.begin();
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
* `command` is the command to be executed, in this example it's the command `led` we're waiting for.
* `parameter` can contain extra information, here its used to send the color for the strip
* 
* Here we're looking for a message in the format `server:rgb:#nnnnnn`, 
* where the #nnnnnn part is a hexidecimal color code (see the readme)
* If we receive this message and it is formatted properly, color all 
* the pixels of the ledstrip to this color.
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
			if (command == "rgb")
			{
				Serial.println(parameter.length());
				if (parameter.length() == 7 && parameter.startsWith("#"))
				{
					// get the R, G and B parts of the string
					String r = parameter.substring(1, 3);
					String g = parameter.substring(3, 5);
					String b = parameter.substring(5, 7);
					Serial.print(r);
					Serial.print(" ");
					Serial.print(g);
					Serial.print(" ");
					Serial.println(b);

					// convert the hex values to decimal
					int rr = StrToHex(r.c_str());
					int gg = StrToHex(g.c_str());
					int bb = StrToHex(b.c_str());

					// then update the pixel colors
					for (int i = 0; i < NUMPIXELS; i++) { 
						// pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
						pixels.setPixelColor(i, pixels.Color(rr, gg, bb));
						pixels.show(); // Send the updated pixel colors to the hardware.
						delay(100);
					}
				}
			}
		}
	}
}

/*
* This function converts a hexademical number (base 16) into
* a decimal number. 
*/
int StrToHex(const char str[])
{
	return (int)strtol(str, 0, 16);
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
