//- -----------------------------------------------------------------------------------------------------------------------
// AskSin++
// 2021-11-17 papa Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
// ci-test=yes board=328p aes=no
//- -----------------------------------------------------------------------------------------------------------------------

/*
 * Damit der upload mittels ISP funktioniert, muss fuer das jeweilige board in boards.txt die zeile mit ‘upload.protocol=arduino’ auskommentiert werden. 
 * Danach ist die ISP-auswahl im IDE menu auch wirksam.
 * 
 * programmers.txt
 * mySmartUSB.name=mySmartUSB
* mySmartUSB.protocol=stk500v2
* mySmartUSB.communication=serial
* mySmartUSB.program.tool=avrdude
* mySmartUSB.program.extra_params=-P{serial.port}
 * 
 * preferences.txt
 * alt: upload.using=bootloader
 * neu: upload.using=mySmartUSB
 */

// define this to read the device id, serial and device type from bootloader section
// #define USE_OTA_BOOTLOADER

#define EI_NOTEXTERNAL
#include <EnableInterrupt.h>
#include <AskSinPP.h>
#include <LowPower.h>

#include <Register.h>
#include <ContactState.h>

// we use a Pro Mini
// Arduino pin for the LED
// D4 == PIN 4 on Pro Mini
#define LED1_PIN 4 // -> D4 -> Red
#define LED2_PIN 5 // -> D5 -> Green

// Arduino pin for the config button
// B0 == PIN 8 on Pro Mini
#define CONFIG_BUTTON_PIN 8 // -> D8

#define REED01_PIN 9  // PC0 - A0 | K7 -> D9
#define REED02_PIN 7   // PD3 - D3 | K6 -> D7
#define REED03_PIN 6  // PC1 - A1| K9 -> D6
#define REED04_PIN 3   // PD7 - D7 | K8 -> D3
#define REED05_PIN 14   // PB1 - D9 | K3 -> A0
#define REED06_PIN 15   // PD6 - D6 | K2 -> A1
#define REED07_PIN 16  // PC2 - A2 | K5 -> A2
#define REED08_PIN 17  // PC3 - A3 | K4 -> A3

#define SABOTAGE_PIN 0

// number of available peers per channel
#define PEERS_PER_CHANNEL 10

// all library classes are placed in the namespace 'as'
using namespace as;

// define all device properties
const struct DeviceInfo PROGMEM devinfo = {
    {0xf2,0x0e,0x00},       // Device ID
    "papaf20e00",           // Device Serial
    {0xF2,0x0E},            // Device Model -> new device HB-SEC-RHS-x4
    0x10,                   // Firmware Version
    as::DeviceType::ThreeStateSensor, // Device Type
    {0x01,0x00}             // Info Bytes
};

/**
 * Configure the used hardware
 */
typedef AvrSPI<10,11,12,13> SPIType;
typedef Radio<SPIType,2> RadioType;
typedef DualStatusLed<LED2_PIN,LED1_PIN> LedType;
typedef AskSin<LedType,NoBattery,RadioType> Hal;
Hal hal;

DEFREGISTER(Reg0,DREG_CYCLICINFOMSG,MASTERID_REGS,DREG_TRANSMITTRYMAX,DREG_SABOTAGEMSG)
class RHSList0 : public RegList0<Reg0> {
public:
  RHSList0(uint16_t addr) : RegList0<Reg0>(addr) {}
  void defaults () {
    clear();
    cycleInfoMsg(true);
    transmitDevTryMax(6);
    sabotageMsg(true);
  }
};

DEFREGISTER(Reg1,CREG_AES_ACTIVE,CREG_MSGFORPOS,CREG_EVENTDELAYTIME,CREG_LEDONTIME,CREG_TRANSMITTRYMAX)
class RHSList1 : public RegList1<Reg1> {
public:
  RHSList1 (uint16_t addr) : RegList1<Reg1>(addr) {}
  void defaults () {
    clear();
    msgForPosA(1); // CLOSED
    msgForPosB(2); // OPEN
    msgForPosC(3); // TILTED
    // aesActive(false);
    // eventDelaytime(0);
    ledOntime(100);
    transmitTryMax(6);
  }
};

typedef ThreeStateChannel<Hal,RHSList0,RHSList1,DefList4,PEERS_PER_CHANNEL> ChannelType;
typedef ThreeStateDevice<Hal,ChannelType,4,RHSList0> RHSType;

RHSType sdev(devinfo,0x20);
ConfigButton<RHSType> cfgBtn(sdev);

// Map Reed Contact States to Position      00                    01                    10                    11
//const uint8_t posmap[4] = {Position::State::PosB,Position::State::PosC,Position::State::PosC,Position::State::PosA};
const uint8_t posmap[4] = {Position::State::PosA,Position::State::PosC,Position::State::PosC,Position::State::PosB};
void setup () {
  DINIT(57600,ASKSIN_PLUS_PLUS_IDENTIFIER);
  sdev.init(hal);
  buttonISR(cfgBtn,CONFIG_BUTTON_PIN);
  sdev.channel(1).init(REED01_PIN,REED02_PIN,SABOTAGE_PIN,posmap);
  sdev.channel(2).init(REED03_PIN,REED04_PIN,SABOTAGE_PIN,posmap);
  sdev.channel(3).init(REED05_PIN,REED06_PIN,SABOTAGE_PIN,posmap);
  sdev.channel(4).init(REED07_PIN,REED08_PIN,SABOTAGE_PIN,posmap);
  sdev.initDone();
}

void loop() {
  bool worked = hal.runready();
  bool poll = sdev.pollRadio();
  if( worked == false && poll == false ) {
    // if nothing to do - go idle
    hal.idle<>();
  }
}
