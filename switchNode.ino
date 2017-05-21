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
#define MY_RFM69_FREQUENCY   RF69_433MHZ

//#define MY_RADIO_NRF24

// Enable Crypto Authentication to secure the node
//#define MY_SIGNING_ATSHA204
//#define  MY_SIGNING_REQUEST_SIGNATURES

// Enable OTA feature
#define MY_OTA_FIRMWARE_FEATURE
#define MY_OTA_FLASH_JDECID 0x2020

#include <SPI.h>
#include <MySensors.h>

#define SPIFLASH_BLOCKERASE_32K   0xD8
#define SPIFLASH_CHIPERASE        0x60

#define SKETCH_NAME "Switch Node "
#define SKETCH_MAJOR_VER "2"
#define SKETCH_MINOR_VER "0"

// Initialising array holding button child ID's
// NULL values used to indicate no switch attached to the corresponding switch connector and no child ID need to be added in a Controller
int SWITCH_CHILD_ID[4] = {1, 2, 3, NULL};


#define BUTTONS_INTERUPT_PIN 3

// Initialising array holding Arduino Digital I/O pins for button/reed switches
int SWITCH_BUTTON_PIN[4] = {4, 5, 6, 7};

int BATTERY_SENSE_PIN = A6;  // select the input pin for the battery sense point
int oldBatteryPcnt = 0;

// Initialising array holding button state messages
MyMessage msg_switch[4];

void before()
{
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
  analogReference(INTERNAL);

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
  static uint8_t  value[4] = {NULL, NULL, NULL, NULL};
  static  uint8_t last_value[4] = {NULL, NULL, NULL, NULL};

  // Get the battery Voltage
  int sensorValue = analogRead(BATTERY_SENSE_PIN);
  // 1M, 470K divider across battery and using internal ADC ref of 1.1V
  // Sense point is bypassed with 0.1 uF cap to reduce noise at that point
  // ((1e6+470e3)/470e3)*1.1 = Vmax = 3.44 Volts
  // 3.44/1023 = Volts per bit = 0.003363075

  int batteryPcnt = sensorValue / 10;
  if (oldBatteryPcnt != batteryPcnt) {
    // Power up radio after sleep
    sendBatteryLevel(batteryPcnt);
    oldBatteryPcnt = batteryPcnt;
  }

// Check active switches
#ifdef MOMENTARY_SWITCH
  for (int i = 0; i <= 3; i++) {
    if (SWITCH_CHILD_ID[i] != NULL) {
      value[i] = digitalRead(SWITCH_BUTTON_PIN[i]);
      if (value[i] == 1) { //last_value[i] != value[i] &&
        uint8_t loadedState = loadState(SWITCH_CHILD_ID[i]);
        if (last_value[i] == NULL) {
          last_value[i] = 0;
        }
        if ( loadedState == 255) {
          //no controller found
          send(msg_switch[i].set(!last_value[i] ? true : false), true);
          last_value[i] = !last_value[i];
        }
        else  {
          send(msg_switch[i].set(!loadedState ? true : false), true);
          last_value[i] = loadedState;
        }
      }
    }
  }
  wait(100);
#else
  for (int i = 0; i <= 3; i++) {
    if (SWITCH_CHILD_ID[i] != NULL) {
      value[i] = digitalRead(SWITCH_BUTTON_PIN[i]);
      if (last_value[i] != value[i]) {
        last_value[i] = value[i];
        Serial.println("VALUE");
        Serial.println(value[i]);
        send(msg_switch[i].set(value[i] ? false : true), true);
      }
    }
  }
  wait(100);
#endif
  sleep(BUTTONS_INTERUPT_PIN - 2, RISING, 0);
}
