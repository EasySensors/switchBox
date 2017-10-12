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

#define MY_NODE_ID 0xAA

// Avoid battery drain if Gateway disconnected and the node sends more than MY_TRANSPORT_STATE_RETRIES times message.
#define MY_TRANSPORT_UPLINK_CHECK_DISABLED
#define MY_PARENT_NODE_IS_STATIC
#define MY_PARENT_NODE_ID 0

// Type of switches connected to the white JST connectors on the board.
// Momentary is notebook style key.
#define MOMENTARY_SWITCH

// Enable and select radio type attached
#define MY_RADIO_RFM69
#define MY_RFM69_FREQUENCY   RFM69_433MHZ
//#define MY_RFM69_FREQUENCY    RFM69_868MHZ
//#define MY_RFM69_FREQUENCY    RFM69_915MHZ

#define MY_RFM69_NEW_DRIVER

//#define MY_RADIO_NRF24

// Enable Crypto Authentication to secure the node
//#define MY_SIGNING_ATSHA204
//#define  MY_SIGNING_REQUEST_SIGNATURES

// Enable OTA feature
//#define MY_OTA_FIRMWARE_FEATURE
//#define MY_OTA_FLASH_JDECID 0x0 //0x2020 

#include <SPI.h>
#include <MySensors.h>

#define SPIFLASH_BLOCKERASE_32K   0xD8
#define SPIFLASH_CHIPERASE        0x60

#define SKETCH_NAME "Switch Node "
#define SKETCH_MAJOR_VER "2"
#define SKETCH_MINOR_VER "0"

// Initialising array holding button child ID's
// NULL values used to indicate no switch attached to the corresponding switch connector and no child ID need to be added in a Controller
int SWITCH_CHILD_ID[4] = {5, NULL, NULL, NULL};

#define BUTTONS_INTERUPT_PIN 3

// Initialising array holding Arduino Digital I/O pins for button/reed switches
int SWITCH_BUTTON_PIN[4] = {4, 5, 6, 7};

int BATTERY_SENSE_PIN = A6;  // select the input pin for the battery sense point
int oldBatteryPcnt = 0;

// Initialising array holding button state messages
MyMessage msg_switch[4];

void before()
{
  analogReference(INTERNAL);
  // RFM reset pin
  pinMode(9, OUTPUT);
  digitalWrite(9, 0);
  // Setup the buttons
  // There is no need to activate internal pull-ups
  for (int i = 0; i <= 3; i++) {
    pinMode(SWITCH_BUTTON_PIN[i], INPUT);
  }
}

void presentation() {
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo(SKETCH_NAME, SKETCH_MAJOR_VER "." SKETCH_MINOR_VER);

  // Register binary input sensor to sensor_node (they will be created as child devices)
  // You can use S_DOOR, S_MOTION or S_LIGHT here depending on your usage.
  // If S_LIGHT is used, remember to update variable type you send in. See "msg" above.
  for (int i = 0; i <= 3; i++) {
    if (SWITCH_CHILD_ID[i] != NULL) {
      msg_switch[i] = MyMessage(SWITCH_CHILD_ID[i], V_LIGHT);
      present(SWITCH_CHILD_ID[i], S_LIGHT);
    }
  }
}

void setup() {

}

// Loop will iterate on changes on the BUTTON_PINs
void loop()
{
  // Buttons state values array
  static uint8_t  readValue =  0;
  static  uint8_t last_value[4] = {NULL, NULL, NULL, NULL};
 
 // Get the battery Voltage
  int sensorValue = analogRead(BATTERY_SENSE_PIN);
  // 1M, 470K divider across battery and using internal ADC ref of 1.1V1
  // ((1e6+470e3)/470e3)*1.1 = Vmax = 3.44 Volts
  /* The MySensors Lib uses internal ADC ref of 1.1V which means analogRead of the pin connected to 470kOhms Battery Devider reaches  
   * 1023 when voltage on the divider is around 3.44 Volts. 2.5 volts is equal to 750. 2 volts is equal to 600. 
   * RFM 69 CW works stable up to 2 volts. Assume 2.5 V is 0% and 1023 is 100% battery charge    
   * 3.3V ~ 1023
   * 3.0V ~ 900
   * 2.5V ~ 750 
   * 2.0V ~ 600
   */

  Serial.print("sensorValue: "); Serial.println(sensorValue); 
  int batteryPcnt = (sensorValue - 600)  / 3;
  
  batteryPcnt = batteryPcnt > 0 ? batteryPcnt:0; // Cut down negative values. Just in case the battery goes below 2V (2.5V) and the node still working. 
  batteryPcnt = batteryPcnt < 100 ? batteryPcnt:100; // Cut down more than "100%" values. In case of ADC fluctuations. 

  if (oldBatteryPcnt != batteryPcnt ) {
    // Power up radio after sleep
    sendBatteryLevel(batteryPcnt);
    oldBatteryPcnt = batteryPcnt;
  }

// Check active switches
uint8_t retry = 5;
#ifdef MOMENTARY_SWITCH
  for (int i = 0; i <= 3; i++) {
    if (SWITCH_CHILD_ID[i] != NULL && digitalRead(SWITCH_BUTTON_PIN[i]) == 1) {
        //uint8_t loadedState = loadState(SWITCH_CHILD_ID[i]);
        readValue == 0 ? readValue = 1:readValue = 0;  
        while (!send(msg_switch[i].set(readValue), true)  && retry > 0) { 
          // send did not go through, try  "uint8_t retry = 5" more times
          sleep(100); retry--;
        }
    }
  }
#else
  for (int i = 0; i <= 3; i++) {
    if (SWITCH_CHILD_ID[i] != NULL) {
      readValue = digitalRead(SWITCH_BUTTON_PIN[i]);
      // find which connector(pin) triggered interrupt 
      if (last_value[i] != readValue) {
        last_value[i] = readValue;
        Serial.println("VALUE");
        Serial.println(readValue);
        while (!send(msg_switch[i].set(readValue), true)  && retry > 0) { 
          // send did not go through, try  "uint8_t retry = 5" more times
          sleep(100); retry--;
        }
      }
    }
  }
#endif
  sleep(BUTTONS_INTERUPT_PIN - 2, RISING, 0);
}
