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

#include "config.h"
#include "radio_mx146.h"
#include <Wire.h>
#include <WProgram.h>

const int MAX_RES = 16;
char res[MAX_RES];

void RadioMx146::send_cmd(const char *cmd, int cmd_len, char *res, int res_len)
{
  int i;
  Wire.beginTransmission(0x48);
  for (i = 0; i < cmd_len; i++) {
    Wire.send(cmd[i]);
  }
  Wire.endTransmission();
  delay(100);
  
  if (res_len > 0) {
    Wire.requestFrom(0x48, res_len);
    for (i = 0; i < res_len; i++) {
      while (Wire.available() == 0) ;
      res[i] = Wire.receive();
    }
  }
}

void RadioMx146::set_freq(unsigned long freq)
{
  char cmd[5];
  cmd[0] = 'B';
  *((unsigned long *)(cmd+1)) = freq;
  send_cmd(cmd, 5, res, MAX_RES);
}

int RadioMx146::query_temp()
{
  send_cmd("QT", 2, res, 1);
  return res[0];
}

void RadioMx146::setup()
{
  Wire.begin();   // Join the I2C bus as a master
  set_freq(144800000UL);
}

void RadioMx146::ptt_on()
{
  digitalWrite(PTT_PIN, HIGH);
  // TODO: should wait for the "RDY" signal
  delay(25);
}

void RadioMx146::ptt_off()
{
  digitalWrite(PTT_PIN, LOW);
}
