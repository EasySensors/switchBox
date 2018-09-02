![arduino Switch Box](https://github.com/EasySensors/switchBox/blob/master/pics/arduino-Switch-Box-2cr.jpg?raw=true)
![arduino Switch Box](https://github.com/EasySensors/switchBox/blob/master/pics/arduino-Switch-Box-21cr.jpg?raw=true)
![arduino Switch Box](https://github.com/EasySensors/switchBox/blob/master/pics/arduino-Switch-Box-22cr.jpg?raw=true)

### The Switch Box (IS6DB2032) is a low cost wireless Arduino IDE compatible (the Atmel ATMega328P) microcontroller with HopeRF RFM69 868\915 or NRF24L01+ 2.4 GHz radio on-board. Best sutable for Home Automation, IOT. Could be used as switch board for radio controlling any DIY project. You may think of it as Arduino Pro Mini plus all the items in the picture below:

![](https://github.com/EasySensors/switchBox/blob/master/pics/replace2.jpg?raw=true)


## Specification: ##
 - Dimensions mm
 - Sleep current consumption 5uA
 - Authentication security - Atmel ATSHA204A Crypto Authentication Chip
 - RFM69-CW (low power consumption version) 868\915 MHz Radio transceiver
 - Footprint available for replacing  RFM69-CW with NRF24L01+ 2.4 GHz radio
 - Battery voltage sensor (via divider)
 - FTDI header for programming
 - Battery connectors: 2 x CR2032
 - Reverse polarity protection
 - Can be 1 or 2 momentary switches versions
 - Each button have LED for message delivery confirmation
 - Reset switch

If you find the switch box useful, it is possible to buy it here: [link to buy](https://www.tindie.com/products/easySensors/arduino-ide-compatible-wireless-switch-box)

**Pin out:** 

Arduino Pins|	Description
------------|--------------
A6 |	Connected to Battery voltage sensor (via divider)
A3 |	Connected to  ATSHA204A
D4, D8, A0 | Connected to momentary switch buttons
Interrupt 1 | Occurs when either button pressed
D5, D6, D7 | Connected to LED's



**Arduino IDE Settings**

![Arduino IDE Settings](https://github.com/EasySensors/ButtonSizeNode/blob/master/pics/IDEsettings.jpg?raw=true)




**programming FTDI adapter connection**

![FTDI Power](https://github.com/EasySensors/SwitchNode/blob/master/pics/FTDIvcc3.jpg?raw=true)


3.3V power option should be used.



How to use it as home automation (IOT) node controller
------------------------------------------------------


switcBox.ino is the Arduino example sketch using [MySensors](https://www.mysensors.org/) API. 

Burn the switcBox.ino sketch into it an it will became  one of the MySensors home automation network Node. 
To create the network you need controller and at least two Nodes one as a Sensor, relay or switch Node and the other one as 
“Gateway Serial”. I personally love [Domoticz](https://domoticz.com/) as conroller. Please check this [HowTo](https://github.com/EasySensors/ButtonSizeNode/blob/master/DomoticzInstallMySensors.md) to install Domoticz.

However, for no-controller setup, as example, you can use 3 nodes - first node as “Gateway Serial”, second node as relay and last one as switch for that relay. No controller needed then, keep the switch and the relay on the same address and the switch will operate the  relay.

Things worth mentioning about the  [MySensors](https://www.mysensors.org/) Arduino sketch: 


Code |	Description
------------|--------------
#define MY_RADIO_RFM69<br>#define MY_RFM69_FREQUENCY   RF69_433MHZ<br>#define MY_IS_RFM69HW|	Define which radio we use – here is RFM 69<br>with frequency 433 MHZ and it is HW<br>type – one of the most powerful RFM 69 radios.<br>If your radio is RFM69CW - comment out line<br>with // #define MY_IS_RFM69HW 
#define MY_NODE_ID 0xE0 | Define Node address (0xE0 here). I prefer to use static addresses<br> and in Hexadecimal since it is easier to identify the node<br> address in  [Domoticz](https://domoticz.com/) devices list after it<br> will be discovered by controller ( [Domoticz](https://domoticz.com/)).<br> However, you can use AUTO instead of the hardcoded number<br> (like 0xE0) though.  [Domoticz](https://domoticz.com/) will automatically assign node ID then.
#define MY_SIGNING_ATSHA204 <br>#define  MY_SIGNING_REQUEST_SIGNATURES | Define if you like to use Crypto Authentication to secure your nodes<br> from intruders or interference. After that, you have to “personalize”<br> all the nodes, which have those, defines enabled.<br> [**How to “personalize” nodes with encryption key**](https://github.com/EasySensors/ButtonSizeNode/blob/master/SecurityPersonalizationHowTo.md).<br> You need both defines in the nodes you need to protect.<br> The Gateway Serial could be with only one of those<br> defines enabled - #define MY_SIGNING_ATSHA204
sleep(BUTTONS_INTERUPT_PIN - 2, RISING, 0); | Sends the Switch Controller into the sleep mode untill<br> somebody press any button. 

Connect the Node to FTDI USB adaptor, select Pro Mini 8MHz board in Arduino IDE and upload the switchBox.ino sketch.

**Done**


The board is created by  [Koresh](https://www.openhardware.io/user/143/projects/Koresh)

![arduino Switch Box](https://github.com/EasySensors/switchBox/blob/master/pics/arduino-Switch-Box-233.jpg?raw=true)
![arduino Switch Box](https://github.com/EasySensors/switchBox/blob/master/pics/arduino-Switch-Box-2.jpg?raw=true)
![arduino Switch Box](https://github.com/EasySensors/switchBox/blob/master/pics/arduino-Switch-Box-21.jpg?raw=true)
![arduino Switch Box](https://github.com/EasySensors/switchBox/blob/master/pics/arduino-Switch-Box-22.jpg?raw=true)
