
This is a fork from https://github.com/NorthernMan54/rtl_433_ESP  
Many thanks to NorthernMan54.

I have made some changes/additions in order to integrate it into Home Assistant via MQTT

Boards that i have tested:

```ini
`  LILYGO® LoRa32 ESP32 @868/915Mhz `
`  LILYGO® LoRa32 ESP32 V2.1_1.6 @868/915Mhz `
```
Purpouse of this is to decode messages from a Vevor 7 in 1 weather station, that i bought from Aliexpress

https://www.aliexpress.com/item/1005007557396913.html?spm=a2g0o.order_list.order_list_main.5.65871802itfnq7


<img width="1555" height="788" alt="image" src="https://github.com/user-attachments/assets/350a90cf-220f-4fb6-bd5d-0fe1ff313e61" />


Instructions for building and uploading the images to Lilygo board:

First of all install Visual Studio, python and git on you computer.

On Visual Studio install PlatformIO clone the repository to your local drive
git clone https://github.com/nabrog87/rtl_433_ESP

Open the platformio.ini from   
YOUT_LOCAL_REPO_LOCATION/example/OOK_Receiver/platformio.ini

Modify example/OOK_Receiver/Include/secrets.h with your WIFI/MQTT server details

```ini
' #define WIFI_SSID     "YOUR_WIFI_SSID" '
' #define WIFI_PASS     "YOUR_WIFI_PASS" '

' #define MQTT_HOST     "192.168.1.10" '
' #define MQTT_PORT     1883 '
' #define MQTT_USER     "mqtt_user" ' 
' #define MQTT_PASS     "mqtt_password-" '

' #define MQTT_TOPIC_OK  "rtl_433/vevor"  '        ; MQTT Topic
' #define MQTT_TOPIC_RAW "rtl_433/vevor/raw" '     
' #define MQTT_TOPIC_STAT "rtl_433/vevor/status" '  ; Mqtt connection status
```

As mentioned in https://github.com/NorthernMan54/rtl_433_ESP?tab=readme-ov-file
the Vevor weather station needs a FSK signal device decoder 
So i've created a new enviroment in platformin.ini called lilygo_lora32_v21new_868_fsk

```ini
' Registering protocol [104] "Vevor Wireless Weather Station 7-in-1" ' 
```

```ini
[env:lilygo_lora32_v21new_868_fsk]
platform = espressif32
framework = arduino
board = ttgo-lora32-v21
monitor_speed = 115200

build_flags =
  '-DRF_MODULE_FREQUENCY=868.35'  ;  I received more messages on 868.35 rather than 868.3
; '-DPUBLISH_UNPARSED=true'
  '-DOOK_MODULATION=false'    ; False is FSK, True is OOK
  '-DRF_MODULE_INIT_STATUS=true'
  '-DRAW_SIGNAL_DEBUG=true'
  '-DRTL_DEBUG=1'
```
Here you can change the frequency. I have tested many frequencies and the most messages i received were on 868.35
Also i have changed the frequency in example/OOK_Receiver/OOK_Receiver.ino to 868.35

```ini
'-#  define RF_MODULE_FREQUENCY 868.35 '-
```
```ini
'-DRF_MODULE_FREQUENCY=433.92'  ; 433 MHz (default)
'-DRF_MODULE_FREQUENCY=868.30'  ; 868 MHz (common for EU sensors)
'-DRF_MODULE_FREQUENCY=915.00'  ; 915 MHz (US ISM band)
```

Build the image and upload it to the board.

Use the serial monitor from PlatformIo to check the received messages. Please do this when you are in a range of max 10 meters from the weather station.



Also you can check the decoded messages from mqtt (using MQTT Explorer);

<img width="1517" height="569" alt="image" src="https://github.com/user-attachments/assets/38b7b110-6e33-449c-b34c-02bc3be9ec72" />


