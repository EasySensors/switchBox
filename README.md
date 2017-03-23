

![enter image description here](https://github.com/EasySensors/SwitchNode/blob/master/pics/switch.jpg?raw=true)

# The Switch Node is a low cost wireless Arduino IDE compatible (the Atmel ATMega328P) microcontroller with RFM 69 CW 433 MHz radio on board and build in hardware debouncer. Best sutable for Home Automation, IOT. Could be used as switch board for radio controlling any DIY project. Check Specs below. It replaces this setup:

![](https://github.com/EasySensors/SwitchNode/blob/master/pics/replace.jpg?raw=true)


## Specification: ##
 - Dimensions 32x50mm
 - Hardware debouncer
 - 4 external switch connectors
 - Authentication security - Atmel ATSHA204A Crypto Authentication Chip
 - External JDEC EPROM
 - Dualoptiboot bootloader. Implements over the air (OTA) firmware update ability
 - RFM69-CW (low power consumption version) 433 MHz Radio transceiver
 - Footprint available for replacing  RFM69-CW with NRF24L01+ 2.4 GHz radio
 - Battery voltage sensor (via divider)
 - Externall battery connector JST 1.25 мм
 - FTDI header for programming
 - Battery connector CR2450


**Pin out:** 


Arduino Pins|	Description
------------|--------------
A0, A1, A4, A5, A7 |	Available ARDUINO analog GPIO / DIGITAL GPIO  ![](https://github.com/EasySensors/SwitchNode/blob/master/pics/a1-5s.jpg?raw=true)
A6 |	Connected to Battery voltage sensor (via divider)
A3 |	Connected to  ATSHA204A
D3 | Connected to Hardware debouncer. Triggers Interrupt<br> if any of the Switch Connectors change state
D4, D5, D6, D7 | Connected to Switch Connectors JST type
D8 |	Connected to CS FLASH chip (OTA) M25P40
ANT |	RFM69 antenna
Gnd | Ground



**Arduino IDE Settings**

![Arduino IDE Settings](https://github.com/EasySensors/ButtonSizeNode/blob/master/pics/IDEsettings.jpg?raw=true)



How to use it as home automation (IOT) node controller
------------------------------------------------------


switchNode.ino is the Arduino example sketch using [MySensors](https://www.mysensors.org/) API. 

Burn the switchNode.ino sketch into it an it will became  one of the MySensors home automation network Node. 
To create the network you need controller and at least two Nodes one as a Sensor, relay or switch Node and the other one as “Gateway Serial”. I personally love [Domoticz](https://domoticz.com/) as conroller. Please check this [HowTo](https://github.com/EasySensors/ButtonSizeNode/blob/master/DomoticzInstallMySensors.md) to install Domoticz.

However, for no-controller setup, as example, you can use 3 nodes - first node as “Gateway Serial”, second node as relay and lastone as switch for that relay. No controller needed then, keep the switch and the relay on the same address and the switch will operate the relay.

Things worth mentioning about the  [MySensors](https://www.mysensors.org/) Arduino sketch: 


Arduino Pins|	Description
------------|--------------
#define MY_RADIO_RFM69<br>#define MY_RFM69_FREQUENCY   RF69_433MHZ<br>#define MY_IS_RFM69HW|	Define which radio we use – here is RFM 69<br>with frequency 433 MHZ and it is HW<br>type – one of the most powerful RFM 69 radios.<br>If your radio is RFM69CW - comment out line<br>with // #define MY_IS_RFM69HW 
#define MY_NODE_ID 0xE0 | Define Node address (0xE0 here). I prefer to use static addresses<br> and in Hexadecimal since it is easier to identify the node<br> address in  [Domoticz](https://domoticz.com/) devices list after it<br> will be discovered by controller ( [Domoticz](https://domoticz.com/)).<br> However, you can use AUTO instead of the hardcoded number<br> (like 0xE0) though.  [Domoticz](https://domoticz.com/) will automatically assign node ID then.
#define MY_OTA_FIRMWARE_FEATURE<br>#define MY_OTA_FLASH_JDECID 0x2020 | Define OTA feature. OTA stands for “Over The Air firmware updates”.<br> If your node does not utilize Sleep mode you can send new “firmware”<br> (compiled sketch binary) by air. **Here is the link on how to do it.** <br>For OTA we use JDEC Flash chip where the node stores<br> new firmware and once it received and controlsum (CRC) is correct<br>  it reboots and flashes your new code into the node<br> controller. So we define it is "erase type" as 0x2020 here. 
#define MY_SIGNING_ATSHA204 | Define if you like to use Crypto Authentication to secure your nodes<br> from intruders or interference. After that, you have to “personalize”<br> all the nodes, which have those, defines enabled.<br> [**How to “personalize” nodes with encryption key**](https://github.com/EasySensors/ButtonSizeNode/blob/master/SecurityPersonalizationHowTo.md)
sleep(BUTTONS_INTERUPT_PIN - 2, RISING, 0); | Sends the Switch Controller into the sleep mode untill<br> somebody press or release any of the 4 buttons. 

Connect the Node to FTDI USB adaptor, select Pro Mini 8MHz board in Arduino IDE and upload the switchNode.ino sketch.

**Done**

>For schematics lovers:

![enter image description here](https://github.com/EasySensors/vvvvvvvvvvvvvvvvvvv/blob/master/pics/schematic.jpg?raw=true)

The board is created by  [Koresh](https://www.openhardware.io/user/143/projects/Koresh)
