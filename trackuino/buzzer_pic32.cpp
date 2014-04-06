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
#include "buzzer.h"
#include "pin.h"
#include <p32xxxx.h>
#include <plib.h>
#include <WProgram.h>

// Module constants
static const unsigned long PWM_PERIOD = F_CPU / 8 / BUZZER_FREQ;
static const unsigned long ON_CYCLES = BUZZER_FREQ * BUZZER_ON_TIME;
static const unsigned long OFF_CYCLES = BUZZER_FREQ * BUZZER_OFF_TIME;
#if BUZZER_TYPE == 0  // active buzzer
static const uint16_t DUTY_CYCLE = PWM_PERIOD;
#endif
#if BUZZER_TYPE == 1  // passive buzzer
static const uint16_t DUTY_CYCLE = PWM_PERIOD / 2;
#endif

// Module variables
static volatile bool is_buzzer_on;
static volatile bool buzzing;
static unsigned long alarm;

// Exported functions
void buzzer_setup()
{
  pinMode(BUZZER_PIN, OUTPUT);
  pin_write(BUZZER_PIN, LOW);
  buzzing = false;
  is_buzzer_on = false;
  alarm = 1;

  // There are two timers capable of PWM, 2 and 3. We are using 2 for the modem,
  // so use 3 for the buzzer.
  OpenTimer3(T3_ON | T3_PS_1_8, PWM_PERIOD);
  ConfigIntTimer3(T3_INT_ON | T3_INT_PRIOR_5);
}

void buzzer_on()
{
  is_buzzer_on = true;
}

void buzzer_off()
{
  is_buzzer_on = false;
}

// Interrupt Service Routine for TIMER 3. This is used to switch between the
// buzzing and quiet periods when ON_CYCLES or OFF_CYCLES are reached.
extern "C" void __ISR (_TIMER_3_VECTOR, ipl5) T3_IntHandler (void)
{
  interrupts();   // allow other interrupts (ie. modem)
  alarm--;
  if (alarm == 0) {
    buzzing = !buzzing;
    if (is_buzzer_on && buzzing) {
      switch(BUZZER_PIN) {
        case 9:
          OpenOC4(OC_ON | OC_TIMER3_SRC | OC_PWM_FAULT_PIN_DISABLE, DUTY_CYCLE, 0);
          break;
        case 10:
          OpenOC5(OC_ON | OC_TIMER3_SRC | OC_PWM_FAULT_PIN_DISABLE, DUTY_CYCLE, 0);
          break;
      }
      alarm = ON_CYCLES;
    } else {
      switch(BUZZER_PIN) {
        case 9:  CloseOC4(); break;
        case 10: CloseOC5(); break;
      }
      alarm = OFF_CYCLES;
      pin_write(BUZZER_PIN, LOW);
    }
  }
  // Clear interrupt flag
  // This will break other interrupts and millis() (read+clear+write race condition?)
  //   IFS0bits.T3IF = 0; // DON'T!! 
  // Instead:
  mT3ClearIntFlag();
}

#endif // #ifdef PIC32MX
