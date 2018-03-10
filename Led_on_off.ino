/***************************************************
  Simple Aurdino-Node MCU based code to control the on-off period of led along with its intensity
  The on-off can be implemented through adafruit io as well as google assisstant on ifttt
  The intensity of the led can be controlled through a slider on adafruit which is read and output brightness of led is controlled
 ****************************************************/
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "HotPot"
#define WLAN_PASS       "idiotsaroundme"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "srishti19"
#define AIO_KEY         "27934646586d4844a61cb249a4b14f00"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

//Adafruit_MQTT_Publish photocell = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/photocell");.
Adafruit_MQTT_Subscribe Light = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Light");
Adafruit_MQTT_Subscribe Fade = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Fade");
Adafruit_MQTT_Subscribe FadeC = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/FadeC");


/*************************** Sketch Code ************************************/

void MQTT_connect();

int p=16; //this is the pin where led is connected
void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(p,OUTPUT);
  //pinMode(D1,OUTPUT);
  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
 mqtt.subscribe(&Light);
  mqtt.subscribe(&Fade);
  mqtt.subscribe(&FadeC);
}

uint32_t x=0;

void loop() {
  
  // Ensure the connection to the MQTT server is alive (this will make the first)
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here
  Adafruit_MQTT_Subscribe *subscription;
  
  while ((subscription = mqtt.readSubscription(5000))) 
  {
    if (subscription == &Light) //only for on off
    {
      Serial.print(F("Got_Light: "));
      Serial.println((char *)Light.lastread);

      if(((char *)Light.lastread)[1]=='F')
      {
        digitalWrite(p,LOW);
        delay(500);
      }
      if(((char *)Light.lastread)[1]=='N')
      {
        digitalWrite(p,HIGH);
        delay(500);
      
    }
      }

      if(subscription==&FadeC)//Fadecontrol on off
      {
        Serial.print(F("Got_FadeC: "));
        Serial.println((char *)FadeC.lastread);
        if(((char *)FadeC.lastread)[1]=='N')
      {
        for(int i=0;i<=255;i=i+25)
        {
           
          analogWrite(D1,i );
          delay(1000);
        }
        }
        if(((char *)FadeC.lastread)[1]=='F')
      {
        for(int i=255;i>=0;i=i-25)
        {
           
          analogWrite(D1,i );
         delay(1000);
        }
        }
        
       
      }
    //this portion of code controls the intensoty of light
    //the slider value from adafruit is read and fed to Led
     if(subscription==&Fade)
    {
      Serial.print(F("Got_Fade: "));
      Serial.println((char *)Fade.lastread);
       uint16_t sliderval = atoi((char *)Fade.lastread);
      analogWrite(D1,sliderval );
    }
  }

//  // Now we can publish stuff!
//  Serial.print(F("\nSending photocell val "));
//  Serial.print(x);
//  Serial.print("...");
//  if (! photocell.publish(x++)) {
//    Serial.println(F("Failed"));
//  } else {
//    Serial.println(F("OK!"));
//  }
  /*
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  */
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
