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
#define MY_DEBUG_VERBOSE_RFM69

// The switch Node ID
#define MY_NODE_ID 0x1


/* Each JST connector status (On or Off) can be sent to a different Relay or Actuator NodeId address.  
 *  relayNodeID stores Relay or Actuator NodeId addresses. Each adress can have different Child\Sensor ID.  
 *  relayChildID is Child\Sensor ID's array. 
 * int relayNodeID[4] = {0xF2, 0xA0, 0x0, 0x0}; 
 * int relayChildID[4] = {1, 2, NULL, NULL};
 * above declaration means: JST connector 1 will send it state to assigned Relay Address 0xF2 with Child\Sensor ID 1. 
 * JST connector 2 will send it state to assigned Relay Address 0xA0 with Child\Sensor ID 2. 
 * JST connectors 3 - 4 have no attached sensorsors and will not be "presented" since there NULL values.
 * NULL value indicates no switch attached to the corresponding JST connector
*/

int relayNodeID[4] = {0x0, 0x0, 0x0, 0x0};
int relayChildID[4] = {1, NULL, NULL, NULL};

// Avoid battery drain if Gateway disconnected and the node sends more than MY_TRANSPORT_STATE_RETRIES times message.
#define MY_TRANSPORT_UPLINK_CHECK_DISABLED
#define MY_PARENT_NODE_IS_STATIC
#define MY_PARENT_NODE_ID 0

// Type of switches connected to the white JST connectors on the board.
// Momentary is notebook style key.
#define MOMENTARY_SWITCH

// Enable and select radio type attached

//#define MY_RADIO_NRF24

#define MY_RADIO_RFM69

// if you use MySensors 2.0 use this style 
//#define MY_RFM69_FREQUENCY   RF69_433MHZ
//#define MY_RFM69_FREQUENCY   RF69_868MHZ
//#define MY_RFM69_FREQUENCY   RF69_915MHZ


//#define MY_RFM69_FREQUENCY   RFM69_915MHZ
#define MY_RFM69_FREQUENCY   RFM69_868MHZ


//#define MY_RFM69_NEW_DRIVER

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

// Initialising array holding Arduino Digital I/O pins for button/reed switches
int switchButtonPin[3] = {4, 8, A0}; 

#define BUTTONS_COUNT 3
int switchButtonLeds[3] = {5, 6, 7};

int BATTERY_SENSE_PIN = A6;  // select the input pin for the battery sense point
int oldBatteryPcnt = 0;

// Initialising array holding button state messages
MyMessage msgSwitch[3];

void before()
{
  analogReference(INTERNAL);

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
  // There is no need to activate internal pull-ups
  for (int i = 0; i < BUTTONS_COUNT; i++) {
    //pinMode(switchButtonPin[i], INPUT);
    pinMode(switchButtonPin[i], INPUT_PULLUP);
    pinMode(switchButtonLeds[i], OUTPUT);
  }

}

byte getButtonState(int btnNum){
  if (btnNum == 1){
    return digitalRead(switchButtonPin[0]);
  }
  if (btnNum == 2){
    return digitalRead(switchButtonPin[1]);
  }
  if (btnNum == 3){
    return digitalRead(switchButtonPin[2]);
  }
}

void blinkButtonLed(int btnNum){
  digitalWrite(switchButtonLeds[btnNum - 1], HIGH);
  wait(50);
  digitalWrite(switchButtonLeds[btnNum - 1 ], LOW);
}

void presentation() {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo(SKETCH_NAME, SKETCH_MAJOR_VER "." SKETCH_MINOR_VER);

  // Register binary input sensor to sensor_node (they will be created as child devices)
  // You can use S_DOOR, S_MOTION or S_LIGHT here depending on your usage.
  // If S_LIGHT is used, remember to update variable type you send in. See "msg" above.

  for (int i = 0; i < BUTTONS_COUNT; i++) {
    if (relayChildID[i] != NULL) {
      msgSwitch[i] = MyMessage(relayChildID[i], V_LIGHT);
      present(relayChildID[i], S_LIGHT);
    }
  }
}

void setup() {

}

// Loop will iterate on changes if the BUTTON_PINs wake up the controller/node
void loop(){ 
  // Buttons state values array
  static uint8_t  readValue =  0;
  static  uint8_t last_value[4] = {NULL, NULL, NULL, NULL};
  
  uint8_t retry = 5;

  // Check active switches
  for (int i = 0; i < BUTTONS_COUNT; i++) {
    //if (relayChildID[i] != NULL && getButtonState(i+1) == 0) {
    if (getButtonState(i+1) == 0) {
        //uint8_t loadedState = loadState(relayChildID[i]);
        readValue == 0 ? readValue = 1:readValue = 0;  
        msgSwitch[i].setDestination(relayNodeID[i]);
        while (!send(msgSwitch[i].set(readValue), true)  && retry > 0) { 
          // send did not go through, try  "uint8_t retry = 5" more times
          //wait(100); 
          retry--;
        }
        blinkButtonLed(i+1);  
    }
  }

   /* This part of the code needs to be ajusted. Coming soon!!!
   // Get the battery Voltage
  int sensorValue = analogRead(BATTERY_SENSE_PIN);
  /* 1M, 470K divider across battery and using internal ADC ref of 1.1V1
   * ((1e6+470e3)/470e3)*1.1 = Vmax = 3.44 Volts
   * The MySensors Lib uses internal ADC ref of 1.1V which means analogRead of the pin connected to 470kOhms Battery Devider reaches  
   * 1023 when voltage on the divider is around 3.44 Volts. 2.5 volts is equal to 750. 2 volts is equal to 600. 
   * RFM 69 CW works stable up to 2 volts. Assume 2.5 V is 0% and 1023 is 100% battery charge    
   * 3.3V ~ 1023
   * 3.0V ~ 900
   * 2.5V ~ 750 
   * 2.0V ~ 600
   
  
  //  Serial.print("sensorValue: "); Serial.println(sensorValue); 
  int batteryPcnt = (sensorValue - 600)  / 3;
  
  batteryPcnt = batteryPcnt > 0 ? batteryPcnt:0; // Cut down negative values. Just in case the battery goes below 2V (2.5V) and the node still working. 
  batteryPcnt = batteryPcnt < 100 ? batteryPcnt:100; // Cut down more than "100%" values. In case of ADC fluctuations. 
  
  if (oldBatteryPcnt != batteryPcnt ) {
    //Power up radio after sleep
    // sendBatteryLevel(batteryPcnt);
    oldBatteryPcnt = batteryPcnt;
  } */

  sleep(BUTTONS_INTERUPT_PIN - 2, FALLING  , 0); 
}


