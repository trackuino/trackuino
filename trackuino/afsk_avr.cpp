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
#ifdef AVR

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "config.h"
#include "afsk_avr.h"


// Module consts

/* The sine_table is the carrier signal. To achieve phase continuity, each tone
 * starts at the index where the previous one left off. By changing the stride of
 * the index (phase_delta) we get 1200 or 2200 Hz. The PHASE_DELTA_XXXX values
 * can be calculated as:
 * 
 * Fg = frequency of the output tone (1200 or 2200)
 * Fm = sampling rate (PLAYBACK_RATE_HZ)
 * Tt = sine table size (TABLE_SIZE)
 * 
 * PHASE_DELTA_Fg = Tt*(Fg/Fm)
 */

// This procudes a "warning: only initialized variables can be placed into
// program memory area", which can be safely ignored:
// http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734
extern const uint8_t afsk_sine_table[512] PROGMEM = {
  127, 129, 130, 132, 133, 135, 136, 138, 139, 141, 143, 144, 146, 147, 149, 150, 152, 153, 155, 156, 158, 
  159, 161, 163, 164, 166, 167, 168, 170, 171, 173, 174, 176, 177, 179, 180, 182, 183, 184, 186, 187, 188, 
  190, 191, 193, 194, 195, 197, 198, 199, 200, 202, 203, 204, 205, 207, 208, 209, 210, 211, 213, 214, 215, 
  216, 217, 218, 219, 220, 221, 223, 224, 225, 226, 227, 228, 228, 229, 230, 231, 232, 233, 234, 235, 236, 
  236, 237, 238, 239, 239, 240, 241, 242, 242, 243, 244, 244, 245, 245, 246, 247, 247, 248, 248, 249, 249, 
  249, 250, 250, 251, 251, 251, 252, 252, 252, 253, 253, 253, 253, 254, 254, 254, 254, 254, 254, 254, 254, 
  254, 254, 255, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 253, 253, 253, 253, 252, 252, 252, 251, 
  251, 251, 250, 250, 249, 249, 249, 248, 248, 247, 247, 246, 245, 245, 244, 244, 243, 242, 242, 241, 240, 
  239, 239, 238, 237, 236, 236, 235, 234, 233, 232, 231, 230, 229, 228, 228, 227, 226, 225, 224, 223, 221, 
  220, 219, 218, 217, 216, 215, 214, 213, 211, 210, 209, 208, 207, 205, 204, 203, 202, 200, 199, 198, 197, 
  195, 194, 193, 191, 190, 188, 187, 186, 184, 183, 182, 180, 179, 177, 176, 174, 173, 171, 170, 168, 167, 
  166, 164, 163, 161, 159, 158, 156, 155, 153, 152, 150, 149, 147, 146, 144, 143, 141, 139, 138, 136, 135, 
  133, 132, 130, 129, 127, 125, 124, 122, 121, 119, 118, 116, 115, 113, 111, 110, 108, 107, 105, 104, 102, 
  101,  99,  98,  96,  95,  93,  91,  90,  88,  87,  86,  84,  83,  81,  80,  78,  77,  75,  74,  72,  71, 
   70,  68,  67,  66,  64,  63,  61,  60,  59,  57,  56,  55,  54,  52,  51,  50,  49,  47,  46,  45,  44, 
   43,  41,  40,  39,  38,  37,  36,  35,  34,  33,  31,  30,  29,  28,  27,  26,  26,  25,  24,  23,  22, 
   21,  20,  19,  18,  18,  17,  16,  15,  15,  14,  13,  12,  12,  11,  10,  10,   9,   9,   8,   7,   7, 
    6,   6,   5,   5,   5,   4,   4,   3,   3,   3,   2,   2,   2,   1,   1,   1,   1,   0,   0,   0,   0, 
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   1,   1,   1, 
    2,   2,   2,   3,   3,   3,   4,   4,   5,   5,   5,   6,   6,   7,   7,   8,   9,   9,  10,  10,  11, 
   12,  12,  13,  14,  15,  15,  16,  17,  18,  18,  19,  20,  21,  22,  23,  24,  25,  26,  26,  27,  28, 
   29,  30,  31,  33,  34,  35,  36,  37,  38,  39,  40,  41,  43,  44,  45,  46,  47,  49,  50,  51,  52, 
   54,  55,  56,  57,  59,  60,  61,  63,  64,  66,  67,  68,  70,  71,  72,  74,  75,  77,  78,  80,  81, 
   83,  84,  86,  87,  88,  90,  91,  93,  95,  96,  98,  99, 101, 102, 104, 105, 107, 108, 110, 111, 113, 
  115, 116, 118, 119, 121, 122, 124, 125
};

// External consts

extern const uint32_t MODEM_CLOCK_RATE = F_CPU; // 16 MHz
extern const uint8_t REST_DUTY         = 127;
extern const uint16_t TABLE_SIZE       = sizeof(afsk_sine_table);
//extern const uint32_t PLAYBACK_RATE    = MODEM_CLOCK_RATE / 510;  // Phase correct PWM
extern const uint32_t PLAYBACK_RATE    = MODEM_CLOCK_RATE / 256;  // Fast PWM


// Exported functions

void afsk_timer_setup()
{
  // Set up Timer 2 to do pulse width modulation on the speaker
  // pin.
  
  // Source timer2 from clkIO (datasheet p.164)
  ASSR &= ~(_BV(EXCLK) | _BV(AS2));
  
  // Set fast PWM mode with TOP = 0xff: WGM22:0 = 3  (p.150)
  // This allows 256 cycles per sample and gives 16M/256 = 62.5 KHz PWM rate
  
  TCCR2A |= _BV(WGM21) | _BV(WGM20);
  TCCR2B &= ~_BV(WGM22);
  
  // Phase correct PWM with top = 0xff: WGM22:0 = 1 (p.152 and p.160))
  // This allows 510 cycles per sample and gives 16M/510 = ~31.4 KHz PWM rate
  //TCCR2A = (TCCR2A | _BV(WGM20)) & ~_BV(WGM21);
  //TCCR2B &= ~_BV(WGM22);
  
#if AUDIO_PIN == 11
  // Do non-inverting PWM on pin OC2A (arduino pin 11) (p.159)
  // OC2B (arduino pin 3) stays in normal port operation:
  // COM2A1=1, COM2A0=0, COM2B1=0, COM2B0=0
  TCCR2A = (TCCR2A | _BV(COM2A1)) & ~(_BV(COM2A0) | _BV(COM2B1) | _BV(COM2B0));
#endif  

#if AUDIO_PIN == 3
  // Do non-inverting PWM on pin OC2B (arduino pin 3) (p.159).
  // OC2A (arduino pin 11) stays in normal port operation: 
  // COM2B1=1, COM2B0=0, COM2A1=0, COM2A0=0
  TCCR2A = (TCCR2A | _BV(COM2B1)) & ~(_BV(COM2B0) | _BV(COM2A1) | _BV(COM2A0));
#endif
  
  // No prescaler (p.162)
  TCCR2B = (TCCR2B & ~(_BV(CS22) | _BV(CS21))) | _BV(CS20);
  // prescaler x8 for slow-mo testing
  //TCCR2B = (TCCR2B & ~(_BV(CS22) | _BV(CS20))) | _BV(CS21);

  // Set initial pulse width to the rest position (0v after DC decoupling)
  OCR2 = REST_DUTY;
}

void afsk_timer_start()
{
  // Clear the overflow flag, so that the interrupt doesn't go off
  // immediately and overrun the next one (p.163).
  TIFR2 |= _BV(TOV2);       // Yeah, writing a 1 clears the flag.

  // Enable interrupt when TCNT2 reaches TOP (0xFF) (p.151, 163)
  TIMSK2 |= _BV(TOIE2);
}

void afsk_timer_stop()
{
  // Output 0v (after DC coupling)
  OCR2 = REST_DUTY;

  // Disable playback interrupt
  TIMSK2 &= ~_BV(TOIE2);
}



#endif // ifdef AVR
