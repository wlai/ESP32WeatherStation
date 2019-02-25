# ESP32WeatherStation
IoT Weather station based on Espressif ESP32 microcontrollers. Leverages its deep sleep capability to save power. Logs temp/atmospheric pressure to a MQTT Broker. 

The current target is "Sparkfun ESP32 Thing" https://www.sparkfun.com/products/13907

The libaries needed are: 
- WiFi https://www.arduino.cc/en/Reference/WiFi 
- PubSubClient https://pubsubclient.knolleary.net/

The MQTT Broker used is https://thingspeak.com/

You'll need to provide your own PrivateKey.h in the same directory, with the followin #defines

#define WIFISSID "mySSID"                           // Put your Wifi SSID here
#define PASSWORD "mySSIDPassword"                   // Put your wifi password here
#define MQTTUSER "ThingSpeakUserID"
#define MQTTPASS "ThingSpeakMQTTAPIKey"
#define MQTTAPIKEY "ThingSpeakWriteAPIKeyForChannel"
#define MQTTCHANNELID NNNNNN                        // Six digit channel number you created on Thingspeak


