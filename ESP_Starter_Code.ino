// On Air Display control
// Light up the On Air sign using a strip of 10 WS2812s
// Button controls if sign is on or off
// Webpage also available to control sign

#include <WiFiManager.h>        // For managing the Wifi Connection
#include <ESP8266mDNS.h>        // For running OTA and Web Server
#include <WiFiUdp.h>            // For running OTA
#include <ArduinoOTA.h>         // For running OTA
#include <TelnetSerial.h>       // For debugging via Telnet


// Device Info
const char* devicename = "initialESP";
const char* devicepassword = "initialadmin";

//for using LED as a startup status indicator
#include <Ticker.h>
Ticker ticker;
boolean ledState = LOW;   // Used for blinking LEDs when WifiManager in Connecting and Configuring

// On board LED used to show status
#ifndef LED_BUILTIN
#define LED_BUILTIN 13 // ESP32 DOES NOT DEFINE LED_BUILTIN
#endif
const int ledPin =  LED_BUILTIN;  // the number of the LED pin

// Telnet Serial variables
TelnetSerial telnetSerial;  // Manage Telnet connection to receive Serial data
Stream *usbSerial;          // Pointer to USB/Hardware Serial for fallback debugging

// Used for running a simple program during loop to blink led
// replace with real variables
unsigned long previousMillis = 0;     // will store last time LED was updated
const long interval = 2000;           // interval at which to blink (milliseconds)


/*************************************************
 * Setup
 *************************************************/
void setup() {
  Serial.begin(115200);
  Serial.println(ESP.getFullVersion());

  // set the digital pin as output:
  pinMode(ledPin, OUTPUT);

  // start ticker to slow blink LED strip during Setup
  ticker.attach(0.6, tick);


  //
  // Set up the Wifi Connection
  //
  WiFi.hostname(devicename);
  WiFi.mode(WIFI_STA);      // explicitly set mode, esp defaults to STA+AP
  WiFiManager wm;
  // wm.resetSettings();    // reset settings - for testing
  wm.setAPCallback(configModeCallback); //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  //if it does not connect it starts an access point with the specified name here  "AutoConnectAP"
  if (!wm.autoConnect(devicename,devicepassword)) {
    //Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(1000);
  }
  //Serial.println("connected");


  //
  // Set up the Multicast DNS
  //
  MDNS.begin(devicename);


  //
  // Set up OTA
  //
  // ArduinoOTA.setPort(8266);
  ArduinoOTA.setHostname(devicename);
  ArduinoOTA.setPassword(devicepassword);
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }
    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    //Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    //Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    //Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    //Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      //Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      //Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      //Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      //Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      //Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();

  // Setup Telnet Serial
  telnetSerial.begin(115200);
  usbSerial = telnetSerial.getOriginalSerial();

  // Let USB/Hardware Serial know where to connect.
  usbSerial->print("Ready! Use 'telnet ");
  usbSerial->print(WiFi.localIP());
  usbSerial->printf(" %d' to connect\n", TELNETSERIAL_DEFAULT_PORT);


  //
  // Done with Setup
  //
  ticker.detach();          // Stop blinking the LED
}


/*************************************************
 * Loop
 *************************************************/
void loop() {
  // Handle any requests
  ArduinoOTA.handle();
  MDNS.update();
  telnetSerial.handle();

  //************************************************
  // Blinking LED code to be replaced with real code
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa
    if (ledState == LOW) {
      ledState = HIGH;
      usbSerial->println("usbSerial HIGH");
      Serial.println("serial HIGH");
    } else {
      ledState = LOW;
      usbSerial->println("usbSerial LOW");
      Serial.println("serial LOW");
    }

    // set the LED with the ledState variable
    digitalWrite(ledPin, ledState);
  }
  //************************************************

}


/*************************************************
 * Callback Utilities during setup
 *************************************************/
 
/*
 * Blink the LED Strip.
 * If on  then turn off
 * If off then turn on
 */
void tick()
{
  //toggle state
  digitalWrite(ledPin, !digitalRead(ledPin));     // set pin to the opposite state
}

/*
 * gets called when WiFiManager enters configuration mode
 */
void configModeCallback (WiFiManager *myWiFiManager) {
  //Serial.println("Entered config mode");
  //Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  //Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);
}
