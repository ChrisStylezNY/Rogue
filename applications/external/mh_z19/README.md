# flipper-zero-mh-z19
Application for measuring carbon dioxide (CO2) with mh-z19 sensor and Flipper zero

![Ok ppm](assets/Ok.png)
![Very bad ppm](assets/Very.png)
![Setup](assets/setup.png)



## Instalation Guide
- Clone application source code
```bash
git clone https://github.com/meshchaninov/flipper-zero-mh-z19.git
```
- Clone flipper zero source code
```bash
git clone --recursive https://github.com/flipperdevices/flipperzero-firmware.git
```
- Move mh_z19 folder to flipperzero-firmware/application_user folder
```bash
mkdir -p flipperzero-firmware/application_user/mh_z19 && mv flipper-zero-mh-z19/* flipperzero-firmware/application_user/mh_z19
```
- Connect flipper zero via USB to your computer
- Build and flash to device firmware
```bash
cd flipperzero-firmware
./fbt
./fbt flash_usb
```

## How to use
- Connect MH-Z19 sensor to your flipper zero:
```
sensor <-> flipper zero
5V <-> 5V (1 pin)
GND <-> GND (8 pin)
PWM <-> A6 (3 pin)
```
![setup](assets/setup2.jpeg)
- Open in flipper zero:
Applications -> GPIO -> [MH-Z19] CO2 sensor
