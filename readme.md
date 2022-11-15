# Emoji Proxies & Ghost Messengers

## Introduction

How can online visitors of cultural events feel seen, become more involved in or even have agency over what happens on-site? That is the central question during this workshop. To facilitate experimentation during this workshop we’ve developed a few prototypes or starting points. Each prototype uses an ESP32 module to communicate with the The Hmm live streaming platform. ESP32 is a hardware module that makes it relatively easy to connect buttons, lights and other electronics to a website. The diagram below illustrates how the module communicates with the live streaming platform. 

![communication diagram](assets/flow.png)

Both the live streaming server and the ESP32 module are connected over the MQTT protocol with an open MQTT server, and are able to send and receive messages. The online audience can send messages in the chat that will be forwarded to the ESP32 module and the ESP32 can do the reverse. 

### Examples 

We’ve developed a few examples that might function as a starting point for experimentation. Each example has more detailed instructions inside. 

Please change the client_id in the sketch to something unique, so that each device has its own id.

1.	[Led](Code/MQTT_led/) - A led is connected to the ESP32 module. When the online audience types a specific message in the chat the led will light up. So the interaction in this example is from online audience to the onsite audience. 
2.	[Button](Code/MQTT_button/)  - In this first example we connect a button to the module. When the button is pressed a message is sent to the live streaming server. If the server receives this message it will show a specific emote. This allows the physical audience to interact with the audience on the live streaming platform. 
3.	[Neopixel](Code/MQTT_neopixel/)  - This example is more elaborate variation on the first example. A specifically crafted message on the live streaming platform can set the color of a led connected to the ESP32.
4.	[Relay](Code/MQTT_relay/)  - By connecting a relay (an electrically operated switch) we can turn on (and off) almost any device. Here we’ve connected a wacky flailing inflatable tube man. 


### Message format

In the code the messageReceived function is called when we receive a message over MQTT.
Each message is formatted like this: `origin:command:parameter`

* `origin` is the 'name' of the sender, this will be 'server' if the message originates from the server, otherwise the name that is set in the `client_id` string in the sketch.
* `command` is the command to be executed. You can find per example commands in their respective .ino files.

* `parameter` can contain extra information, for example 'on' to turn a led on, or a color for the led strip example.

The examples contain code to split the message string into its constituent parts.

### Livestream link? 

### Breadboard

To build our prototypes we will be using a solderless breadboard. This is a little plastic board with holes in it. These holes let you easily insert electronic components to quickly test an electronic circuit without having to solder. There's more information on breadboards and how to use them on the links below the image.


![A breadboard](https://upload.wikimedia.org/wikipedia/commons/7/73/400_points_breadboard.jpg)

* https://www.sciencebuddies.org/science-fair-projects/references/how-to-use-a-breadboard
* https://learn.adafruit.com/lesson-0-getting-started/breadboard


## Installation

In this workshop we'll use a few pre-prepared computers, but if you'd like to continue on your own machine follow the steps below.

1. Download and install the Arduino IDE
2. Install the ESP32 board
3. Install the drivers
4. Install the needed libraries

We've documented the installation process on the [Hackers & Designers wiki](https://wiki.hackersanddesigners.nl/index.php?title=ESP32_Arduino_Setup).
Follow the steps on the linked page, but instead of installing the libraries mentioned in the article, install the libraries we need for the examples. To do that **`REPLACE STEP 5`** with these instructions:

1. Open the library manager by going to the menu `Sketch` > `Include Library` > `Manage Libraries...`
2. In the search field type: MQTT, scroll down until you find a library simply called MQTT by Joel Gaehwiller. Click in the install button and wait for the download to finish. Do not close the library manager.
3. Install `ArduinoJson`, `Adafruit Neopixel` and `StringSplitter` in the same way as the previous step.
5. Now continue with step 6 in the article, but replace the code with this:
```c
#include <MQTT.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include <StringSplitter.h>

void setup() {
}

void loop() {
}  
```
Press the verify button in the IDE (check mark, top left). If the code compiles without errors, you're all set.
