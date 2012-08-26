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

/* Credit to:
 *
 * cathedrow for this idea on using the ADC as a volt meter:
 * http://code.google.com/p/tinkerit/wiki/SecretVoltmeter
 */

#ifdef PIC32MX

#include "config.h"
#include "pin.h"
#include "sensors_pic32.h"
#include <WProgram.h>

void sensors_setup()
{
  pinMode(EXTERNAL_LM60_VS_PIN, OUTPUT);
  pinMode(INTERNAL_LM60_VS_PIN, OUTPUT);
  analogReference(DEFAULT);       // Ref=AVDD, AVSS
}

int sensors_lm60(int powerPin, int readPin)
{
  uint16_t adc = 0;
  pin_write(powerPin, HIGH);   // Turn the LM60 on
  delayMicroseconds(5);           // Allow time to settle
  adc = analogRead(readPin);      // Real read
  pin_write(powerPin, LOW);    // Turn the LM60 off
  int mV = 3300L * adc / 1024L;   // Millivolts
  
  switch(TEMP_UNIT) {
    case 1: // C
      // Vo(mV) = (6.25*T) + 424 -> T = (Vo - 424) * 100 / 625
      return (4L * (mV - 424) / 25) + CALIBRATION_VAL;
    case 2: // K
      // C + 273 = K
      return (4L * (mV - 424) / 25) + 273 + CALIBRATION_VAL;
    case 3: // F
      // (9/5)C + 32 = F
      return (36L * (mV - 424) / 125) + 32 + CALIBRATION_VAL;
  }
}

int sensors_ext_lm60()
{
  return sensors_lm60(EXTERNAL_LM60_VS_PIN, EXTERNAL_LM60_VOUT_PIN);
}

int sensors_int_lm60()
{
  return sensors_lm60(INTERNAL_LM60_VS_PIN, INTERNAL_LM60_VOUT_PIN);
}

int sensors_vin()
{
   uint16_t adc = analogRead(VMETER_PIN); 
   uint16_t mV = 3300L * adc / 1024;
   
   // Vin = mV * R2 / (R1 + R2)
   int vin = (uint32_t)mV * (VMETER_R1 + VMETER_R2) / VMETER_R2;
   return vin;
}


#endif // ifdef PIC32MX
