///////////////////////////////////////////////////////////////
//
//  Quad LFO
//
//  OUTPUT:
//  Pin 11 (LFO1), 3 (LFO2), 6 (LFO3), and 5 (LFO4).
//  It is important to add low-pass filters to these
//  pins to smooth out the resulting PWM waveform
//
//  INPUT:
//  Potentiometers to control LFO frequency
//  Momentary switches to increment through wave tables
//  Digital inputs (triggers) to restart LFO
//
//  See the PIN MAP, below, for specific pin assignments
//
//  ==============================================================
//  PIN MAP:
//  ==============================================================
//
//  Analog
//
//  A0: Freq LFO 1      (POT)
//  A1: Freq LFO 2      (POT)
//  A2: Freq LFO 3      (POT)
//  A3: Freq LFO 4      (POT)
//  A4: NC
//  A5: NC
//
//  Digital
//
//  D0:   NC            (SERIAL TX)
//  D1:   NC            (SERIAL RX)
//  D2:   LFO 1 WAVE    (SWITCH)
//  D3:   LFO 2 OUTPUT  (PWM~)
//  D4:   LFO 2 WAVE    (SWITCH)
//  D5:   LFO 4 OUTPUT  (PWM~)
//  D6:   LFO 3 OUTPUT  (PWM~)
//  D7:   LFO 3 WAVE    (SWITCH)
//  D8:   LFO 4 WAVE    (SWITCH)
//  D9:   LFO 1 SYNC    (DIGITAL INPUT)
//  D10:  LFO 2 SYNC    (DIGITAL INPUT)
//  D11:  LFO 1 OUTPUT  (PWM~)
//  D12:  LFO 3 SYNC    (DIGITAL INPUT)
//  D13:  LFO 4 SYNC    (DIGITAL INPUT)
//
//  ==============================================================
//

//  The MIT License (MIT)
//
//  Copyright (c) 2013-2019 Robert W. Gallup (www.robertgallup.com)
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
//

// Macros for setting and clearing bits
#define   setBit(addr, bit) (_SFR_BYTE(addr) |=  _BV(bit))
#define clearBit(addr, bit) (_SFR_BYTE(addr) &= ~_BV(bit))

// Number of oscillators
const byte NUM_LFO = 4;

// Constant used in calculating frequency tuning word
const unsigned long long POW2TO32 = pow(2, 32);

//------------------------------------------
//  PWM Pins and Registers
//  WARNING: DO NOT CHANGE THESE!
const byte LFO_PIN[] = {11, 3, 6, 5};
volatile byte *const PWM_REG[] = {&OCR2A, &OCR2B, &OCR0A, &OCR0B};
//------------------------------------------

// Control Framework
#include "src/CS_Switch.h"
#include "src/CS_Pot.h"

// Wave tables
#include "wave/noise256.h"
#include "wave/ramp256.h"
#include "wave/saw256.h"
#include "wave/sine256.h"
#include "wave/tri256.h"
#include "wave/pulse8.h"
#include "wave/pulse16.h"
#include "wave/pulse64.h"
#include "wave/sq256.h"

// Settings
#include "Settings.h"

// Wave table pointers
const byte *waveTable[] = {sine256, ramp256, saw256, tri256, pulse8, pulse16, pulse64, sq256, noise256};
#define NUM_WAVES (sizeof(waveTable) / sizeof(byte *))

// Display (optional)
#if defined(DISPLAY)
#include "Display.h"
Display displaySurface;
#endif

// Oscillator frequency controls and wave select inputs
CS_Pot    freqCtrl[] = {CS_Pot    (FREQ1_PIN, POT_RESPONSE), CS_Pot    (FREQ2_PIN, POT_RESPONSE), CS_Pot    (FREQ3_PIN, POT_RESPONSE), CS_Pot    (FREQ4_PIN, POT_RESPONSE)};
CS_Switch waveCtrl[] = {CS_Switch (WAVE1_PIN),    CS_Switch (WAVE2_PIN),    CS_Switch (WAVE3_PIN),    CS_Switch (WAVE4_PIN)};

// Oscillator frequency ranges
const double FREQ_RANGE[] = {LFO1_FREQ_MAX-LFO1_FREQ_MIN, LFO2_FREQ_MAX-LFO2_FREQ_MIN, LFO3_FREQ_MAX-LFO3_FREQ_MIN, LFO4_FREQ_MAX-LFO4_FREQ_MIN};
const double FREQ_MIN  [] = {LFO1_FREQ_MIN, LFO2_FREQ_MIN, LFO3_FREQ_MIN, LFO4_FREQ_MIN};

// Set interrupt frequency (16,000,000 / 510)
// 510 is divisor rather than 512 since with phase correct PWM
// an interrupt occurs after one up/down count of the register
// See: https://www.arduino.cc/en/Tutorial/SecretsOfArduinoPWM
const double INTERRUPT_FREQ = 31372.549;

// Generic pin state variable
byte pinState;

// Interrupt variables (volatile)
volatile byte tickCounter;                      // Counts interrupt "ticks". Reset every 125
volatile byte fourMilliCounter;                 // Counter incremented every 4ms

volatile unsigned long accumulator[NUM_LFO];    // Counter accumulator for LFOs
unsigned long tuningWord[NUM_LFO];              // Wavetable tuning word

// Pointers to LFO wavetables
byte waveNum[] = {0, 0, 0, 0};
byte *waveform[NUM_LFO];

// Sync variables
#if defined(SYNC)
  const byte SYNC_PIN[] = {SYNC1_PIN, SYNC2_PIN, SYNC3_PIN, SYNC4_PIN};
  byte lastSync[NUM_LFO];
#endif

void setup()
{
  Serial.begin(9600);

  // DISPLAY
#if defined(DISPLAY)
  displaySurface.begin();
#endif

  // Initialize PWM pin, LFO waveform, frequency control, wave selector input, and tuning word
  for (byte i=0; i<NUM_LFO; i+=1) {
    pinMode (LFO_PIN[i], OUTPUT);
    waveform[i] = (byte*)waveTable[waveNum[i]];
    freqCtrl[i].begin();
    while (waveCtrl[i].stateDebounced() == 0);
    tuningWord[i] = POW2TO32 * (((((double)freqCtrl[i].value() * FREQ_RANGE[i]) / 1023L) + FREQ_MIN[i]) / INTERRUPT_FREQ);
  }
  
  // Initialize sync pins (optional)
#if defined(SYNC)
  for (byte i=0; i<NUM_LFO; i+=1) {
    pinMode (SYNC_PIN[i], INPUT);
    lastSync[i] = digitalRead(SYNC_PIN[i]);
  };  
#endif

  // Setup PWM mode and Interrupt
  PWM_Setup();

}


void loop()
{

  // SYNC
#if defined(SYNC)
  for (byte i=0; i<NUM_LFO; i+=1) {
    pinState = digitalRead(SYNC_PIN[i]);
    if (pinState != lastSync[i]) {
      lastSync[i] = pinState;
      if (pinState == SYNC_TRIGGER) {
        accumulator[i] = 0;
      }
    }
  }
#endif

  // Check other controls and update display every 1/10 second
  if (fourMilliCounter > 25) {
    fourMilliCounter = 0;

    // If DISPLAY defined in Settings.h
#if defined(DISPLAY)
    displaySurface.update();
#endif

    // Update LFOs
    for (byte i=0; i<NUM_LFO; i+=1) {
      
      // WAVEFORM
      pinState = waveCtrl[i].stateDebounced();
      if (waveCtrl[i].changed()) {
        if (pinState == 1) {
          Serial.println(waveNum[i]);
          waveNum[i]  = (waveNum[i] + 1) % NUM_WAVES;
          waveform[i] = (byte*)waveTable[waveNum[i]];
        }
      }
      
      // FREQUENCY
      tuningWord[i] = POW2TO32 * (((((double)freqCtrl[i].value() * FREQ_RANGE[i]) / 1023L) + FREQ_MIN[i]) / INTERRUPT_FREQ);
    }
  }
}


//******************************************************************
// PWM setup
//
void PWM_Setup() {

  // TIMER 0
  
  // Prescaler 1
  setBit   (TCCR0B, CS20);
  clearBit (TCCR0B, CS21);
  clearBit (TCCR0B, CS22);

  // Non-inverted PWM
  clearBit (TCCR0A, COM2A0);
  setBit   (TCCR0A, COM2A1);
  clearBit (TCCR0A, COM2B0);
  setBit   (TCCR0A, COM2B1);

  // Phase Correct PWM
  setBit   (TCCR0A, WGM20);
  clearBit (TCCR0A, WGM21);
  clearBit (TCCR0B, WGM22);

  // TIMER 2
  
  // Prescaler 1
  setBit   (TCCR2B, CS20);
  clearBit (TCCR2B, CS21);
  clearBit (TCCR2B, CS22);

  // Non-inverted PWM
  clearBit (TCCR2A, COM2A0);
  setBit   (TCCR2A, COM2A1);
  clearBit (TCCR2A, COM2B0);
  setBit   (TCCR2A, COM2B1);

  // Phase Correct PWM
  setBit   (TCCR2A, WGM20);
  clearBit (TCCR2A, WGM21);
  clearBit (TCCR2B, WGM22);

  // ENABLE INTERRUPT ON TIMER 2
  setBit   (TIMSK2, TOIE2);

}


////////////////////////////////////////////////////////////////
//
// Interrupt Service (TIMER 2)
// Frequency = 16,000,000 / 510 = 31372.5
//
ISR(TIMER2_OVF_vect) {

  // Count every four milliseconds
  if (tickCounter++ == 125) {
    fourMilliCounter++;
    tickCounter = 0;
  }

  // Update PWM registers for each wave
  for (byte i=0; i<NUM_LFO; i+=1) {
    accumulator[i] += tuningWord[i];
    *PWM_REG[i] = pgm_read_byte_near(waveform[i] + (accumulator[i] >> 24));
  }

}
