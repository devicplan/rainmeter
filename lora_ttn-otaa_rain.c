/*******************************************************************************
 * Jens Dietrich - Hoyerswerda - 05.11.2022 Rainmeter 
 * Durchmesser 11,3cm entspricht 0,01m“ / Diameter 11,3cm corresponds to 0,01m
 * 100ml erzeugen 48 Impulse = 1cm Wasserhöhe entspricht 48 Impulse (0,21mm / 0.21 Liter je Impuls) /
 * 100ml generate 48 pulses = 1cm water height corresponds to 48 pulses (0.21mm / 0.21 Liter per pulse)
 * Clockerror = https://www.thethingsnetwork.org/forum/t/using-lmic-setclockerror-on-mcci-lmic-howto/39776
 * 
 * Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman 
 * Copyright (c) 2018 Terry Moore, MCCI
 *
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 *
 * This example sends a valid LoRaWAN packet with payload "Hello,
 * world!", using frequency and encryption settings matching those of
 * the The Things Network.
 *
 * This uses OTAA (Over-the-air activation), where where a DevEUI and
 * application key is configured, which are used in an over-the-air
 * activation procedure where a DevAddr and session keys are
 * assigned/generated for use with all further communication.
 *
 * Note: LoRaWAN per sub-band duty-cycle limitation is enforced (1% in
 * g1, 0.1% in g2), but not the TTN fair usage policy (which is probably
 * violated by this sketch when left running for longer)!

 * To use this sketch, first register your application and device with
 * the things network, to set or generate an AppEUI, DevEUI and AppKey.
 * Multiple devices can use the same AppEUI, but each device has its own
 * DevEUI and AppKey.
 *
 * Do not forget to define the radio type correctly in
 * arduino-lmic/project_config/lmic_project_config.h or from your BOARDS.txt.
 * 
  *******************************************************************************/

#include <lmic.h>                                               // MCCI LoRaWAN LMIC library V 4.1.1 (11/2022)
#include <hal/hal.h>
#include <SPI.h>
#include <Wire.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

#define debug                                                   // comment out for debug output

unsigned long raintotal = 0;                                    // sum rain
uint16_t rain = 0;                                              // current value rain
uint8_t rain_interrupt = 0;

#define LED         4                                           // led - unused
#define REED        3                                           // reed contact against ground

//
// For normal use, we require that you edit the sketch to replace FILLMEIN
// with values assigned by the TTN console. However, for regression tests,
// we want to be able to compile these scripts. The regression tests define
// COMPILE_REGRESSION_TEST, and in that case we define FILLMEIN to a non-
// working but innocuous value.
//
#ifdef COMPILE_REGRESSION_TEST
# define FILLMEIN 0
#else
# warning "You must replace the values marked FILLMEIN with real values from the TTN control panel!"
# define FILLMEIN (#dont edit this, edit the lines that use FILLMEIN)
#endif

// This EUI must be in little-endian format, so least-significant-byte
static const u1_t PROGMEM APPEUI[8]={0x01, 0x20, 0x0A0, 0x04, 0x11, 0x01, 0x89, 0xA0};
void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8);}

// This should also be in little endian format, see above.
static const u1_t PROGMEM DEVEUI[8]={0xB2, 0x73, 0x25, 0xD0, 0x8E, 0xD5, 0xC3, 0x70};
void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8);}

// This key should be in big endian format (or, since it is not really a
static const u1_t PROGMEM APPKEY[16] = {0xE8, 0x5A, 0xA5, 0x8F, 0xA7, 0x18, 0x98, 0xC6, 0xF6, 0xB2, 0xBA, 0x3B, 0x10, 0xC1, 0xBE, 0xCF};
void os_getDevKey (u1_t* buf) {  memcpy_P(buf, APPKEY, 16);}

//static uint8_t mydata[] = "Hello, world!";
static uint8_t mydata[] = "000000";
static osjob_t sendjob;
const unsigned TX_INTERVAL = 8400;                           // send interval in sec, in between everything sleeps (8400 about 180 minutes)

const lmic_pinmap lmic_pins = {
    .nss = 10,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 9,
    .dio = {2, 5, LMIC_UNUSED_PIN},
};

#ifdef debug
void printHex2(unsigned v) {
    v &= 0xff;
    if (v < 16)
        Serial.print('0');
    Serial.print(v, HEX);
}
#endif

void onEvent (ev_t ev) {
    switch(ev) {
        case EV_SCAN_TIMEOUT:
            Serial.println(F("F1"));
            break;
        case EV_BEACON_FOUND:
            Serial.println(F("F2"));
            break;
        case EV_BEACON_MISSED:
            Serial.println(F("F3"));
            break;
        case EV_BEACON_TRACKED:
            Serial.println(F("F4"));
            break;
        case EV_JOINING:
            Serial.println(F("J-ING"));
            break;
        case EV_JOINED:
            Serial.println(F("J-NED"));
#ifdef debug
            {
              u4_t netid = 0;
              devaddr_t devaddr = 0;
              u1_t nwkKey[16];
              u1_t artKey[16];
              LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
              Serial.print(F("netid: "));
              Serial.println(netid, DEC);
              Serial.print(F("devaddr: "));
              Serial.println(devaddr, HEX);
              Serial.print(F("AppSKey: "));
              for (size_t i=0; i<sizeof(artKey); ++i) {
                if (i != 0)
                  Serial.print("-");
                printHex2(artKey[i]);
              }
              Serial.println("");
              Serial.print(F("NwkSKey: "));
              for (size_t i=0; i<sizeof(nwkKey); ++i) {
                      if (i != 0)
                              Serial.print("-");
                      printHex2(nwkKey[i]);
              }
              Serial.println();
            }
#endif
            // Disable link check validation (automatically enabled
            // during join, but because slow data rates change max TX
	    // size, we don't use it in this example.
            LMIC_setLinkCheckMode(0);
            break;
        /*
        || This event is defined but not used in the code. No
        || point in wasting codespace on it.
        ||
        || case EV_RFU1:
        ||     Serial.println(F("EV_RFU1"));
        ||     break;
        */
        case EV_JOIN_FAILED:
            Serial.println(F("F5"));
            break;
        case EV_REJOIN_FAILED:
            Serial.println(F("F6"));
            break;
        case EV_TXCOMPLETE:
            Serial.println(F("F7"));
            if (LMIC.txrxFlags & TXRX_ACK)
              Serial.println(F("R ack"));
            if (LMIC.dataLen) {                                                           // rx data evaluation
              Serial.print(F("Received "));
              Serial.print(LMIC.dataLen);
              Serial.print(F(" bytes of payload: 0x"));

              for (int i = 0; i < LMIC.dataLen; i++) {
                if (LMIC.frame[LMIC.dataBeg + i] < 0x10) {
                  Serial.print(F("0"));
                }
                Serial.print(LMIC.frame[LMIC.dataBeg + i], HEX);
                if ((i == 0) && (LMIC.frame[LMIC.dataBeg + i] != 0))  {
                  digitalWrite(LED, HIGH);                                                // led on (led not equipped and used)
                }
                else  {
                  digitalWrite(LED, LOW);                                                 // led off 
                }
              }
              Serial.println();
            }
                         
            delay(500);
            for(uint16_t t=0;t<(TX_INTERVAL/8);t++)  {                                    // interval see above
              if(rain_interrupt)  {
                digitalWrite(LED, HIGH);
                delay(100);
                digitalWrite(LED, LOW);
                if (t<((TX_INTERVAL/8)-(240/8)))  {                                       // reduce transmission pause in case of rain
                  t=(TX_INTERVAL/8)-(240/8);                                              // 240 = 4-5 Minutes
                }
                rain_interrupt = 0;
                rain++;
                raintotal++;
              }
              startSleeping();
            }
            delay(500);
            os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(1), do_send);
            break;
        case EV_LOST_TSYNC:
            Serial.println(F("F8"));
            break;
        case EV_RESET:
            Serial.println(F("F9"));
            break;
        case EV_RXCOMPLETE:
            // data received in ping slot
            Serial.println(F("F10"));
            break;
        case EV_LINK_DEAD:
            Serial.println(F("F11"));
            break;
        case EV_LINK_ALIVE:
            Serial.println(F("F12"));
            break;
        /*
        || This event is defined but not used in the code. No
        || point in wasting codespace on it.
        ||
        || case EV_SCAN_FOUND:
        ||    Serial.println(F("EV_SCAN_FOUND"));
        ||    break;
        */
        case EV_TXSTART:
            Serial.println(F("F13"));
            break;
        case EV_TXCANCELED:
            Serial.println(F("F14"));
            break;
        case EV_RXSTART:
            /* do not print anything -- it wrecks timing */
            break;
        case EV_JOIN_TXCOMPLETE:
            Serial.println(F("F15"));
            break;

        default:
            Serial.print(F("F16: "));
            Serial.println((unsigned) ev);
            break;
    }
}

void do_send(osjob_t* j){
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("F17"));
    } else {
        mydata[0] = (rain & 0xFF00)>>8;                         // rain
        mydata[1] = (rain & 0x00FF);
        mydata[2] = (raintotal & 0xFF000000)>>24;               // rain sum
        mydata[3] = (raintotal & 0x00FF0000)>>16;
        mydata[4] = (raintotal & 0x0000FF00)>>8;
        mydata[5] = (raintotal & 0x000000FF);
        rain = 0;                                               // after send back to zero
        LMIC_setTxData2(1, mydata, 6, 0);        

        Serial.println(F("P queued"));
    }
}

void int_reedswitch()  {
    rain_interrupt++;
}

void setup() {
    pinMode(LED, OUTPUT);                                       // output pin d3
    digitalWrite(LED, LOW);                                     // led off
    
    pinMode(REED, INPUT_PULLUP);                                // reedswitch on pin d4
    attachInterrupt(digitalPinToInterrupt(REED),int_reedswitch, FALLING);
    
    Serial.begin(9600);
    delay(2000);
    Serial.println(F("Start"));
    os_init();
    LMIC_reset();
    do_send(&sendjob);
}

void loop() {
    os_runloop_once();
}

ISR (WDT_vect) {
   wdt_disable(); 
}

void startSleeping() {
    // clear various "reset" flags
    MCUSR = 0;                                                  // allow changes, disable reset, enable Watchdog interrupt
    WDTCSR = bit (WDCE) | bit (WDE);                            // set interval (see datasheet p55)
    WDTCSR = bit (WDIE) | bit (WDP3) | bit (WDP0);              // set WDIE, and 8 seconds delay
    //WDTCSR = bit (WDIE) | bit (WDP2) | bit (WDP1);            // same with 1 second
    wdt_reset();                                                // start watchdog timer
    set_sleep_mode (SLEEP_MODE_PWR_DOWN);                       // prepare for powerdown  
    sleep_enable();  
    sleep_cpu ();                                               // power down !
}
