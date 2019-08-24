# ArduinoDualLFO
---

ArduinoQuadLFO uses PWM, interrupts and simple low-pass filters on output pins to create four simultaneous waveforms from an Arduino.

## Description

ArduinoQuadLFO assumes that you have certain hardware hooked up to Arduio pins:

2. One potentiometer and a switch for each LFO (frequency, depth, waveform)
4. Floating input pin for the trigger input (the default state is determined on startup. Trigger is activated when the state changes)

For more information on the physical connections, see the Fritzing files. I've included the native Fritzing file (Arduino Dual LFO.fzz) as well
as PDF copies of the schematic and breadboard layout.

The Arduino sketch is in the Quad_LFO folder. You can copy it to your Arduino Sketches folder. Or,
you can open the sketch directly from the Arduino IDE using File > Open...

Also included is an Excel worksheet that calculates the various waveforms used in the LFOs. Sample data is copied
from the worksheet by selecting the relevant column and copy/pasting it to a sketch header (with variable definition
before and after the data -- see the other wave headers for reference). You can use the worksheet to calculate and add your own waveforms.

