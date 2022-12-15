/*
 * bininps2
 *
 * Copyright (c) 2015 panStamp <contact@panstamp.com>
 * 
 * This file is part of the panStamp project.
 * 
 * panStamp  is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 * 
 * panStamp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with panStamp; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301
 * USA
 * 
 * Author: Daniel Berenguer
 * Creation date: 04/01/2015
 *
 * Device:
 * Binary input + counter module
 *
 * Description:
 * Device that reports the binary state of 12 digital inputs, 4 of them being
 * used as pulse counters as well.
 *
 * PANSTAMP AVR
 * This sketch is not supported by panStamp AVR.
 * panStamp AVR does not support attachInterrupt on pins other than INT1 (D3).
 * RTC calendar mode is not supported by AVR modules either.
 * With panStamp AVR use the classic "bininps" sketch or "pulsecounter"
 *
 * PANSTAMP NRG
 * Binary inputs only!
 * This sketch uses internal pull-up's. Counters increment when inputs turn down
 * (LOW state)
 *
 * This sketch can be used to detect key/switch presses, binary alarms or
 * any other binary sensor. It can also be used to count pulses from a vaste
 * variety of devices such as energy meters, water meters, gas meters, etc.
 * You may want to add some delays in updateValues in order to better debounce
 * the binary states. We suggest to add external capacitors to the inputs as well.
 * Capacitor values and delays should depend on the nature and frequence of the
 * input signals.
 *
 * This device is NOT low-power enabled so it will not enter low-power mode just
 * after reading the binary states and transmitting them over the SWAP
 * network. Powering over battery is not recommend
 *
 * Associated Device Definition File, defining registers, endpoints and
 * configuration parameters:
 * windowstatussensor.xml (Window Status Sensor)
 */
 
#include "swap.h"
#include "regtable.h"
#include "HardwareSerial.h"

/* #define DEBUG */

/**
 * Maximum repeating count
 */
const byte maxRepeaterHop = 1;

/**
 * Register data containers made global
 */
byte dtBinInputs[1];    // Binary input states
boolean raisedIRQ = false;
static unsigned long last_transmission_time = 0;
uint8_t group_assignment[8] = {0,0,0,0,0,0,0,0}; // welcher Pin ist welchem Port zugeordnet? -> group_assignment[PinNumber] = GroupNumber
                                                 // Gruppe 0 - 7 ist valid
uint8_t group_count[8]      = {0,0,0,0,0,0,0,0}; // wieviele Pins sind welcher Gruppe zugeordnet? -> group_count[GroupNumber] = <anzahl>
uint8_t group_state[8]      = {0,0,0,0,0,0,0,0}; // Status der einzelnen Gruppe -> group_state[GroupNumber] = Status
                                                 // Status: 0 => closed
                                                 // Status: 1 => tilted
                                                 // Status: 2 => opened
                                                 
unsigned int LED_state = 0;               // Overall Status; 0 => closed, 1 => tilted, 2 => opened
unsigned int LED_R_State = LOW;           // ledState used to set the Red LED
unsigned int LED_Y_State = LOW;           // ledState used to set the Yellow LED
unsigned int LED_G_State = LOW;           // ledState used to set the Green LED
unsigned long LED_BlinkOnTime = 0;        // interval for blinking LED, set in loop, eg. 50ms
unsigned long LED_PrevMillis = 0;         // will store last time LED was updated
boolean send_config = false;              // if true, send config in loop
//variable initiaization, setup by configuration 
unsigned long LED_BlinkOnEvtTime = 5000;  // Blink for ms if an IRQ occurs  
unsigned long LED_BlinkOnIdleTime = 50;   // Blink for ms if an nothing occurs  
unsigned long LED_BlinkOffTime =  10000;  // during blinking, eg. 10s (50ms on, 10s off)


// RTC data
//RTCDATA rtcData;

/**
 * check if TXIntervall is reached
 */
bool isTimeToTX(){
  if( millis() - last_transmission_time >= (unsigned long)1000 * swap.txInterval )
    {   
      last_transmission_time = millis();
      return true;
    }   
   return false;
}

/**
 * handling debug messages
 */
void debug(char* txt, bool newline) {
  #ifdef DEBUG
  if (newline) {Serial.println(txt);} else {Serial.print(txt);}
  #endif
}
void debug_bin(byte txt, bool newline) {
  #ifdef DEBUG
  if (newline) {Serial.println(txt, BIN);} else {Serial.print(txt, BIN);}
  #endif
}
void debug_hex(byte txt, bool newline) {
  #ifdef DEBUG
  if (newline) {Serial.println(txt, HEX);} else {Serial.print(txt, HEX);}
  #endif
}
void debug_dec(byte txt, bool newline) {
  #ifdef DEBUG
  if (newline) {Serial.println(txt, DEC);} else {Serial.print(txt, DEC);}
  #endif
}

/**
 * Set LED State
 * LED_state: 0 => closed, 1 => tilted, 2 => opened
 */
void SetLEDState(unsigned int state, bool OnOff){
  // Startzustand herstellen
  LED_R_State = LOW;
  LED_Y_State = LOW;
  LED_G_State = LOW;
  
  if (state == 0) LED_G_State = OnOff;
  else if (state == 1) LED_Y_State = OnOff;
  else if (state == 2) LED_R_State = OnOff;

  digitalWrite(LED_R, LED_R_State);
  digitalWrite(LED_Y, LED_Y_State);
  digitalWrite(LED_G, LED_G_State);
}

/**
 * Input Change Interrupts
 * 1. Bit auf 0 setzen
 * 2. Bit auf den gemessenen Wert setzen
 */
void irq0(void){ dtBinInputs[0] = dtBinInputs[0] & 0xFE; dtBinInputs[0] |= digitalRead(cnt0);      raisedIRQ=true; debug("PinChange 1", true);}
void irq1(void){ dtBinInputs[0] = dtBinInputs[0] & 0xFD; dtBinInputs[0] |= digitalRead(cnt1) << 1; raisedIRQ=true; debug("PinChange 2", true);}
void irq2(void){ dtBinInputs[0] = dtBinInputs[0] & 0xFB; dtBinInputs[0] |= digitalRead(cnt2) << 2; raisedIRQ=true; debug("PinChange 3", true);}
void irq3(void){ dtBinInputs[0] = dtBinInputs[0] & 0xF7; dtBinInputs[0] |= digitalRead(cnt3) << 3; raisedIRQ=true; debug("PinChange 4", true);}
void irq4(void){ dtBinInputs[0] = dtBinInputs[0] & 0xEF; dtBinInputs[0] |= digitalRead(cnt4) << 4; raisedIRQ=true; debug("PinChange 5", true);}
void irq5(void){ dtBinInputs[0] = dtBinInputs[0] & 0xDF; dtBinInputs[0] |= digitalRead(cnt5) << 5; raisedIRQ=true; debug("PinChange 6", true);}
void irq6(void){ dtBinInputs[0] = dtBinInputs[0] & 0xBF; dtBinInputs[0] |= digitalRead(cnt6) << 6; raisedIRQ=true; debug("PinChange 7", true);}
void irq7(void){ dtBinInputs[0] = dtBinInputs[0] & 0x7F; dtBinInputs[0] |= digitalRead(cnt7) << 7; raisedIRQ=true; debug("PinChange 8", true);}

/**
 * configPorts
 *
 * Configure binary ports to generate interrupts
 */
void configPorts(void)
{
  // Configure digital pins as inputs with internal pull-up's
  pinMode(cnt0, INPUT_PULLUP);
  pinMode(cnt1, INPUT_PULLUP);
  pinMode(cnt2, INPUT_PULLUP);
  pinMode(cnt3, INPUT_PULLUP);
  pinMode(cnt4, INPUT_PULLUP);
  pinMode(cnt5, INPUT_PULLUP);
  pinMode(cnt6, INPUT_PULLUP);
  pinMode(cnt7, INPUT_PULLUP);

  // workaround für Pin 16/17 bei PCB Change 1 (fehlerhaftes Design)
  pinMode(16, INPUT_PULLUP);
  pinMode(17, INPUT_PULLUP);


  pinMode(LED_R, OUTPUT);
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_G, OUTPUT);
  
  // Attach pins to custom IRQ's
#ifdef PANSTAMP_NRG
  attachInterrupt(cnt0, irq0, CHANGE);
  attachInterrupt(cnt1, irq1, CHANGE);
  attachInterrupt(cnt2, irq2, CHANGE);
  attachInterrupt(cnt3, irq3, CHANGE);
  attachInterrupt(cnt4, irq4, CHANGE);
  attachInterrupt(cnt5, irq5, CHANGE);
  attachInterrupt(cnt6, irq6, CHANGE);
  attachInterrupt(cnt7, irq7, CHANGE);
#else
  attachPCINT(digitalPinToPCINT(cnt0), irq0, CHANGE);
  attachPCINT(digitalPinToPCINT(cnt1), irq1, CHANGE);
  attachPCINT(digitalPinToPCINT(cnt2), irq2, CHANGE);
  attachPCINT(digitalPinToPCINT(cnt3), irq3, CHANGE);
  attachPCINT(digitalPinToPCINT(cnt4), irq4, CHANGE);
  attachPCINT(digitalPinToPCINT(cnt5), irq5, CHANGE);
  attachPCINT(digitalPinToPCINT(cnt6), irq6, CHANGE);
  attachPCINT(digitalPinToPCINT(cnt7), irq7, CHANGE);
#endif
}

/**
 * updateValues
 *
 * Update binary state registers and counters for the first time
*/ 
void updateValues(void)
{
  irq0();
  irq1();
  irq2();
  irq3();
  irq4();
  irq5();
  irq6();
  irq7();
}

/**
 * setup
 *
 * Arduino setup function
 */
void setup()
{
  panstamp.setHighTxPower(); // only for non-battery powered devices
  
  #ifdef DEBUG
  Serial.begin(38400);
  #endif
  
  debug("Start",true);
  // neue Parameter in die Variablen laden
  getVarsFromEeprom();
    
  // Configure ports
  configPorts();
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_Y, LOW);
  digitalWrite(LED_G, LOW);
  
  // Init SWAP stack
  swap.init();

  // Enable repeater mode
  //swap.enableRepeater(maxRepeaterHop);
  
  // Transmit product code
  swap.getRegister(REGI_PRODUCTCODE)->getData();
  delay(100);
  // Enter SYNC state
  // not Low powered!!
  swap.enterSystemState(SYSTATE_RXON);

  // During 3 seconds, listen the network for possible commands whilst the LED blinks
  for(unsigned int i=0 ; i<3 ; i++)
  {
    digitalWrite(LED_R, HIGH); delay(100);
    digitalWrite(LED_R, LOW);  delay(100);
    digitalWrite(LED_Y, HIGH); delay(100);
    digitalWrite(LED_Y, LOW);  delay(100);
    digitalWrite(LED_G, HIGH); delay(100);
    digitalWrite(LED_G, LOW);  
    delay(500);
  }
  // Transmit periodic Tx interval
  swap.getRegister(REGI_TXINTERVAL)->getData();
  delay(100);
  // Transmit Configuration
  swap.getRegister(REGI_CONFIGURATION)->getData();
  delay(100);
  
  // update the values initially into the register
  updateValues();
  
  // Transmit initial binary states
  swap.getRegister(REGI_BININPUTS)->getData();
  delay(100);

  raisedIRQ=false;
}

/**
 * loop
 *
 * Arduino main loop
 */
void loop()
{
  unsigned long currentMillis = millis();
  
  // Sleep until next (RTC) calendar alarm or pin interrupt
  //panstamp.sleep();
  
  // transmit only if IRQ has raised or TXInterval has reached
  if (raisedIRQ || isTimeToTX()) {
    debug("-------------------------------", true);
    debug("Status: ", false);
    debug_bin(dtBinInputs[0], true);  

    if (raisedIRQ) {
      LED_state = 0; // Startzustand
      // Zustand den LEDs zuweisen
      for(unsigned int i=1;i<=8;i++) {
        // gruppe 0 ist default und nicht besetzt
        // feststellen, welcher Status per LED angezeigt werden soll
        LED_state = max(group_state[i], LED_state);
      }
      
      SetLEDState(LED_state, HIGH);
      LED_BlinkOnTime=LED_BlinkOnEvtTime;
      LED_PrevMillis = currentMillis;
    }

    // Transmit binary states if an IRQ has raised
    raisedIRQ=false;
    swap.getRegister(REGI_BININPUTS)->getData();
    last_transmission_time = currentMillis;

    delay(100);
    swap.getRegister(REGI_GROUPSTATES)->getData();

    
  }  

  // LED blinking
  // https://www.arduino.cc/en/Tutorial/BlinkWithoutDelay
  if ((LED_R_State || LED_Y_State || LED_G_State ) && (currentMillis - LED_PrevMillis > LED_BlinkOnTime)) {
    // eine LED ist an, BlinkOnTime ist abgelaufen
    SetLEDState(LED_state, LOW);
    LED_PrevMillis = currentMillis;
  } else if ((!LED_R_State && !LED_Y_State && !LED_G_State ) && (currentMillis - LED_PrevMillis > LED_BlinkOffTime)) {
    // alle LEDs sind aus und BlinkOffTime ist abgelaufen
    SetLEDState(LED_state, HIGH);
    LED_BlinkOnTime=LED_BlinkOnIdleTime;
    LED_PrevMillis = currentMillis;
  }

  if(send_config) {
    //set by setter proc of configuration (SetConfiguration())
    delay(100);
    swap.getRegister(REGI_CONFIGURATION)->getData();
    send_config = false;
  }
  
}
