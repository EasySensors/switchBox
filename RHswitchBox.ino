#include <SPI.h>           //included with Arduino IDE install (www.arduino.cc)
#include <LowPower.h>
#include <RHReliableDatagram.h>
#include <RH_RF95.h>

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

// Singleton instance of the radio driver
RH_RF95 driver;

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, CLIENT_ADDRESS);

#define SERIAL_BAUD 115200

#define BUTTONS_INTERUPT_PIN 3

int BATTERY_SENSE_PIN = A6;  // select the input pin for the battery sense point
int oldBatteryPcnt = 0;

// Arduino pinout in the picture here: https://github.com/EasySensors/switchBox/blob/master/pics/buttonsLEDPinout.png

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
  
  while (!Serial) ; // Wait for serial port to be available
  if (!manager.init())
    Serial.println("init failed");
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  driver.setModemConfig((RH_RF95::ModemConfigChoice) 0);
  driver.setFrequency(915.0);

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  
  // driver.setTxPower(23, false);

  // Defaults to 200ms. For some slow rates nad with ReliableDatagrams youi may need to increase the reply timeout 
  // with manager.setTimeout() to deal with the long transmission times.
  
  // manager.setTimeout();

  // You can optionally require this module to wait until Channel Activity
  // Detection shows no activity on the channel before transmitting by setting
  // the CAD timeout to non-zero:
  // driver.setCADTimeout(10000);


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
   //Serial.println("loop()");
  //debounce delay
  delay(50);
  // Check active switches after interrupt wakeup 

  for (int i = 0; i < NUMBER_OF_BUTTONS; i++) {
    if (digitalRead(switchButtonPin[i]) == 0 && relayChildID[i] != NULL) {  
        readValue == 0 ? readValue = 1:readValue = 0;  // inverting the value each push
        char buffr[10];
        buffr[10] = '\0';
        sprintf(buffr,"B %i;V %i",relayChildID[i],readValue); //\/0
        // Send a message to manager_server
        if (manager.sendtoWait(buffr, sizeof(buffr), SERVER_ADDRESS)) {//GATEWAYID  relayNodeID[i]
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
    driver.sleep();
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); 
    detachInterrupt(1);
}
