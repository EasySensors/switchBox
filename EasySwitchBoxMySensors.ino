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

// The switch Node ID  ---------------------------------------------------------
#define MY_NODE_ID  177 //0xBc  // 0xBe // AUTO


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


// Arduino pinout in the picture here: https://github.com/EasySensors/switchBox/blob/master/pics/pinuot.jpg
/*
  int relayNodeID[3] = {0, 0, 0}; // Relay addressess for reach button to send switch ON\OFF states. Can be any address;
  int relayChildID[3] = {1, 1, 1}; //NULL value means no need to report\present it to cntroller;
  #define NUMBER_OF_BUTTONS 3
  int switchButtonPin[3] = {4, 4, A0}; // D4, D8, A0 for switches
  int switchButtonLeds[3] = {5, 5, 7}; // D5, D6, D7 for LED's.
*/
//---------------------------------------------------------------------------------------
#define NUMBER_OF_BUTTONS 2

int relayNodeID[2] = {0, 0}; // 0x .. is the relay addressess for each button to send switch ON\OFF states. Can be any address; 0 is SmartHome controller address.
int relayChildID[2] = {4,4}; //{4,4}; //0 value means no need to report\present it to cntroller;

int switchButtonPin[2] = {4,A0};  //Arduino Pins D4, A0 are for switches
int switchButtonLeds[2] = {5,7}; // Arduino Pins D5, D7 are for LED's.


// Avoid battery drain if Gateway disconnected and the node sends more than MY_TRANSPORT_STATE_RETRIES times message.
#define MY_TRANSPORT_UPLINK_CHECK_DISABLED
#define MY_PARENT_NODE_IS_STATIC
#define MY_PARENT_NODE_ID 0


// Enable and select radio type attached

#define MY_RADIO_RFM69
//#define MY_IS_RFM69HW
#define MY_RFM69_TX_POWER_DBM (14u) 

//#define MY_RFM69_FREQUENCY   RFM69_915MHZ
#define MY_RFM69_FREQUENCY   RFM69_868MHZ
//#define MY_RFM69_FREQUENCY   RFM69_433MHZ


//#define MY_RADIO_RFM95
#define MY_RFM95_MODEM_CONFIGRUATION RFM95_BW_500KHZ | RFM95_CODING_RATE_4_5, RFM95_SPREADING_FACTOR_2048CPS | RFM95_RX_PAYLOAD_CRC_ON, RFM95_AGC_AUTO_ON // 
#define MY_RFM95_TX_POWER_DBM (14u) // upto 24u 14

#define   MY_RFM95_FREQUENCY RFM95_868MHZ
//#define   MY_RFM95_FREQUENCY RFM95_915MHZ
//#define   MY_RFM95_FREQUENCY RFM95_433MHZ

#define RFM95_RETRY_TIMEOUT_MS      (700ul)

// Enable Crypto Authentication to secure the node
//#define MY_SIGNING_ATSHA204
//#define  MY_SIGNING_REQUEST_SIGNATURES

#include <MySensors.h>

#define SKETCH_NAME "easySwitchBox "
#define SKETCH_MAJOR_VER "2"
#define SKETCH_MINOR_VER "0"

#define BUTTONS_INTERUPT_PIN 3

int BATTERY_SENSE_PIN = A6;  // select the input pin for the battery sense point
uint8_t batteryPcnt, oldBatteryPcnt = 0xFF; 

// Initialising array holding button state messages
MyMessage msgSwitch[2];


void before()
{
  analogReference(INTERNAL); //  DEFAULT  

 
  
  #ifdef  MY_RADIO_RFM69 or MY_RADIO_RFM95
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
  pinMode(BATTERY_SENSE_PIN, INPUT);

  //reading unused pin - init ADC 
  analogRead(A7);
  // Get the battery Voltage
  int sensorValue = analogRead(BATTERY_SENSE_PIN);


  float voltage = sensorValue*0.007930934;   //3M \ 470k divider analogReference(INTERNAL);
  //float voltage = sensorValue*0.010079372; //1M \ 470k divider analogReference(INTERNAL);

  
  if (voltage > 3.5 && voltage < 5.6 )  {
   // The battery level is critical. To avoid rebooting and flooding radio with presentation messages
   // turn the device into hwSleep mode.

   #ifdef MY_DEBUG
      Serial.print(F("Before() sensorValue ")); Serial.println(sensorValue);
      Serial.print(F("Before() voltage ")); Serial.println(voltage);
      Serial.println("Voltage is below 5.6 Volts. Halting boot"); 
   #endif

   blinkButtonLedFail(1);
   blinkButtonLedFail(2); 
   blinkButtonLedFail(1);
   blinkButtonLedFail(2); 

  
   hwSleep(BUTTONS_INTERUPT_PIN - 2, FALLING  , 0);
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
    if (relayChildID[i] != 0) {
      present(relayChildID[i], S_BINARY);
      msgSwitch[i] = MyMessage(relayChildID[i], V_STATUS); 
      blinkButtonLed(i+1);
    }
  }
}

void setup() {
  
 
}



void batteryLevelRead(){
  
  // Get the battery Voltage
  
  int sensorValue = analogRead(BATTERY_SENSE_PIN);
  /*  Devider values R1 = 3M, R2 = 470K divider across batteries
   *  Vsource = Vout * R2 / (R2+R1)   = 7,383 * Vout;
   *  we use internal refference voltage of 1.1 Volts. Means 1023 Analg Input values  = 1.1Volts
   *  5.7 V is close to dead bateries under the working load. 6.3 or more - is 100% something in between is working range.
   */
   
  // voltage report valid in room temperatures
  
  float voltage = sensorValue*0.007930934;   //3M \ 470k divider analogReference(INTERNAL);
  //float voltage = sensorValue*0.010079372; //1M \ 470k divider analogReference(INTERNAL);

  Serial.print(F("batteryLevelRead() sensorValue ")); Serial.println(sensorValue);
  Serial.print(F("batteryLevelRead()) voltage ")); Serial.println(voltage);
 
  if (voltage > 6.3)  batteryPcnt = 100;
  else if (voltage < 5.7) batteryPcnt = 0;
  else batteryPcnt = (int)((voltage - 5.7) / 0.006) ;
}


void batteryReport(){
  if ( (abs(oldBatteryPcnt - batteryPcnt) > 10 ) || oldBatteryPcnt == 0xFF ) {
    sendBatteryLevel(batteryPcnt);
    // this wait(); is 2.0 and up RFM69 specific. Hope to get rid of it soon
    // TSF:MSG:SEND,238-238-0-0,s=255,c=3,t=0,pt=1,l=1,sg=0,ft=0,st=OK:100
   
    // wait for ACK signal up to RFM95_RETRY_TIMEOUT_MS or 50ms for rfm miliseconds
    #ifdef  MY_RADIO_RFM95
      wait(RFM95_RETRY_TIMEOUT_MS, 3, 0);
    #endif
    #ifdef  MY_RADIO_RFM69
     // waiting up to xxx millis ACK of type 2 message t=2
      wait(500, 3, 0);
    #endif
    oldBatteryPcnt = batteryPcnt;
  }
}


// Loop will iterate on changes if the BUTTON_PINs wake up the controller/node
void loop(){ 
  // Buttons state values array
  static uint8_t  readValue =  0;

  batteryLevelRead();

  // Check active switches after interrupt wakeup 
  for (int i = 0; i < NUMBER_OF_BUTTONS; i++) {
    if ((digitalRead(switchButtonPin[i]) == 0) && (relayChildID[i] != 0)) {  
        readValue == 0 ? readValue = 1:readValue = 0;  // inverting the value each push
        msgSwitch[i].setDestination(relayNodeID[i]); 
        //msgSwitch[i].type = V_LIGHT;
        bool stat =  send(msgSwitch[i].set(readValue), true);
        // wait for ACK signal up to RFM95_RETRY_TIMEOUT_MS or 50ms for rfm miliseconds
        #ifdef  MY_RADIO_RFM95
          wait(RFM95_RETRY_TIMEOUT_MS, 1, 3);
        #endif
        #ifdef  MY_RADIO_RFM69
          wait(200, 1, 3);
        #endif
        // Blink  respective LED's once if message delivered. 3 times if failed to receive ACK
        stat ? blinkButtonLed(i+1) : blinkButtonLedFail(i+1);
    }
  }

  batteryReport();
  
  sleep(BUTTONS_INTERUPT_PIN - 2, FALLING  , 0); 
}
