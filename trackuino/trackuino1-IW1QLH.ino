/* Trackuino copyright (C) 2010  EA5HAV Javi
 Modified to work with DRA818V
*/
#include <SoftwareSerial.h>

// Mpide 22 fails to compile Arduino code because it stupidly defines ARDUINO 
// as an empty macro (hence the +0 hack). UNO32 builds are fine. Just use the
// real Arduino IDE for Arduino builds. Optionally complain to the Mpide
// authors to fix the broken macro.
#if (ARDUINO + 0) == 0
#error "Oops! We need the real Arduino IDE (version 22 or 23) for Arduino builds."
#error "See trackuino.pde for details on this"

// Refuse to compile on arduino version 21 or lower. 22 includes an 
// optimization of the USART code that is critical for real-time operation
// of the AVR code.
#elif (ARDUINO + 0) < 22
#error "Oops! We need Arduino 22 or 23"
#error "See trackuino.pde for details on this"

#endif

// Trackuino custom libs
#include "config.h"
#include "afsk_avr.h"
#include "afsk_pic32.h"
#include "aprs.h"
#include "gps.h"
#include "pin.h"
#include "power.h"
#include "sensors_avr.h"
#include "sensors_pic32.h"

// Arduino/AVR libs
#if (ARDUINO + 1) >= 100
#  include <Arduino.h>
#else
#  include <WProgram.h>
#endif

// Module constants
static const uint32_t VALID_POS_TIMEOUT = 2000;  // ms

// Module variables
static int32_t next_aprs = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define PwDw   11  // Power Down pin. This need to start low, then be set high before programming.
#define DRA_RXD 7   // The Arduino IO line that is connected to the DRA818's TXD pin. 3.3V only
#define DRA_TXD 12   // The Arduino IO line connected to the DRA818's RXD pin.
SoftwareSerial dra_serial(DRA_RXD, DRA_TXD);    //for DRA818
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define gps_power      2
#define DRA818_SLEEP digitalWrite(PwDw, LOW)
#define DRA818_NORMAL digitalWrite(PwDw, HIGH)
void setup()
{
  pinMode(gps_power, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(PwDw, OUTPUT);
  pin_write(LED_PIN, LOW);
  
  DRA818_SLEEP;
  digitalWrite(gps_power, HIGH);
  
  pin_write(LED_PIN, LOW);
  dra_serial.begin(9600);
  Serial.begin(GPS_BAUDRATE);
#ifdef DEBUG_RESET
  Serial.println("RESET");
#endif

  afsk_setup();
  gps_setup();
  sensors_setup();

#ifdef IW1QLH_DEBUG

  char ch;
  char response[20];
  int resp_idx = 0;
  bool ok = false;
  while (!ok) {
    
    if ((int32_t) (millis() - next_aprs) >= 0) {
      next_aprs = millis() + 3000;
      DRA818_NORMAL;
      delay(3000);
      Serial.println("testing DRA818 reply");
      dra_serial.println(F("AT+DMOCONNECT"));
    }

    while (dra_serial.available()) {
      ch = dra_serial.read();
      Serial.print(ch);
      if ((ch == '\n') || (ch == '\r')) {
        ok = (strcmp(response, "+DMOCONNECT:0")== 0);
        resp_idx = 0;
      }
      else
      {
        response[resp_idx++] = ch;
        response[resp_idx] = 0;
        if (resp_idx >= 20)
          resp_idx = 0;
      }
    }
  }
  Serial.println("DRA818 OK");
  DRA818_SLEEP;

  Serial.println("wait slot");
#endif

 // Do not start until we get a valid time reference
  // for slotted transmissions.
  if (APRS_SLOT >= 0) {
    do {
      while (! Serial.available())
        power_save();
    } while (! gps_decode(Serial.read()));
    
    next_aprs = millis() + 1000 *
      (APRS_PERIOD - (gps_seconds + APRS_PERIOD - APRS_SLOT) % APRS_PERIOD);
  }
  else {
    next_aprs = millis();
  }  
  // TODO: beep while we get a fix, maybe indicating the number of
  // visible satellites by a series of short beeps?

#ifdef IW1QLH_DEBUG
  Serial.println("end setup");
#endif
}

void get_pos()
{
  // Get a valid position from the GPS
  int valid_pos = 0;
  uint32_t timeout = millis();
  do {
    if (Serial.available())
      valid_pos = gps_decode(Serial.read());
  } while ( (millis() - timeout < VALID_POS_TIMEOUT) && ! valid_pos) ;
/*
  if (valid_pos) 
  {
    if (gps_altitude > BUZZER_ALTITUDE) {
      buzzer_off();   // In space, no one can hear you buzz
    } else {
      buzzer_on();
    }
  }
  */
}

#ifdef IW1QLH_DEBUG
  int ex_delta;
  int delta;
#endif

void loop()
{
  bool ok;
  char ch;
  
#ifdef IW1QLH_DEBUG
  delta = (next_aprs - millis()) / 1000;
  if (delta != ex_delta) {
    ex_delta = delta;
    Serial.println(delta);
  }
#endif
  // Time for another APRS frame
  if ((int32_t) (millis() - next_aprs) >= 0) {
    get_pos();

    DRA818_NORMAL;
    delay(3000); //init test
    
    dra_serial.println(F("AT+DMOSETGROUP=0,144.8000,144.8000,0000,4,0000")); //Send config data
    uint32_t timeout = millis() + 3000;
    ok = false;
    while (!ok) {
      if (dra_serial.available()) {
        ch = dra_serial.read();
        Serial.print(char(ch));
        ok = ((ch == '\n') || (ch == '\r'));
      }
      if (millis() > timeout)
        break;
    }
    digitalWrite(PTT_PIN, HIGH); //low TX high RX // drive 2N7002 to GND to force PTT 4 pin to LOW
   
#ifdef IW1QLH_DEBUG
    Serial.println();
    Serial.print(millis());
    Serial.println(" PTT ON");
#endif
    aprs_send();
   
    next_aprs += APRS_PERIOD * 1000L;
    while (afsk_flush()) {
      power_save();
    }

#ifdef IW1QLH_DEBUG
    Serial.print(millis());
    Serial.println(" PTT OFF");
#endif
    DRA818_SLEEP;
    digitalWrite(PTT_PIN, LOW);

#ifdef DEBUG_MODEM
    // Show modem ISR stats from the previous transmission
    afsk_debug();
#endif

  } else {
    // Discard GPS data received during sleep window
    while (Serial.available()) {
      Serial.read();
    }
    if (dra_serial.available()) {
      Serial.print(char(dra_serial.read()));
    }
  }

  power_save(); // Incoming GPS data or interrupts will wake us up
}
