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
#include "modem.h"
#include "modem_hal_pic32.h"
#include "pin_pic32.h"
#include <p32xxxx.h>
#include <plib.h>
#include <WProgram.h>

#if AUDIO_PIN == 3
#  define OCxRS OC1RS
#  define OCxCON OC1CON
#endif
#if AUDIO_PIN == 11
#  error Oops! The Uno32 can't do PWM on pin 11
#endif

// Module constants
static const uint8_t modem_sine_table[512] = {
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

// External constants
const uint32_t MODEM_CLOCK_RATE = F_CPU / 2;  // 40 MHz
const uint8_t REST_DUTY         = 127;
const uint16_t TABLE_SIZE       = sizeof(modem_sine_table);


// External functions
void modem_hal_setup()
{
  // digitalWrites (to turn on/off sensors, mainly) disrupt the timers
  // (see wiring_digital.c and look for 'digital_pin_to_timer' in
  // variants/Uno32/Board_Data.c). We need to make sure we don't use
  // digitalWrite on any pin that is related to timer 2. Note that
  // other timers are not an option since the only PWM capable timers
  // are 2 and 3 and 3 is already used for the buzzer.
  
  OpenTimer2(T2_ON | T2_PS_1_2, 0xFF); 
  OpenOC1(OC_ON | OC_TIMER2_SRC | OC_PWM_FAULT_PIN_DISABLE, 0, 0);

  // The above could also be accomplished by... ?
  /*
  T2CONbits.ON = 0; // Turn timer 2 off before anything else (recommended in ref. guide 14.3.11)
  T2CON = T2_PS_1_8; // Set prescaler x8
  TMR2 = 0; // Clear the counter
  PR2 = 0xFF; // Set the period
  IPC2bits.T2IP = 6; // Set T2 interrupt priority to 6
  OCxCON = OC_ON | OC_TIMER2_SRC | OC_PWM_FAULT_PIN_DISABLE;   // Set OCx to use timer2 (OCxCON defined in .h)
  T2CONbits.ON = 1; // Turn the timer on
  OCxRS = REST_DUTY;   // Set output PWM to rest duty
  */
  // Use pin 43 on the Uno32 as PTT indicator
  pinMode(43, OUTPUT);
 
}

void modem_hal_start()
{
  ConfigIntTimer2(T2_INT_ON | T2_INT_PRIOR_6);
  
  // The above could also be accomplished by ... ?
  /*  
  IFS0bits.T2IF = 0; // Clear the T2 interrupt flag, so that the ISR doesn't go off immediatelly
  IEC0bits.T2IE = 1;   // Enable T2 interrupt
  */

  // Turn PTT led on
  pin_write(43, HIGH);
}

void modem_hal_stop()
{
  // Return to rest duty cycle
  SetDCOC1PWM(REST_DUTY);
  //OCxRS = REST_DUTY;

  // Disable playback interrupt
  mT2IntEnable(0);
  //IEC0bits.T2IE = 0;

  // Turn PTT led off
  pin_write(43, LOW);
}

void modem_hal_output_sample(int phase)
{
  SetDCOC1PWM(modem_sine_table[phase]);
  //OCxRS = modem_sine_table[phase];
}

#ifdef DEBUG_MODEM
uint16_t modem_hal_timer_counter()
{
  return (uint16_t) TMR2;
}

int modem_hal_isr_overrun()
{
  return (IFS0bits.T2IF);
}
#endif

// This is called at PLAYBACK_RATE Hz to load the next sample.
extern "C" void __ISR(_TIMER_2_VECTOR, ipl6) T2_IntHandler (void)
{
  // Clear interrupt. By clearing the IF *before* handling the
  // interrupt we can test for overruns (the IF would turn back on).
  mT2ClearIntFlag();

  modem_playback();
}


#endif // ifdef PIC32MX
