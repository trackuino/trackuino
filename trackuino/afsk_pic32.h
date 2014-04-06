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

#ifdef PIC32MX

#ifndef __AFSK_PIC32_H__
#define __AFSK_PIC32_H__

#include <p32xxxx.h>
#include <plib.h>
#include <stdint.h>
#include "config.h"

#define AFSK_ISR extern "C" void __ISR(_TIMER_2_VECTOR, ipl6) T2_IntHandler (void)

// Exported consts
extern const uint32_t MODEM_CLOCK_RATE;
extern const uint8_t REST_DUTY;
extern const uint16_t TABLE_SIZE;
extern const uint32_t PLAYBACK_RATE;

// Exported vars
extern const uint8_t afsk_sine_table[];

// Inline functions (this saves precious cycles in the ISR)
inline uint8_t afsk_read_sample(int phase)
{
  return afsk_sine_table[phase];
}

inline void afsk_output_sample(uint8_t s)
{
  SetDCOC1PWM(s);
}

inline void afsk_clear_interrupt_flag()
{
  mT2ClearIntFlag();
}

#ifdef DEBUG_MODEM
inline uint16_t afsk_timer_counter()
{
  return (uint16_t) TMR2;
}

inline int afsk_isr_overrun()
{
  return (IFS0bits.T2IF);
}
#endif


// Exported functions
void afsk_setup();
void afsk_send(const uint8_t *buffer, int len);
void afsk_start();
bool afsk_flush();
void afsk_isr();
void afsk_timer_setup();
void afsk_timer_start();
void afsk_timer_stop();
#ifdef DEBUG_MODEM
void afsk_debug();
#endif

#endif
#endif // PIC32MX
