# ArduinoDualLFO

ArduinoQuadLFO uses PWM, interrupts and simple low-pass filters on output pins to create four simultaneous waveforms from an Arduino.

ArduinoQuadLFO assumes that you have certain hardware hooked up to Arduio pins (see pin map in source code):

1. One PWM pin for output connected to a simple lowpass filter (see below)
2. One potentiometer to affect frequency
3. One switch to increment through waveforms
4. One floating digital input pin to restart the LFO (can be active HIGH or LOW)

```
               1K
            Resistor
PWM PIN> ----/\/\/\--•------------  (LFO Output)
                     |
                     |
                    ---  100nf
                    ---  capacitor
                     |
                     |

                    GND
```

The Arduino sketch is in the Quad_LFO folder. You can copy it to your Arduino Sketches folder. Or,
you can open the sketch directly from the Arduino IDE using File > Open...

Also included is an Excel worksheet that calculates the various waveforms used in the LFOs. Sample data is copied
from the worksheet by selecting the relevant column and copy/pasting it to a sketch header (with variable definition
before and after the data -- see the other wave headers for reference). You can use the worksheet to calculate and add your own waveforms.

