// **********************************************************************************
// Struct Send RFM69 Example
// **********************************************************************************
// Copyright Felix Rusu 2018, http://www.LowPowerLab.com/contact
// **********************************************************************************
// License
// **********************************************************************************
// This program is free software; you can redistribute it 
// and/or modify it under the terms of the GNU General    
// Public License as published by the Free Software       
// Foundation; either version 3 of the License, or        
// (at your option) any later version.                    
//                                                        
// This program is distributed in the hope that it will   
// be useful, but WITHOUT ANY WARRANTY; without even the  
// implied warranty of MERCHANTABILITY or FITNESS FOR A   
// PARTICULAR PURPOSE. See the GNU General Public        
// License for more details.                              
//                                                        
// Licence can be viewed at                               
// http://www.gnu.org/licenses/gpl-3.0.txt
//
// Please maintain this license information along with authorship
// and copyright notices in any redistribution of this code
// **********************************************************************************
#include <RFM69.h>         //get it here: https://www.github.com/lowpowerlab/rfm69
#include <RFM69_ATC.h>     //get it here: https://www.github.com/lowpowerlab/rfm69
//#include <SPI.h>           //included with Arduino IDE install (www.arduino.cc)
#include <LowPower.h>
//*********************************************************************************************
//************ IMPORTANT SETTINGS - YOU MUST CHANGE/CONFIGURE TO FIT YOUR HARDWARE *************
//*********************************************************************************************
#define NODEID      4
#define NETWORKID   100
#define GATEWAYID   1
//Match frequency to the hardware version of the radio on your Moteino (uncomment one):
//#define FREQUENCY     RF69_433MHZ
//#define FREQUENCY     RF69_868MHZ
#define FREQUENCY     RF69_915MHZ

//#define ENCRYPTKEY    "sampleEncryptKey" //has to be same 16 characters/bytes on all nodes, not more not less!

// #define IS_RFM69HW_HCW  //uncomment only for RFM69HW/HCW! Leave out if you have RFM69W/CW!

//*********************************************************************************************
//Auto Transmission Control - dials down transmit power to save battery
//Usually you do not need to always transmit at max output power
//By reducing TX power even a little you save a significant amount of battery power
//This setting enables this gateway to work with remote nodes that have ATC enabled to
//dial their power down to only the required level

#define ENABLE_ATC    //comment out this line to disable AUTO TRANSMISSION CONTROL
//*********************************************************************************************
#define SERIAL_BAUD 115200

#ifdef ENABLE_ATC
  RFM69_ATC radio;
#else
  RFM69 radio;
#endif


byte sendSize=0;
boolean requestACK = false;

#define BUTTONS_INTERUPT_PIN 3

int BATTERY_SENSE_PIN = A6;  // select the input pin for the battery sense point
int oldBatteryPcnt = 0;

// Arduino pinout in the picture here: https://github.com/EasySensors/switchBox/blob/master/pics/pinuot.jpg

int relayNodeID[2] = {1, 1}; // Relay addressess for reach button to send switch ON\OFF states. Can be any address; 0 is SmartHome controller address.
int relayChildID[2] = {1, 2}; //NULL value means no need to report\present it to cntroller;
#define NUMBER_OF_BUTTONS 2
int switchButtonPin[2] = {4,A0};  //Arduino Pins D4, A0 are for switches
int switchButtonLeds[2] = {5,7}; // Arduino Pins D5, D7 are for LED's.


void wakeUp()
{


}

void blinkButtonLed(int btnNum){
  digitalWrite(switchButtonLeds[btnNum - 1], HIGH);
  delay(50);
  digitalWrite(switchButtonLeds[btnNum - 1 ], LOW);
}

void blinkButtonLedFail(int btnNum){
  for (int i = 0; i < 4; i++) {
    digitalWrite(switchButtonLeds[btnNum - 1], HIGH);
    delay(30);
    digitalWrite(switchButtonLeds[btnNum - 1 ], LOW);
    delay(30);
  }
}


void setup() {
  Serial.begin(115200);

 pinMode(9, OUTPUT);
    //reset RFM module
    digitalWrite(9, 1);
    delay(1);
    // set Pin 9 to high impedance
    pinMode(9, INPUT);
    delay(10);
  

  if (!radio.initialize(FREQUENCY,NODEID,NETWORKID)){
    Serial.println("RADIO INIT FAIL");
  }
    
  #ifdef IS_RFM69HW_HCW
    radio.setHighPower(); //must include this only for RFM69HW/HCW!
  #endif
  //radio.encrypt(ENCRYPTKEY);
  //radio.promiscuous(false);
  analogReference(DEFAULT); //  INTERNAL
  /*  RFM reset pin is 9
   *  A manual reset of the RFM69HCW\CW is possible even for applications in which VDD cannot be physically disconnected.
   *  Pin RESET should be pulled high for a hundred microseconds, and then released. The user should then wait for 5 ms
   *  before using the module.
   */
   

    // Setup the buttons
    for (int i = 0; i < NUMBER_OF_BUTTONS; i++) {
      //pinMode(switchButtonPin[i], INPUT);
      pinMode(switchButtonPin[i], INPUT_PULLUP);
      pinMode(switchButtonLeds[i], OUTPUT);
    }

  for (int i = 0; i < NUMBER_OF_BUTTONS; i++) {
      blinkButtonLed(i+1);
    }
    
  attachInterrupt(1, wakeUp, FALLING);
}

void loop() {
  // Buttons state values 
  static uint8_t  readValue =  0;
  
  //debounce delay
  delay(50);
  // Check active switches after interrupt wakeup 

  for (int i = 0; i < NUMBER_OF_BUTTONS; i++) {
    if (digitalRead(switchButtonPin[i]) == 0 && relayChildID[i] != NULL) {  
        readValue == 0 ? readValue = 1:readValue = 0;  // inverting the value each push
        char buffr[10];
        buffr[10] = '\0';
        sprintf(buffr,"B %i;V %i",relayChildID[i],readValue); //\/0
        if (radio.sendWithRetry(relayNodeID[i], buffr, sizeof(buffr))) {//GATEWAYID  relayNodeID[i]
        // Blink  respective LED's once 
        blinkButtonLed(i+1) ;
        }
        else {
          // Blink  respective LED's 3 times = failed to receive ACK
          blinkButtonLedFail(i+1);
        }
    }
  }
    attachInterrupt(1, wakeUp, FALLING);
    radio.sleep();
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); 
    detachInterrupt(1);
}
