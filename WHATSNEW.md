Version 1.52 - 2016/08/25
-------------------------

 * FIX: Support U-BLOX M8 (and other Glonass / mixed GPSs), thanks to tmyrtle

 * FIX: GPS minor fixes / debug code cleanup, thanks to tmyrtle


Version 1.51 - 2014/07/25
-------------------------

 * FIX: Compilation in Arduino 1.5.7 (deprecation of prog_uchar)

 * FIX: Renamed .pde -> .ino


Version 1.5 - 2014/04/06
------------------------

 * FIX: Allow compilation on Arduino 1.0+

 * FIX: Added pre-emphasis flag, makes the signal more intelligible

 * FIX: Revamped modem code to avoid ISR overruns

 * FIX: improve baudrate accuracy


Version 1.4 - 2012/08/26
------------------------

 * NEW: Support for PIC32 / Chipkit Uno32 platform

 * NEW: Support for active or passive buzzers (DC or PWM driven)

 * NEW: Battery voltage sensing

 * NEW: Slotted transmissions for multilaunch events
 
 * REMOVED: Support for the MX146 radio


Version 1.31 - 2011/08/02
-------------------------

 * FIX: Altitude 16-bit rollover


Version 1.3 - 2011/07/05
------------------------

 * FIX: More robust GPS parsing. It should improve compatibility with most
   NMEA-standard units.

 * FIX: Several optimizations in the modem module aimed at improving signal
   quality and reducing the chance of packet loss.

 * FIX: Cut down SRAM usage by 30% by moving the big sine table to PROGMEM.

 * NEW: Buzzer support for acoustic payload location.

 * NEW: Debug information to aid in testing the firmware without a radio.

 * NEW: The LED now indicates CPU activity.


Version 1.2 - 2011/06/09
------------------------

 * FIX: APRS messages reported 1/10th of the actual speed

 * NEW: Switch in config.h between celsius/farenheit/kelvin temperatures


Version 1.1 - 2011/01/12
------------------------

 * FIX: Eagle files: use soldermask under MOSFET Q5 as heat sink

 * FIX: TX LED's pin wasn't properly set as an output

 * FIX: Move code into the sketch's directory to avoid library clutter

 * NEW: Read internal/external temperatures from LM60 sensors

 
Version 20100706
----------------

 * First public release
