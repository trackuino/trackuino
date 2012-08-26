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

#include "afsk_pic32.h"
#include <p32xxxx.h>
#include <plib.h>

// This is the timer 2 interrupt service routine (ISR). Multiple
// modems can be operated from here.
extern "C" void __ISR(_TIMER_2_VECTOR, ipl6) T2_IntHandler (void)
{
  // Clear interrupt. By clearing the IF *before* handling the
  // interrupt we can test for overruns (the IF would turn back on).
  mT2ClearIntFlag();

  // Call modem ISRs:
  afsk_isr();
}

#endif // ifdef PIC32MX
