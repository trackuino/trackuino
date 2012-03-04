/* trackuino copyright (C) 2010  EA5HAV Javi
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

// Mpide 22 fails to compile Arduino code because it stupidly defines ARDUINO 
// as an empty macro (hence the +0 hack). UNO32 builds are fine. Just use the
// real Arduino IDE for Arduino builds. Optionally complain to the Mpide
// authors to fix the broken macro.
#if (ARDUINO + 0) == 0
#error "Oops! We need the real Arduino IDE (version 22+) for Arduino builds."
#error "See trackuino.pde for details on this"

// Refuse to compile on arduino version 21 or lower. 22 includes an 
// optimization of the USART code that is critical for real-time operation
// of the AVR code.
#elif (ARDUINO + 0) < 22
#error "Oops! We need Arduino 22"
#error "See trackuino.pde for details on this"
#endif



// Trackuino custom libs
#include "aprs.h"
#include "ax25.h"
#include "buzzer_avr.h"
#include "buzzer_pic32.h"
#include "config.h"
#include "gps.h"
#include "modem.h"
#include "pin_pic32.h"
#include "power_avr.h"
#include "power_pic32.h"
#include "radio.h"
#include "radio_hx1.h"
#include "radio_mx146.h"
#include "sensors_avr.h"
#include "sensors_pic32.h"

// Arduino/AVR libs
#include <Wire.h>
#include <WProgram.h>

uint32_t next_tx_millis;


void setup()
{
  pinMode(LED_PIN, OUTPUT);
  pin_write(LED_PIN, LOW);

  Serial.begin(GPS_BAUDRATE);
#ifdef DEBUG_RESET
  Serial.println("RESET");
#endif
  modem_setup();
  buzzer_setup();
  sensors_setup();
  gps_setup();

  // Schedule the next transmission within APRS_DELAY ms
  next_tx_millis = millis() + APRS_DELAY;
  
  // TODO: alternate beeping while gps gets a fix
  //buzzer_on();
}

#define read_count(dest) __asm__ __volatile__("mfc0 %0,$9" : "=r" (dest))
#define read_comp(dest) __asm__ __volatile__("mfc0 %0,$11" : "=r" (dest))
#define read_exc(dest) __asm__ __volatile__("mfc0 %0,$13" : "=r" (dest))
#define read_status(dest) __asm__ __volatile__("mfc0 %0,$12" : "=r" (dest))

void loop()
{
  int c;

  if ((int32_t)(millis() - next_tx_millis) >= 0) {

    // Show modem ISR stats from the previous transmission
#ifdef DEBUG_MODEM
    modem_debug();
#endif

    // Build and send a new aprs frame
    aprs_send();
    next_tx_millis += APRS_PERIOD;
  }

  if (Serial.available()) {
    c = Serial.read();
    if (gps_decode(c)) {
/*
      if (gps_altitude > BUZZER_ALTITUDE)
        buzzer_off();   // In space, no one can hear you buzz
      else
        buzzer_on();
*/
    }
  } else {
    power_save();
  }
}
