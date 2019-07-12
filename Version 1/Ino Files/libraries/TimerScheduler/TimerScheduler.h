/*
TimerScheduler.h - A timer/scheduler library for fairly accurate, 
low speed timing of function calls in Arduino sketches. 
Copyright (c) 2010 Ed Bennett.  All rights reserved.

This library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



#ifndef TimerScheduler_h
#define TimerScheduler_h

#include <inttypes.h>


class TimerScheduler
{
private:
    static uint16_t milliseconds;
    static void onTickService(void);
    static void (*user_onTick[16])(void);
        // checks and clears flag
    uint8_t checkCallbackFlag(uint8_t);            // channel

public:
    TimerScheduler();

    
/*
the user can:

(0) register their callback by attaching it to a timer unit
(1) begin() the hardware timer running
(2) set a timer unit to ENABLED or DISABLED
(3) attach an Arduino Dio pin to a timer unit
(4) set the timer unit to either astable or monostable mode
(5) load a new HtL setpoint
(6) load a new LtH setpoint
(7) start a count by setting the runStopHtL = RUN
(8) read a counter's rollover flag
(9) reset a counter's rollover flag
(A) halt a timer unit and clear its timing setpoints

*/
    
//-- interface functions
    // setup things    
    void onTick(uint8_t, void(*)(void));    // register a callback: channel, function name
    void begin(void);                       // hardware timer setup
    void attachPin(uint8_t, int);           // channel, Arduino pin number

    // timer controls
    //void enable(uint8_t);                   // timer unit
    void start(uint8_t);                    // enables timer's run flag + updates in isr
    void stop(uint8_t);                     // completely skip this timer in isr update
    void setMasterPeriod(uint8_t);          // set timer2's end value -- controls hdwe IRQ rate
                                                // must be called after Timer.begin() (FIX THIS)
    void reset(uint8_t);                    // stop and zero out everything
    void clearCount(uint8_t);               // don't stop -- clear current count & flags
    void setHighTime(uint8_t, uint16_t);    // channel, value
    void setLowTime(uint8_t, uint16_t);     // channel, value
    uint8_t flagIsSet(uint8_t);             // for polling a timer inside loop()
   
    // only for setting flags and doing callbacks -- LtH time is 0
    void repeat(uint8_t, uint16_t);         // channel, value        
    void mono(uint8_t, uint16_t);
          
    
    // channel, flag ASTABLE=0 MONOSTABLE=1
    void stableMode(uint8_t, uint8_t);             
    
};

extern TimerScheduler Timer;

#endif
