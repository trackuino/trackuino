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

#include "pin.h"
#include <plib.h>
#include <stdint.h>
#include <WProgram.h>

// This is a digitalWrite() replacement that does not disrupt
// timer 2.
void pin_write(uint8_t pin, uint8_t val)
{
  volatile p32_ioport * iop;
  uint8_t         port;
  uint16_t        bit;
  //* Get the port number for this pin.
  if ((pin >= NUM_DIGITAL_PINS) || ((port = digitalPinToPort(pin)) == NOT_A_PIN))
  {
    return;
  }

  //* Obtain pointer to the registers for this io port.
  iop = (p32_ioport *)portRegisters(port);

  //* Obtain bit mask for the specific bit for this pin.
  bit = digitalPinToBitMask(pin);

  //* Set the pin state
  if (val == LOW)
  {
    iop->lat.clr = bit;
  }
  else
  {
    iop->lat.set = bit;
  }
}


#endif // PIC32MX

