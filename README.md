# rtl_433_ESP (fork)

This repository is a fork of [NorthernMan54/rtl_433_ESP](https://github.com/NorthernMan54/rtl_433_ESP). Many thanks to NorthernMan54 — I made changes to integrate the project with Home Assistant via MQTT.

<img width="1555" height="788" alt="image" src="https://github.com/user-attachments/assets/350a90cf-220f-4fb6-bd5d-0fe1ff313e61" />

## Purpose

This fork decodes messages from a Vevor 7‑in‑1 weather station purchased from AliExpress:
https://www.aliexpress.com/item/1005007557396913.html

The Vevor station uses an FSK protocol. This repository includes a PlatformIO environment configured for FSK decoding on TTGO/LilyGO LoRa32 boards.

## Tested boards

- LILYGO® LoRa32 ESP32 @ 868/915 MHz
- LILYGO® LoRa32 ESP32 V2.1_1.6 @ 868/915 MHz (ttgo-lora32-v21)

## Requirements

- Visual Studio Code (or another code editor)
- PlatformIO extension for VS Code
- GitHub Repositories extension for VS Code
- Git: https://git-scm.com/install/windows
- A supported TTGO/LilyGO LoRa32 board
- Board drivers: https://www.wch.cn/downloads/CH343SER_ZIP.html or https://www.silabs.com/software-and-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads
- The Vevor weather station 7 in 1 (or other compatible RF sensors)

## Getting started

1. Clone this repository:
   git clone https://github.com/nabrog87/rtl_433_ESP

2. Open Visual Studio Code and install/enable the PlatformIO and GitHub Repositories extensions.

3. Open the PlatformIO project at `example/OOK_Receiver/platformio.ini`.

4. Edit `example/OOK_Receiver/Include/secrets.h` with your Wi‑Fi and MQTT details.

Example `secrets.h`:
```c
#define WIFI_SSID     "YOUR_WIFI_SSID"
#define WIFI_PASS     "YOUR_WIFI_PASS"

#define MQTT_HOST     "192.168.1.10"
#define MQTT_PORT     1883
#define MQTT_USER     "mqtt_user"
#define MQTT_PASS     "mqtt_password"

#define MQTT_TOPIC_OK  "rtl_433/vevor"         // MQTT topic for parsed messages
#define MQTT_TOPIC_RAW "rtl_433/vevor/raw"     // MQTT topic for raw messages
#define MQTT_TOPIC_STAT "rtl_433/vevor/status" // MQTT connection/status topic
```

## PlatformIO environment for FSK (Vevor)

The Vevor weather station uses FSK modulation. I added a PlatformIO environment named `lilygo_lora32_v21new_868_fsk` configured for FSK decoding:

```ini
[env:lilygo_lora32_v21new_868_fsk]
platform = espressif32
framework = arduino
board = ttgo-lora32-v21
monitor_speed = 115200

build_flags =
  -DRF_MODULE_FREQUENCY=868.35      ; I received more messages on 868.35 rather than 868.30
  -DOOK_MODULATION=false            ; False = FSK, True = OOK
  -DRF_MODULE_INIT_STATUS=true
  -DRAW_SIGNAL_DEBUG=true
  -DRTL_DEBUG=1
; -DPUBLISH_UNPARSED=true           ; Enable to publish unparsed/raw frames for debugging
```

I had the best reception at 868.35 MHz for my station. You can try other common frequencies by changing `-DRF_MODULE_FREQUENCY`:

```ini
-DRF_MODULE_FREQUENCY=433.92   ; 433 MHz (default)
-DRF_MODULE_FREQUENCY=868.30   ; 868 MHz (common for EU sensors)
-DRF_MODULE_FREQUENCY=915.00   ; 915 MHz (US ISM band)
```

Also update the frequency constant in `example/OOK_Receiver/OOK_Receiver.ino` .

```ini
   #define RF_MODULE_FREQUENCY 868.35
```
## Build and upload

1. In PlatformIO, select the `lilygo_lora32_v21new_868_fsk` environment (or another environment that matches your board).
2. Build the firmware.
3. Upload the firmware to the board.
4. Open the serial monitor at 115200 baud to view logs and decoded messages.

Note: For initial testing, keep the receiver within ~10 meters of the weather station.

## MQTT

After the device connects to Wi‑Fi and your MQTT broker, parsed sensor messages will be published to the MQTT topic(s) configured in `secrets.h` (for example, `rtl_433/vevor`). You can inspect messages using tools such as [MQTT Explorer](https://mqtt-explorer.com/).

<img width="1517" height="569" alt="image" src="https://github.com/user-attachments/assets/38b7b110-6e33-449c-b34c-02bc3be9ec72" />

## Home Assistant

I have included the configuration.yaml sensors and the dashboard script, with dynamic icons and colors, based on the state of sensors.

To make this dashboard to work you need to install a few components
 - HACS
 - Wind Rose Card via HACS
 - apexcharts-card via HACS
 - layout-card via HACS
 - Mushroom via HACS
 - card-mod via HACS

## Tips and troubleshooting

- If you see few or no messages, try adjusting the RF frequency slightly (for example, 868.30, 868.35, 868.40).
- Ensure `-DOOK_MODULATION=false` is set for FSK decoding.
- Test the board close to the weather station when first verifying decoding.
- Enable `-DPUBLISH_UNPARSED=true` to publish all received frames (helpful for debugging unknown sensors).
- Use the serial monitor to check status and debug output.

## Acknowledgements

This project is based on the work of [NorthernMan54](https://github.com/NorthernMan54/rtl_433_ESP). Thank you for the original project.

## Links

- Original project: https://github.com/NorthernMan54/rtl_433_ESP
- My fork: https://github.com/nabrog87/rtl_433_ESP
