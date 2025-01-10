/*
    DSC Keybus Interface

    https://github.com/taligentx/dscKeybusInterface

    This library is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef dscClassicKeypad_h
#define dscClassicKeypad_h

#include <Arduino.h>

#if defined(__AVR__)
const byte dscBufferSize = 10;  // Number of keys to buffer if the sketch is busy
#elif defined(ESP8266) || defined (ESP32)
const byte dscBufferSize = 50;
#endif
const byte dscReadSize = 2;    // Maximum bytes of a Keybus command

enum Light {off, on, blink};    // Custom values for keypad lights status

class dscClassicKeypadInterface {

  public:
    dscClassicKeypadInterface(byte setClockPin, byte setReadPin, byte setWritePin);

    // Interface control
    void begin(Stream &_stream = Serial);                            // Initializes the stream output to Serial by default
    bool loop();                                                     // Returns true if valid panel data is available
    void beep(byte beeps = 0);                                       // Keypad beep, 1-128 beeps
    void tone(byte beep = 0, bool tone = false, byte interval = 0);  // Keypad tone pattern, 1-7 beeps at 1-15s interval, with optional constant tone
    void buzzer(byte seconds = 0);                                   // Keypad buzzer, 1-255 seconds

    // Keypad key
    byte key, keyAvailable;

    // Keypad lights
    Light lightReady = on, lightArmed, lightMemory, lightBypass, lightTrouble, lightProgram, lightFire, lightBacklight = on;
    Light lightZone1, lightZone2, lightZone3, lightZone4, lightZone5, lightZone6, lightZone7, lightZone8;

    // Panel Keybus commands
    byte classicCommand[2]  = {0x00, 0x80};

    /*
     * moduleData[] stores keypad data in an array: command [0], stop bit by itself [1], followed by the
     * remaining data.  These can be accessed directly in the sketch to get data that is not already tracked
     * in the library.  See dscKeybusPrintData.cpp for the currently known DSC commands and data.
     */
    static volatile byte moduleData[dscReadSize];

    // Key data buffer overflow, true if dscBufferSize needs to be increased
    static volatile bool bufferOverflow;

    // Timer interrupt function to capture data - declared as public for use by AVR Timer1
    static void dscClockInterrupt();

  private:

    void zoneLight(Light lightZone, byte zoneBit);
    void panelLight(Light lightPanel, byte zoneBit);

    Stream* stream;
    byte panelLights = 0x80, previousLights = 0x80;
    byte panelBlink, previousBlink;
    byte panelZones, previousZones;
    byte panelZonesBlink, previousZonesBlink;
    bool startupCycle = true;
    bool setBeep, setTone, setBuzzer;
    byte commandInterval = 26;   // Sets the milliseconds between panel commands
    bool keyBeep, beepStart;

    #if defined(ESP32)
    static hw_timer_t * timer1;
    static portMUX_TYPE timer1Mux;
    #endif

    static int clockInterval;
    static byte dscClockPin, dscReadPin, dscWritePin;
    static volatile byte keyData;
    static volatile byte keyBufferLength;
    static volatile byte keyBuffer[dscBufferSize];
    static volatile bool commandReady, moduleDataDetected;
    static volatile bool alarmKeyDetected, alarmKeyResponsePending;
    static volatile byte clockCycleCount, clockCycleTotal;
    static volatile byte panelCommand[dscReadSize], panelCommandByteCount, panelCommandByteTotal;
    static volatile byte isrPanelBitTotal, isrPanelBitCount;
    static volatile byte isrModuleData[dscReadSize], isrModuleBitTotal, isrModuleBitCount, isrModuleByteCount;
    static volatile unsigned long intervalStart, beepInterval, repeatInterval, keyInterval, alarmKeyTime, alarmKeyInterval;
};

#endif // dscKeypad_h
