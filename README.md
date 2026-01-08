
This is a fork from https://github.com/NorthernMan54/rtl_433_ESP  
Many thanks to NorthernMan54.

I have made some changes/additions in order to integrate it into Home Assistant via MQtt

Boards that i have tested:
  LILYGO® LoRa32 ESP32 @868/915Mhz
  LILYGO® LoRa32 ESP32 V2.1_1.6 @868/915Mhz

Purpouse of this is to decode messages from a Vevor 1 in 1 weather station, taht i bought from Aliexpress

https://www.aliexpress.com/item/1005007557396913.html?spm=a2g0o.order_list.order_list_main.5.65871802itfnq7


<img width="1555" height="788" alt="image" src="https://github.com/user-attachments/assets/350a90cf-220f-4fb6-bd5d-0fe1ff313e61" />


Instructions for building and uploading the images to Lilygo

First of all install Visual Studio, python and git on you computer
On Visual Studio install PlatformIO

Clone the repository on yor computer
git clone https://github.com/nabrog87/rtl_433_ESP

Open the platformio.ini from   
YOUT_LOCAL_REPO_LOCATION/example/OOK_Receiver/platformio.ini

Modify example/OOK_Receiver/Include/secrets.h with your WIFI/MQTT server details

```ini
#define WIFI_SSID     "YOUR_WIFI_SSID"
#define WIFI_PASS     "YOUR_WIFI_PASS"

#define MQTT_HOST     "192.168.1.10"
#define MQTT_PORT     1883
#define MQTT_USER     "mqtt_user"
#define MQTT_PASS     "mqtt_password-"

#define MQTT_TOPIC_OK  "rtl_433/vevor"
#define MQTT_TOPIC_RAW "rtl_433/vevor/raw"
#define MQTT_TOPIC_STAT "rtl_433/vevor/status"
```



```ini
'-DRF_MODULE_FREQUENCY=433.92'  ; 433 MHz (default)
'-DRF_MODULE_FREQUENCY=868.30'  ; 868 MHz (common for EU sensors)
'-DRF_MODULE_FREQUENCY=915.00'  ; 915 MHz (US ISM band)
```
