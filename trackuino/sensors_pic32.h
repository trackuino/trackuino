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
#ifndef __SENSORS_PIC32_H__
#define __SENSORS_PIC32_H__

void sensors_setup();
unsigned long sensors_aref();
long sensors_internal_temp();
int sensors_int_lm60();
int sensors_ext_lm60();
int sensors_vin();

#endif // ifndef __SENSORS_PIC32_H__
#endif // ifdef PIC32MX
