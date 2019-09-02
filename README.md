# ArduinoQuadLFO

ArduinoQuadLFO uses PWM, interrupts and simple low-pass filters on output pins to create four simultaneous waveforms from an Arduino.

ArduinoQuadLFO assumes that you have certain hardware hooked up to Arduio pins (see pin map in source code):

1. One PWM pin for output connected to a simple lowpass filter
2. One potentiometer to adjust frequency
3. One switch to increment through waveforms
4. One floating digital input pin to restart the LFO (Optional. Can be active HIGH or LOW)

Additional Notes:

* The Arduino sketch is in the Quad_LFO folder. You can copy it to your Arduino Sketches folder. Or,
you can open the sketch directly from the Arduino IDE using **File > Open...**
* Also included is an Excel worksheet to calculate waveforms used. Sample data is copied from the worksheet to a sketch header file. You can create your own tabs and functions in the worksheet to create your own waveforms.

## Hardware Connections
**Lowpass Filter for PWM Output Pins:**

```
               1K
            Resistor
PWM PIN > ----/\/\/\--•------------  (LFO Output)
                      |
                      |
                     ---  100nf
                     ---  capacitor
                      |
                      |
                     GND


```

**Frequency Potentiometer:**

```
                     5V
                     |
                    _|_
                   |   |
ANALOG IN <------- |   |  POT (10K)
                   |   |
                    ---
                     |
                     |
                    GND
                    

```
**Waveform Selector Button (Tact Switch):**

```
                    /
DIGITAL IN <------ /    ------ GND
                    

```
