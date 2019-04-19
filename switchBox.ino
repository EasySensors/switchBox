/**
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2015 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 * 
**/

// Enable debug prints to serial monitor
#define MY_DEBUG
//#define MY_DEBUG_VERBOSE_RFM69

// The switch Node ID
#define MY_NODE_ID 0x43


/* Each Button status (On or Off) can be sent to a different Relay or Actuator NodeId address.  
 *  relayNodeID stores Relay or Actuator NodeId addresses. Each adress can have different Child\Sensor ID.  
 *  relayChildID is Child\Sensor ID's array. 
 * int relayNodeID[3] = {0xF2, 0xA0,0x0}; 
 * int relayChildID[3] = {1, 2, NULL};
 * above declaration means: Button 1 will send it state to assigned Relay Address 0xF2 with Child\Sensor ID 1. 
 * Button 2 will send it state to assigned Relay Address 0xA0 with Child\Sensor ID 2. 
 * Button 3 have no attached sensorsors and will not be "presented" since there is NULL value.
 * NULL value means no need to report\present it to cntroller;
*/

#define TWO_BUTTONS_SWITCH 

//#define ONE_BUTTON_SWITCH
//#define THREE_BUTTON_SWITCH    // all buttons will work

// Arduino pinout in the picture here: https://github.com/EasySensors/switchBox/blob/master/pics/pinuot.jpg

#ifdef ONE_BUTTON_SWITCH
  int relayNodeID[1] = {0xf6}; 
  int relayChildID[1] = {1}; 
  #define NUMBER_OF_BUTTONS 1
  int switchButtonPin[1] = {8};  // D8 for switch
  int switchButtonLeds[1] = {6};  //D6 for LED. Although one switch enclosure is not transparent enough for LED.
#endif

#ifdef TWO_BUTTONS_SWITCH
  int relayNodeID[2] = {0x0, 0x0}; // Relay addressess for reach button to send switch ON\OFF states. Can be any address;
  int relayChildID[3] = {4, 4}; //NULL value means no need to report\present it to cntroller;
  #define NUMBER_OF_BUTTONS 2
  int switchButtonPin[2] = {4,A0};  // D4, A0 for switches
  int switchButtonLeds[2] = {5,7}; // D5, D7 for LED's.
#endif

#ifdef THREE_BUTTON_SWITCH
  int relayNodeID[3] = {0xf6, 0xf6, 0xf6}; // Relay addressess for reach button to send switch ON\OFF states. Can be any address;
  int relayChildID[3] = {1, 1, 1}; //NULL value means no need to report\present it to cntroller;
  #define NUMBER_OF_BUTTONS 3
  int switchButtonPin[3] = {4, 8, A0}; // D4, D8, A0 for switches
  int switchButtonLeds[3] = {5, 6, 7}; // D5, D6, D7 for LED's.
#endif


// Avoid battery drain if Gateway disconnected and the node sends more than MY_TRANSPORT_STATE_RETRIES times message.
#define MY_TRANSPORT_UPLINK_CHECK_DISABLED
#define MY_PARENT_NODE_IS_STATIC
#define MY_PARENT_NODE_ID 0


// Enable and select radio type attached

//#define MY_RADIO_NRF24

//#define MY_RADIO_RFM69
//#define MY_IS_RFM69HW

#define MY_RFM69_FREQUENCY   RFM69_915MHZ
//#define MY_RFM69_FREQUENCY   RFM69_868MHZ
//#define MY_RFM69_FREQUENCY   RFM69_433MHZ
//#define MY_RFM69_NEW_DRIVER  


#define MY_RADIO_RFM95
#define MY_RFM95_MODEM_CONFIGRUATION RFM95_BW_500KHZ | RFM95_CODING_RATE_4_5, RFM95_SPREADING_FACTOR_2048CPS | RFM95_RX_PAYLOAD_CRC_ON, RFM95_AGC_AUTO_ON // 
#define MY_RFM95_TX_POWER_DBM (20u)

#define   MY_RFM95_FREQUENCY RFM95_915MHZ
//#define   MY_RFM95_FREQUENCY RFM95_868MHZ
//#define   MY_RFM95_FREQUENCY RFM95_433MHZ


// Enable Crypto Authentication to secure the node
//#define MY_SIGNING_ATSHA204
//#define  MY_SIGNING_REQUEST_SIGNATURES

#include <MySensors.h>

#define SPIFLASH_BLOCKERASE_32K   0xD8
#define SPIFLASH_CHIPERASE        0x60

#define SKETCH_NAME "Switch Box "
#define SKETCH_MAJOR_VER "1"
#define SKETCH_MINOR_VER "0"

#define BUTTONS_INTERUPT_PIN 3

int BATTERY_SENSE_PIN = A6;  // select the input pin for the battery sense point
int oldBatteryPcnt = 0;

// Initialising array holding button state messages
MyMessage msgSwitch[3];

void before()
{
  analogReference(DEFAULT); //  INTERNAL
  #ifdef  MY_RADIO_RFM69
    /*  RFM reset pin is 9
     *  A manual reset of the RFM69HCW\CW is possible even for applications in which VDD cannot be physically disconnected.
     *  Pin RESET should be pulled high for a hundred microseconds, and then released. The user should then wait for 5 ms
     *  before using the module.
     */
    pinMode(9, OUTPUT);
    //reset RFM module
    digitalWrite(9, 1);
    delay(1);
    // set Pin 9 to high impedance
    pinMode(9, INPUT);
    delay(10);
  #endif

  // Setup the buttons
  for (int i = 0; i < NUMBER_OF_BUTTONS; i++) {
    //pinMode(switchButtonPin[i], INPUT);
    pinMode(switchButtonPin[i], INPUT_PULLUP);
    pinMode(switchButtonLeds[i], OUTPUT);
  }
}

void blinkButtonLed(int btnNum){
  digitalWrite(switchButtonLeds[btnNum - 1], HIGH);
  wait(50);
  digitalWrite(switchButtonLeds[btnNum - 1 ], LOW);
}

void blinkButtonLedFail(int btnNum){
  for (int i = 0; i < 4; i++) {
    digitalWrite(switchButtonLeds[btnNum - 1], HIGH);
    wait(30);
    digitalWrite(switchButtonLeds[btnNum - 1 ], LOW);
    wait(30);
  }
}

void presentation() {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo(SKETCH_NAME, SKETCH_MAJOR_VER "." SKETCH_MINOR_VER);

  // Register binary input sensor to sensor_node (they will be created as child devices)
  // You can use S_DOOR, S_MOTION or S_LIGHT here depending on your usage.
  // If S_LIGHT is used, remember to update variable type you send in. See "msg" above.

  for (int i = 0; i < NUMBER_OF_BUTTONS; i++) {
    if (relayChildID[i] != NULL) {
      msgSwitch[i] = MyMessage(relayChildID[i], V_LIGHT);
      present(relayChildID[i], S_LIGHT);
      blinkButtonLed(i+1);
    }
  }
}

void setup() {

}

// Loop will iterate on changes if the BUTTON_PINs wake up the controller/node
void loop(){ 
  // Buttons state values array
  static uint8_t  readValue =  0;
  uint8_t retry = 5;

  // Check active switches
  for (int i = 0; i < NUMBER_OF_BUTTONS; i++) {
    if (digitalRead(switchButtonPin[i]) == 0 && relayChildID[i] != NULL) {  
        readValue == 0 ? readValue = 1:readValue = 0;  // inverting the value each push
        msgSwitch[i].setDestination(relayNodeID[i]); 
        bool stat =  send(msgSwitch[i].set(readValue), true);
        // wait for ACK signal up to 2500 miliseconds
        wait(2500, 1, 2);
        stat ? blinkButtonLed(i+1) : blinkButtonLedFail(i+1);
    }
  }

  // Get the battery Voltage
  int sensorValue = analogRead(BATTERY_SENSE_PIN);
  /* 1M, 470K divider across batteries
   * 610 ~ 100 % is close to 6.1 V
   * 400 ~ 0 % is close to 4V
   */
  int batteryPcnt = (sensorValue - 400)  / 2;
  
  batteryPcnt = batteryPcnt > 0 ? batteryPcnt:0; // Cut down negative values. Just in case the battery goes below 4V and the node still working. 
  batteryPcnt = batteryPcnt < 100 ? batteryPcnt:100; // Cut down more than "100%" values. In case of ADC fluctuations. 

  if (oldBatteryPcnt != batteryPcnt ) {
    sendBatteryLevel(batteryPcnt);
    oldBatteryPcnt = batteryPcnt;
  }

  sleep(BUTTONS_INTERUPT_PIN - 2, FALLING  , 0); 
}
