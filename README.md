
Mmny thanks, this is a fork of https://github.com/NorthernMan54/rtl_433_ESP

I have made some changes/additions in order to integrate it into Home Assistant/MQtt

Boards thai i have tested
LILYGO® LoRa32 ESP32 @868/915Mhz
LILYGO® LoRa32 ESP32 V2.1_1.6 @868/915Mhz



```ini
'-DRF_MODULE_FREQUENCY=433.92'  ; 433 MHz (default)
'-DRF_MODULE_FREQUENCY=868.30'  ; 868 MHz (common for EU sensors)
'-DRF_MODULE_FREQUENCY=915.00'  ; 915 MHz (US ISM band)
```
