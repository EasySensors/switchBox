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
 *******************************
 *
 * DESCRIPTION
 * The ArduinoGateway prints data received from sensors on the serial link.
 * The gateway accepts input on seral which will be sent out on radio network.
 *
 * The GW code is designed for Arduino Nano 328p / 16MHz
 *
 * Wire connections (OPTIONAL):
 * - Inclusion button should be connected between digital pin 3 and GND
 * - RX/TX/ERR leds need to be connected between +5V (anode) and digital pin 6/5/4 with resistor 270-330R in a series
 *
 * LEDs (OPTIONAL):
 * - To use the feature, uncomment any of the MY_DEFAULT_xx_LED_PINs
 * - RX (green) - blink fast on radio message recieved. In inclusion mode will blink fast only on presentation recieved
 * - TX (yellow) - blink fast on radio message transmitted. In inclusion mode will blink slowly
 * - ERR (red) - fast blink on error during transmission error or recieve crc error
 *
 */

// Enable debug prints to serial monitor
//#define MY_DEBUG
//#define MY_DEBUG_VERBOSE_RFM69




// Enable and select radio type attached

//#define MY_RADIO_NRF24

#define MY_RADIO_RFM69
#define MY_IS_RFM69HW

//#define MY_RFM69_FREQUENCY   RFM69_915MHZ
//#define MY_RFM69_FREQUENCY   RFM69_868MHZ
//#define MY_RFM69_FREQUENCY   RFM69_433MHZ
//#define MY_RFM69_NEW_DRIVER  

//BW125/SF128

//#define MY_RADIO_RFM95
//#define MY_RFM95_MODEM_CONFIGRUATION  RFM95_BW125CR45SF128
#define MY_RFM95_MODEM_CONFIGRUATION RFM95_BW_500KHZ | RFM95_CODING_RATE_4_5, RFM95_SPREADING_FACTOR_2048CPS | RFM95_RX_PAYLOAD_CRC_ON, RFM95_AGC_AUTO_ON // 

//#define MY_RFM95_TX_POWER_DBM (20u)
#define RFM95_RETRY_TIMEOUT_MS      (2500ul)

//#define   MY_RFM95_FREQUENCY RFM95_915MHZ
//#define   MY_RFM95_FREQUENCY RFM95_868MHZ
//#define   MY_RFM95_FREQUENCY RFM95_433MHZ

//#define MY_SIGNING_ATSHA204_PIN A2

//#define MY_SIGNING_ATSHA204
//#define  MY_SIGNING_REQUEST_SIGNATURES




//#define MY_RFM95_FREQUENCY   RFM95_915MHZ



//#define MY_OTA_FIRMWARE_FEATURE

// Set LOW transmit power level as default, if you have an amplified NRF-module and
// power your radio separately with a good regulator you can turn up PA level.
#define MY_RF24_PA_LEVEL RF24_PA_LOW

// Enable serial gateway
#define MY_GATEWAY_SERIAL

// Define a lower baud rate for Arduino's running on 8 MHz (Arduino Pro Mini 3.3V & SenseBender)
#if F_CPU == 8000000L
#define MY_BAUD_RATE 38400
#endif

// Enable inclusion mode
#define MY_INCLUSION_MODE_FEATURE
// Enable Inclusion mode button on gateway
//#define MY_INCLUSION_BUTTON_FEATURE

// Inverses behavior of inclusion button (if using external pullup)
//#define MY_INCLUSION_BUTTON_EXTERNAL_PULLUP

// Set inclusion mode duration (in seconds)
#define MY_INCLUSION_MODE_DURATION 60
// Digital pin used for inclusion mode button
//#define MY_INCLUSION_MODE_BUTTON_PIN  3

// Set blinking period
#define MY_DEFAULT_LED_BLINK_PERIOD 300

// Inverses the behavior of leds
//#define MY_WITH_LEDS_BLINKING_INVERSE

// Flash leds on rx/tx/err
// Uncomment to override default HW configurations
//#define MY_DEFAULT_ERR_LED_PIN 4  // Error led pin
//#define MY_DEFAULT_RX_LED_PIN  6  // Receive led pin
//#define MY_DEFAULT_TX_LED_PIN  5  // the PCB, on board LED


#define RELAY_pin 13 // Digital pin connected to relay

#define RELAY_sensor0 0
#define RELAY_sensor1 1

#include <MySensors.h>

MyMessage msg(RELAY_sensor0, V_LIGHT);
//MyMessage msg0(RELAY_sensor0, V_LIGHT);
//MyMessage msg1(RELAY_sensor1, V_LIGHT);



void before() {

  /*
  Serial.begin(115200);
  Serial.println("!");
  pinMode(9,OUTPUT);
  digitalWrite(9,LOW);
  wait(10);
  digitalWrite(9,HIGH);
  wait(10);
  digitalWrite(9,LOW);
  wait(10);
  */

  // Built in Arduino Nano LED
  pinMode(RELAY_pin,OUTPUT);
  digitalWrite(RELAY_pin,HIGH);//LOW

}

void setup() {
  // Setup locally attached sensors
}

void presentation() {
 // Present locally attached sensors
}

void loop() {
  // Send locally attached sensor data here
}

void receive(const MyMessage &message) {
  Serial.print("payload.. ");     Serial.println(message.getBool());
  Serial.print(" from.. "); Serial.println(message.sender);
  Serial.print(" sensor.. "); Serial.println(message.sensor);
  Serial.print(" message.getCommand().. "); Serial.println(message.getCommand()); 
  Serial.print(" message.typ.. "); Serial.println(message.type);

 if ((message.getCommand() == 1) && (message.type == V_LIGHT)  && !message.isAck()) {
     // Change relay state according to message payload value
     digitalWrite(RELAY_pin, message.getBool()?1:0);
     // Store state in eeprom
     saveState(message.sensor, message.getBool());
     // Report state to a controller
     msg.setDestination(0);
     send(msg.set(loadState(message.sensor)), true);
   }
}
