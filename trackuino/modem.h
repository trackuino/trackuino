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

#ifndef __MODEM_H__
#define __MODEM_H__

#define MODEM_MAX_PACKET 512

extern volatile unsigned char modem_packet[MODEM_MAX_PACKET];  // Upper layer data
extern volatile unsigned int modem_packet_size;                // in bits

void modem_setup();
void modem_flush_frame();
int modem_busy();
void modem_playback();
#ifdef DEBUG_MODEM
void modem_debug();
#endif

#endif
