# ESP_Starter_Code
ESP32 and ESP8266 initial code base for starting a solution.

**Sets up the following base configuration:**
* Wifi access through WifiManager (hotspot will be started if cannot connect to Wifi)
* Set up OTA for future updates
* Set up mDNS to be able to find the device on the network


**Configuration:**

```c++
// Device Info
const char* devicename = "OnAir";
const char* devicepassword = "onairadmin";
```

The `devicename` is the name that will be used as the hostname for the device. It is also used for the Access Point SSID for configuring access to the Wifi network. The device name is also used for the multicast DNS host and the OTA hostname.

The `devicepassword` is used for the Access Point SSID when connecting to configure access to the Wifi network. It is also the password used for uploading an OTA file to the device.
