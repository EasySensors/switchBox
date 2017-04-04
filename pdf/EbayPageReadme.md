

### The Switch Node is a low cost wireless Arduino IDE compatible (the Atmel ATMega328P) microcontroller with HopeRF RFM69-CW 433 MHz radio on board and build in hardware debouncer. Best sutable for Home Automation, IOT. Could be used as switch board for radio controlling any DIY project.

## Features
- Fully compatible with the Arduino IDE (PRO MINI 8 MG Hz)
- Over the Air, firmware updates (OTA) ready. In combination with preprogrammed Dual Optiboot boot loader and  on board External JDEC  EPROM\Flash memory the Node allows OTA software updates.
- Onboard RFM69-CW 433 MHz Radio Transceiver  with wire antenna
- Onboard Atmel ATSHA204A CryptoAuthentication chip makes communication with the Node very secured. ATSHA204A implements secured handshake and if handshake fails the Node will not executing any command sent to it
- Programming header is FTDI, TTL Serial. For programming you need to have  USB TTL Serial – FTDI adaptor
- ENIG Emersion Gold finish
- MySensors compatible. You can use superb set of scripts and libraries from [mysensors.org](http://www.mysensors.org) project  and convert the nodes into reliable network of little home sensors, relays and actuators.
- Supports both ON/OFF and momentary switches 
- Battery connector CR2450
- Yes, it is open sourced. Completely.

Comes with Arduino example sketch.

## Overviev
If you are familaiar with Arduino boards, the Switch Node is basically Arduino pro Mini with onboard HopeRF RFM69-CW 433 MHz radio, external EPROM flash and cryptoauthentication chip. To save some board dimensions The Switch Node does not have onboard USB-Serial converter. You need to buy FTDI Adapter for programming. HopeRF RFM69-CW provide extremly good range. Hundreds of meters in open areas. Exteranal Flash chip alows to burn arduino sketch wirelessly. Onboard Atmel ATSHA204A CryptoAuthentication chip secures communacation with other controllers. Some IOT ideas based on the switch node and similar controllers and complete specs you may find here [my GitHub page](https://github.com/EasySensors/SwitchNode)

![enter image description here](https://github.com/EasySensors/SwitchNode/blob/master/pics/sw1.jpg?raw=true)
![enter image description here](https://github.com/EasySensors/SwitchNode/blob/master/pics/sw2.jpg?raw=true)
![enter image description here](https://github.com/EasySensors/SwitchNode/blob/master/pics/sw3.jpg?raw=true)

Package Content:
-	The Controller board 1 pcs  
-	JST Connectros 5 pcs  
- No Battery
