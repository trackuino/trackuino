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

/* Credit to:
 *
 * Michael Smith for his Example of Audio generation with two timers and PWM:
 * http://www.arduino.cc/playground/Code/PCMAudio
 *
 * Ken Shirriff for his Great article on PWM:
 * http://arcfn.com/2009/07/secrets-of-arduino-pwm.html 
 *
 * The large group of people who created the free AVR tools.
 * Documentation on interrupts:
 * http://www.nongnu.org/avr-libc/user-manual/group__avr__interrupts.html
 */

#include "config.h"
#include "afsk_avr.h"
#include "afsk_pic32.h"
#include "pin.h"
#include "radio_hx1.h"
#include <WProgram.h>
#include <stdint.h>

// Module consts
static const uint16_t BAUD_RATE       = 1200;
static const uint8_t SAMPLES_PER_BAUD = (PLAYBACK_RATE / BAUD_RATE);
static const uint16_t PHASE_DELTA_1200 = (((TABLE_SIZE * 1200L) << 7) / PLAYBACK_RATE); // Fixed point 9.7
static const uint16_t PHASE_DELTA_2200 = (((TABLE_SIZE * 2200L) << 7) / PLAYBACK_RATE);


// Module globals
volatile static unsigned char current_byte;
volatile static unsigned char current_sample_in_baud;    // 1 bit = SAMPLES_PER_BAUD samples
volatile static bool go = false;                         // Modem is on
volatile static unsigned int phase_delta;                // 1200/2200 for standard AX.25
volatile static unsigned int phase;                      // Fixed point 9.7 (2PI = TABLE_SIZE)
volatile static unsigned int packet_pos;                 // Next bit to be sent out
#ifdef DEBUG_MODEM
volatile static int overruns = 0;
volatile static uint32_t isr_calls = 0;
volatile static uint32_t avg_isr_time = 0;
volatile static uint16_t fast_isr_time = 65535;
volatile static uint16_t slow_isr_time = 0;
volatile static unsigned int slow_packet_pos;
volatile static unsigned char slow_sample_in_baud;
#endif

// The radio (class defined in config.h)
static RadioHx1 radio;

volatile static unsigned int afsk_packet_size = 0;
volatile static const uint8_t *afsk_packet;

// Exported functions

void afsk_setup()
{
  // Start radio
  radio.setup();
}

void afsk_send(const uint8_t *buffer, int len)
{
  afsk_packet_size = len;
  afsk_packet = buffer;
}

int
afsk_busy()
{
  return go;
}

void afsk_start()
{
  phase_delta = PHASE_DELTA_1200;
  phase = 0;
  packet_pos = 0;
  current_sample_in_baud = 0;
  go = true;

  // Start timer (CPU-specific)
  afsk_timer_setup();
  
  // Key the radio
  radio.ptt_on();

  // Start transmission
  afsk_timer_start();
}

// This is called at PLAYBACK_RATE Hz to load the next sample.
void afsk_isr()
{
  if (go) {

    // If done sending packet
    if (packet_pos == afsk_packet_size) {
      go = false;         // End of transmission
      afsk_timer_stop();  // Disable modem
      radio.ptt_off();    // Release PTT
      goto end_isr;       // Done, gather ISR stats
    }

    // If sent SAMPLES_PER_BAUD already, go to the next bit
    if (current_sample_in_baud == 0) {    // Load up next bit
      if ((packet_pos & 7) == 0)          // Load up next byte
        current_byte = afsk_packet[packet_pos >> 3];
      else
        current_byte = current_byte / 2;  // ">>1" forces int conversion
      if ((current_byte & 1) == 0) {
        // Toggle tone (1200 <> 2200)
        phase_delta ^= (PHASE_DELTA_1200 ^ PHASE_DELTA_2200);
      }
    }
    
    phase += phase_delta;
    uint8_t s = afsk_read_sample((phase >> 7) & (TABLE_SIZE - 1));
    afsk_output_sample(s);

    if(++current_sample_in_baud == SAMPLES_PER_BAUD) {
      current_sample_in_baud = 0;
      packet_pos++;
    }
  }
 
end_isr:
#ifdef DEBUG_MODEM
  // Track overruns
  if (afsk_isr_overrun())
    overruns++;

  isr_calls++;
  uint16_t t = afsk_timer_counter();

  // Track slowest execution time in slow_isr_time
  if (t > slow_isr_time) {
    slow_isr_time = t;
    slow_packet_pos = packet_pos;
    slow_sample_in_baud = current_sample_in_baud;
  }
  
  // Track fastest execution time
  if (t < fast_isr_time) {
    fast_isr_time = t;
  }
  
  // Track average time
  avg_isr_time += t;
#endif

  return;

}

#ifdef DEBUG_MODEM
void afsk_debug()
{
  Serial.print("t(fast,avg,slow)=");
  Serial.print(fast_isr_time);
  Serial.print(",");
  if (isr_calls)
    Serial.print(avg_isr_time / isr_calls);
  else
    Serial.print("INF");
  Serial.print(",");
  Serial.print(slow_isr_time);
  Serial.print("; pos=");
  Serial.print(slow_packet_pos);
  Serial.print("/");
  Serial.print(afsk_packet_size);
  Serial.print("; sam=");
  Serial.print(slow_sample_in_baud, DEC);
  Serial.print("; overruns/isr=");
  Serial.print(overruns);
  Serial.print("/");
  Serial.println(isr_calls);

  isr_calls = 0;
  avg_isr_time = 0;
  slow_isr_time = 0;
  fast_isr_time = 65535;
  overruns = 0;
}
#endif
