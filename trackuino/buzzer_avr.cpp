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

#include "config.h"
#include "buzzer_avr.h"
#include <WProgram.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#if BUZZER_PIN == 9
#  define OCR1 OCR1A
#endif
#if BUZZER_PIN == 10
#  define OCR1 OCR1B
#endif

// Module constants
static const unsigned long PWM_PERIOD = F_CPU / BUZZER_FREQ;
static const unsigned long ON_CYCLES = BUZZER_FREQ * BUZZER_ON_TIME;
static const unsigned long OFF_CYCLES = BUZZER_FREQ * BUZZER_OFF_TIME;

// Module variables
static volatile bool buzzing;
static volatile unsigned long alarm;

// Exported functions
void buzzer_setup()
{
  pinMode(BUZZER_PIN, OUTPUT);
  buzzing = false;
  alarm = 1;

  // Top is ICR1 (WGM1=14), p.135
  TCCR1A = _BV(WGM11);
  TCCR1B = _BV(WGM13) | _BV(WGM12);

  // Set top to PWM_PERIOD
  ICR1 = PWM_PERIOD;

  // Set duty cycle = 50%
  OCR1 = PWM_PERIOD / 2;

  // Enable interrupts on timer overflow
  TIMSK1 |= _BV(TOIE1);
}

void buzzer_on()
{
  // Start the timer, no prescaler (CS1=1)
  TCCR1B |= _BV(CS10);
}

void buzzer_off()
{
  // Stop the timer (CS1=0)
  TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
}

// Interrupt Service Routine for TIMER1. This is used to switch between the
// buzzing and quiet periods when ON_CYCLES or OFF_CYCLES are reached.
ISR (TIMER1_OVF_vect)
{
  alarm--;
  if (alarm == 0) {
    buzzing = !buzzing;
    if (buzzing) {
#if BUZZER_PIN == 9
      // Non-inverting pin 9 (COM1A=2), p.135
      TCCR1A |= _BV(COM1A1);
#endif
#if BUZZER_PIN == 10
      // Non-inverting pin 10 (COM1B=2), p.135
      TCCR1A |= _BV(COM1B1)
#endif
      alarm = ON_CYCLES;
    } else {
#if BUZZER_PIN == 9
      // Disable PWM on pin 9
      TCCR1A &= ~_BV(COM1A1);
#endif
#if BUZZER_PIN == 10
      // Disable PWM on pin 10
      TCCR1A &= ~_BV(COM1B1);
#endif
      alarm = OFF_CYCLES;
    }
  }
}

#endif // #ifdef AVR
