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
#ifndef __MODEM_HAL_AVR_H__
#define __MODEM_HAL_AVR_H__

#include <avr/pgmspace.h>
#include <stdint.h>
#include "config.h"

// Exported consts
extern const uint32_t MODEM_CLOCK_RATE;
extern const uint8_t REST_DUTY;
extern const uint16_t TABLE_SIZE;

// Exported functions
void modem_hal_setup();   // modem setup
void modem_hal_start();   // start modulation
void modem_hal_stop();    // stop modulation
void modem_hal_output_sample(int phase);    // output sample
#ifdef DEBUG_MODEM
uint16_t modem_hal_timer_counter();         // timer counter
int modem_hal_isr_overrun();                // has the isr overrun?
#endif

#endif // ifndef __MODEM_HAL_AVR_H__
#endif // ifdef AVR
