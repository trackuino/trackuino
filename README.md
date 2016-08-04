![Banner](https://github.com/trackuino/trackuino/wiki/img/trackuino-banner-narrow.png)

This is the firmware for Trackuino, an open-source APRS tracker based on the Arduino platform. It was designed primarily to track high altitude balloons, so it has other handy features like reading temperature sensors and a buzzer for acoustic location.

Trackuino is intended for use by licensed radio amateurs.

Features
========

 * Arduino shield form factor (you can stack more shields on it)
 * GPS: Venus 638FLPx. Reports okay above 18 Km.
 * Radio: Radiometrix's HX1 (300 mW).
 * 1200 bauds AFSK using 8-bit PWM
 * Sends out standard APRS position messages (latitude, longitude, altitude, course, speed and time).
 * Internal/external temperature sensors (LM60) to read temperature in and outside the payload
 * Active/passive buzzer support to ease acoustic payload location.
 * 2 x SMA female plugs (1 x GPS in + 1 x radio out)
 * Open source (GPLv2 license), both software and hardware. In other words, do whatever you want with it: modify it, add it to your project, etc. as long as you opensource your modifications as well.

Download
========

The latest version is 1.52.

Use the `Download ZIP` button to get the source code.

Building
========

If you are building for the Arduino platform you need Arduino IDE version 0023 or higher (tested with versions 0023, 1.0.x and 1.5.x). Get it from the [Arduino web site](http://arduino.cc/).

If you are building for the Chipkit Uno32 you need the Mpide IDE. Tested with 0023-20130715. Get it from the [Chipkit site](http://chipkit.net/).

Unzip the firmware in your sketches directory and load it up by double-clicking on trackuino.ino.

The single most important configuration file is "config.h". The file is self-documented. Here is where you set up your callsign, among other things.

Flashing
========

**Important**: When flashing the Arduino/Uno32, remove the Venus GPS or the entire Trackuino shield. After flashing the firmware, you can plug it back in. The GPS and the host computer share the same serial port on the AVR, so they will conflict when used together.

Hardware
========

The [Trackuino shield](https://github.com/trackuino/shield) repository contains the Eagle schematic / pcb files of a shield you can build as-is (gerber files are included) or modify to suit your needs. Check its README for details.

Related projects
================

Some other HAB-related projects I wrote:

  * https://github.com/trackuino/hab-tracker - balloon trajectory prediction tool for Android 
  * https://github.com/trackuino/aprsdb - an efficient and queriable APRS database server (required by the Hab Tracker Android app)
  * https://github.com/trackuino/chdk-intervalometer - a LUA intervalometer for the old Canon A570IS camera (which might work on other CHDK cameras too)

Support
=======

Discuss firmware bugs or suggestions in the issue tracker, or ask for help at the [http://hab-ham.org/forum/](hab-ham.org forum).
