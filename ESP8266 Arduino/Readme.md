Arduino ESP8266 - Smart Bulb
===================


This example demonstrates the use of IOStash MQTT interface with Arduino running on ESP8266 by implementing a simple connected light bulb that can be controlled via the internet.

 **Install Required Libraries **


      - Arduino Pub/sub Client
      - ArduinoJSON

 
 **How To Run**

Setup the example according to the tutorial on http://iostash.com.

**Notes**

Make sure that you change the `MQTT_MAX_PACKET_SIZE` constant in PubSubClient.h of the PubSubClient library to `256` so that the whole message packet is received. 

For support or assistance, drop an email to support@iostash.com