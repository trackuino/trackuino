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

#include "config.h"
#include "buzzer_pic32.h"
#include "pin_pic32.h"
#include <plib.h>
#include <WProgram.h>

// Module constants
static const unsigned long PWM_PERIOD = F_CPU / 8 / BUZZER_FREQ;
static const unsigned long ON_CYCLES = BUZZER_FREQ * BUZZER_ON_TIME;
static const unsigned long OFF_CYCLES = BUZZER_FREQ * BUZZER_OFF_TIME;

#if BUZZER_PIN == 9
#  define BUZZER_OCxCON OC4CON
#  define BUZZER_OCxRS OC4RS
#endif

#if BUZZER_PIN == 10
#  define BUZZER_OCxCON OC5CON
#  define BUZZER_OCxRS OC5RS
#endif

// Module variables
static volatile bool buzzing;
static unsigned long alarm;

// Exported functions
void buzzer_setup()
{
  pinMode(BUZZER_PIN, OUTPUT);
  buzzing = false;
  alarm = 1;

  // There are two timers capable of PWM, 2 and 3. We are using 2 for the modem,
  // so use 3 for the buzzer.
  // Turn timer 3 off before anything else (recommended in ref. guide 14.3.11)
  T3CONbits.ON = 0;

  // Set prescaler x8
  T3CON = T3_PS_1_8;

  // Clear the counter
  TMR3 = 0;

  // Set the period
  PR3 = PWM_PERIOD;
  
  // Set up interrupt priority
  IPC3bits.T3IP = 6;
  
  // Clear the T1 interrupt flag, so that the ISR doesn't go off immediatelly
  IFS0bits.T3IF = 0;

  // Enable T2 interrupt
  IEC0bits.T3IE = 1;

  // Set OCx to use timer2 (OCxCON defined above)
  BUZZER_OCxCON = OC_ON | OC_TIMER3_SRC | OC_PWM_FAULT_PIN_DISABLE;

  // Set output PWM to rest duty
  BUZZER_OCxRS = PWM_PERIOD / 2;
}

void buzzer_on()
{
  // Turn the timer on
  T3CONbits.ON = 1;
}

void buzzer_off()
{
  // Turn the timer off
  T3CONbits.ON = 0;
  
  // Turn of the driver fet so that power is not wasted through the bleeder resistor
  pin_write(BUZZER_PIN, LOW);
}

// Interrupt Service Routine for TIMER 3. This is used to switch between the
// buzzing and quiet periods when ON_CYCLES or OFF_CYCLES are reached.
extern "C" void __ISR (_TIMER_3_VECTOR, ipl6) T3_IntHandler (void)
{
  alarm--;
  if (alarm == 0) {
    buzzing = !buzzing;
    if (buzzing) {
      BUZZER_OCxRS = PWM_PERIOD / 2;
      alarm = ON_CYCLES;
    } else {
      BUZZER_OCxRS = 0;
      alarm = OFF_CYCLES;
    }
  }
  // Clear interrupt flag
  IFS0bits.T3IF = 0;
}

#endif // #ifdef PIC32MX
