# ArduinoQuadLFO

ArduinoQuadLFO uses PWM, interrupts and simple low-pass filters on output pins to create four simultaneous waveforms from an Arduino.

For each oscillator, ArduinoQuadLFO uses one analog pin and 2-3 digital pins (see pin map in source code):

1. One analog pin connected to the center pin of a potentiometer to adjust frequency
2. One digital pin for PWM output connected to a simple lowpass filter
3. One digital pin input connected to a switch to increment through waveforms. This uses the Arduino internal pullup resistor
4. One digital pin input to restart the LFO (Optional. It is floating. Active HIGH or LOW is determined in settings.h)

Additional Notes:

* The Arduino sketch is in the Quad_LFO folder. You can copy it to your Arduino Sketches folder. Or,
you can open the sketch directly from the Arduino IDE using **File > Open...**
* Also included is an Excel workbook to calculate 256-byte waveforms. Sample data is copied from a wave worksheet to a sketch header file. You can add your own worksheets to the workbook to create your own waveforms.

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
