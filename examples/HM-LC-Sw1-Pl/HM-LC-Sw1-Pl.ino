//- -----------------------------------------------------------------------------------------------------------------------
// AskSin++
// 2017-08-31 papa Creative Commons - http://creativecommons.org/licenses/by-nc-sa/3.0/de/
//- -----------------------------------------------------------------------------------------------------------------------

// define this to read the device id, serial and device type from bootloader section
// #define USE_OTA_BOOTLOADER

// This firmware converts a MAX! into a Homematic
#error "not working"
#define HM_LC_SW1_PL 0x00,0x11

// define all device properties
#define DEVICE_ID HMID(0x34,0x17,0x30)
#define DEVICE_SERIAL "HMax000000"
#define DEVICE_MODEL  HM_LC_SW1_PL
#define DEVICE_FIRMWARE 0x16
#define DEVICE_TYPE DeviceType::Switch
#define DEVICE_INFO 0x01,0x01,0x00
#define DEVICE_CONFIG

//#define PINA _SFR_IO8(0x19)
//#define DDRA _SFR_IO8(0x1A)
//#define PORTA _SFR_IO8(0x1B)

// #include <EnableInterrupt.h>
#include <AskSinPP.h>
#include <TimerOne.h>
//#include <LowPower.h>

#include <MultiChannelDevice.h>
#include <SwitchChannel.h>


#define CONFIG_BUTTON_PIN PA0 // ????
#define RELAY1_PIN 8 // PB0

// number of available peers per channel
#define PEERS_PER_CHANNEL 16


// all library classes are placed in the namespace 'as'
using namespace as;

/**
 * Configure the used hardware
 */
typedef AvrSPI<PB4,PB5,PB6,PB7,PortB> RadioSPI;
typedef StatusLed<PD4,PortD> LedType;
typedef AskSin<LedType,NoBattery,Radio<RadioSPI,3> > Hal;

//typedef AvrSPI<PB2,PB3,PB4,PB5,PortB> RadioSPI;
//typedef StatusLed<PD4,PortD> LedType;
//typedef AskSin<LedType,NoBattery,Radio<RadioSPI,2> > Hal;

// setup the device with channel type and number of channels
typedef MultiChannelDevice<Hal,SwitchChannel<Hal,PEERS_PER_CHANNEL>,1> SwitchType;

Hal hal;
SwitchType sdev(0x20);
ConfigToggleButton<SwitchType> cfgBtn(sdev);

// map number of channel to pin
// this will be called by the SwitchChannel class
uint8_t SwitchPin (__attribute__((unused)) uint8_t number) {
  return RELAY1_PIN;
}


void setup () {
  DINIT(57600,ASKSIN_PLUS_PLUS_IDENTIFIER);
  sdev.init(hal);

  for( uint8_t i=1; i<=sdev.channels(); ++i ) {
    sdev.channel(i).lowactive(false);
  }

//  buttonISR(cfgBtn,CONFIG_BUTTON_PIN);

  // create internal peerings - CCU2 needs this
  HMID devid;
  sdev.getDeviceID(devid);
  for( uint8_t i=1; i<=sdev.channels(); ++i ) {
    Peer ipeer(devid,i);
    // create internal peer if not already done
    uint8_t idx = 0; // make compiler happy
    if( sdev.channel(i).peer(idx) != ipeer ) {
      sdev.channel(i).peer(ipeer);
    }
  }
  // delay next send by random time
  hal.waitTimeout((rand() % 3500)+1000);
}

void loop() {
  bool worked = hal.runready();
  bool poll = sdev.pollRadio();
  if( worked == false && poll == false ) {
//    hal.activity.savePower<Idle<>>(hal);
  }
}
