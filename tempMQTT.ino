/****************************************
 * Include Libraries
 ****************************************/
#include <WiFi.h>
#include <PubSubClient.h>

//
// Interernal temperature Sensors & Blue LED
//
int ledPin = 5;
#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();

//
// WiFi
//
#define WIFISSID "ranchburger" // Put your WifiSSID here
#define PASSWORD "Trapezoidal#33*Pocahanta" // Put your wifi password here

//
// MQTT to ThingSpeak
//
const char* mqttServer = "mqtt.thingspeak.com";
const int mqttPort = 1883;
const char* mqttUser = "cerealinc";
const char* mqttPass = "3ND8E8M6N835SULE";
char writeAPIKey[] = "I1U9FSWRZUQ7TSRY";
long channelID = 706000;

WiFiClient client;
PubSubClient mqttClient(client);           // Initialize the PuBSubClient library.

static const char alphanum[] ="0123456789"
                              "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                              "abcdefghijklmnopqrstuvwxyz";  // For random generation of client ID.

//
// Timer Deep Sleep
//   Code Sample from: https://github.com/SensorsIot/ESP32-Deep-Sleep/blob/master/TimerWakeUp/TimerWakeUp.ino
//   Reference: https://github.com/espressif/esp-idf/blob/abea9e4/components/esp32/include/esp_sleep.h)
//
#define uS_TO_S_FACTOR 1000000  // Conversion factor for micro seconds to seconds 
#define TIME_TO_SLEEP  15        // Time ESP32 will go to sleep (in seconds) 
RTC_DATA_ATTR int bootCount = 0;  // Save bootcount in RTC memory

//
// Method to print the reason by which ESP32
// has been awaken from sleep
//
void print_wakeup_reason(){
  //esp_sleep_wakeup_cause_t wakeup_reason;
  esp_sleep_source_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_UNDEFINED : Serial.println("Wakeup caused by Undefined"); break;
    case ESP_SLEEP_WAKEUP_EXT0      : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1      : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER     : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD  : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP       : Serial.println("Wakeup caused by ULP program"); break;
    case ESP_SLEEP_WAKEUP_GPIO      : Serial.println("Wakeup caused by GPIO"); break;
    case ESP_SLEEP_WAKEUP_UART      : Serial.println("Wakeup caused by UART"); break;
    default : Serial.println("Wakeup was casued by ?????"); break;
  }
}


void setup() {

  //
  // Serial Monitor
  Serial.begin(115200);

  //
  // WiFi
  WiFi.begin(WIFISSID, PASSWORD);
  Serial.println();
  Serial.print("Wait for WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("Connected @ IP addr ");
  Serial.println(WiFi.localIP());

  //
  // Onboard Blue LED
  // pinMode(ledPin, OUTPUT);

  //Increment boot number and print it every reboot
  ++bootCount;
  // Serial.println("Boot number: " + String(bootCount));

  //Print the wakeup reason for ESP32
  // print_wakeup_reason();

  
 // Reconnect if MQTT client is not connected.
  if (!mqttClient.connected())
  {
    reconnect();
  }
  // Call the loop continuously to establish connection to the server.
  mqttClient.loop();   
  
  // Report Data
  mqttpublish();

  
  /*
  Next we decide what all peripherals to shut down/keep on
  By default, ESP32 will automatically power down the peripherals
  not needed by the wakeup source, but if you want to be a poweruser
  this is for you. Read in detail at the API docs
  http://esp-idf.readthedocs.io/en/latest/api-reference/system/deep_sleep.html
  Left the line commented as an example of how to configure peripherals.
  The line below turns off all RTC peripherals in deep sleep.
  */
  /* 
     esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
     Serial.println("Configured all RTC Peripherals to be powered down in sleep");
  */
  /*
  Now that we have setup a wake cause and if needed setup the
  peripherals state in deep sleep, we can now start going to
  deep sleep.
  In the case that no wake up sources were provided but deep
  sleep was started, it will sleep forever unless hardware
  reset occurs.
  */

  /*
  First we configure the wake up source
  We set our ESP32 to wake up every 5 seconds
  */
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  // Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
  //" Seconds");
  
  // Serial.print("Measuring Temperature: ");  
  // Convert raw temperature in F to Celsius degrees
  // Serial.print((temprature_sens_read() - 32) / 1.8);
  // Serial.println(" C");
  

  // Blink Blue LED
  // digitalWrite(ledPin, HIGH);
  // delay(500);
  // digitalWrite(ledPin, LOW);
  // delay(500);
  
  Serial.println("Going to sleep now");
  esp_deep_sleep_start();

  // Nothing else following will get executed
  Serial.println("This should never be printed");

}


void loop() {
  
  // The Arduino main loop won't get executed in Deep Sleep mode of operation

}

void reconnect()
{
  char clientID[9];

  // Loop until reconnected.
  while (!mqttClient.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Generate ClientID
    for (int i = 0; i < 8; i++) {
        clientID[i] = alphanum[random(51)];
    }
    clientID[8]='\0';
    mqttClient.setServer(mqttServer, mqttPort);
    
    // Connect to the MQTT broker
    if (mqttClient.connect(clientID,mqttUser,mqttPass))
    {
      Serial.print("Connected with Client ID:  ");
      Serial.print(String(clientID));
      Serial.print(", Username: ");
      Serial.print(mqttUser);
      Serial.print(" , Passwword: ");
      Serial.println(mqttPass);
    } else
    {
      Serial.print("failed, rc=");
      // Print to know why the connection failed.
      // See https://pubsubclient.knolleary.net/api.html#state for the failure code explanation.
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
void mqttpublish() {

//  float t = dht.readTemperature(true); // Read temperature from DHT sensor.
//  float h = dht.readHumidity();  // Read humidity from DHT sensor.
//  int lightLevel = analogRead(LIGHTPIN); // Read from light sensor

// Create data string to send to ThingSpeak
//  String data = String("field1=" + String(t, DEC) + "&field2=" + String(h, DEC) + "&field3=" + String(lightLevel, DEC));
  int fakeCount;
  fakeCount = 32 * (sin((float) bootCount/25) + random(-0.5, 0.5)) + 60;
  String data = String("field1=" + String(fakeCount, DEC));
  int length = data.length();
  char msgBuffer[length];
  data.toCharArray(msgBuffer,length+1);
  Serial.println(msgBuffer);

  // Create a topic string and publish data to ThingSpeak channel feed.
  String topicString ="channels/" + String( channelID ) + "/publish/"+String(writeAPIKey);
  length=topicString.length();
  char topicBuffer[length];
  topicString.toCharArray(topicBuffer,length+1);

  mqttClient.publish( topicBuffer, msgBuffer );

}
