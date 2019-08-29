// Settings 

// Comment out the following #define to deactivate SYNC input(s)
// Specify whether HIGH or LOW is the trigger state using "TRIGGER" constants
// To use a common pin for syncing both LFOs, define SYNC_COMMON (uncomment the #define)
// The common pin/trigger are taken from SYNC1

// Uncomment the following line to support a user-supplied display
//#define DISPLAY

// FREQ RESPONSE (responsiveness of frequency POT. 1-4 with 4 being less responsive but more stable
const byte POT_RESPONSE =         2;

// FREQUENCY (analog pins)

const byte FREQ1_PIN =            0;
const byte FREQ2_PIN =            1;
const byte FREQ3_PIN =            2;
const byte FREQ4_PIN =            3;

// WAVE SWITCHES (digital pins)

const byte WAVE1_PIN =            2;
const byte WAVE2_PIN =            4;
const byte WAVE3_PIN =            7;
const byte WAVE4_PIN =            8;

// LFO SYNC (Optional) -- Comment #define to remove SYNC feature

#define SYNC
#if defined(SYNC)
const byte SYNC1_PIN =            9;
const byte SYNC2_PIN =           10;
const byte SYNC3_PIN =           12;
const byte SYNC4_PIN =           13;
const byte SYNC_TRIGGER =      HIGH;
#endif

// LFO FREQUENCY SETTINGS
const double LFO1_FREQ_MIN =      0.1;
const double LFO1_FREQ_MAX =      100;

const double LFO2_FREQ_MIN =      0.1;
const double LFO2_FREQ_MAX =      100;

const double LFO3_FREQ_MIN =      0.1;
const double LFO3_FREQ_MAX =      100;

const double LFO4_FREQ_MIN =      0.1;
const double LFO4_FREQ_MAX =      100;
