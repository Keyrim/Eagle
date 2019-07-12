/*
TimerScheduler.cpp - A timer/scheduler library for fairly accurate, 
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

/*
The general program structure and naming schemes in this
library are copied from the "TWI/I2C library for Wiring &
Arduino" Copyright (c) 2006 Nicholas Zambetti.
*/


extern "C" {
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "clock.h"
}

#include "TimerScheduler.h"

// uncomment to do direct i/o on pins for o'scope testing
#include <avr/io.h>
#include <avr/interrupt.h>

// Initialize Class Variables //////////////////////////////////////////////////
void (*TimerScheduler::user_onTick[16])(void);

// Constructors ////////////////////////////////////////////////////////////////

TimerScheduler::TimerScheduler()
{
}
 
// Public Methods //////////////////////////////////////////////////////////////


//-- interface functions

// register the user's callback-
// the call to onTick() is made in the Arduino code
// The user's function gets aliased to
// user_onTick().

void TimerScheduler::onTick(uint8_t unitNum, void (*function)(void))
{
    user_onTick[unitNum] = function;
}


// in TimerScheduler::begin()
// clock_attachUnitRolloverEvent() attaches the worker function
// TimerScheduler::onTickService() to the alias
// clock_onUnitTick() which is called inside the ISR in clock.c
// Also, sets up timer2.


void TimerScheduler::begin()
{
    clock_attachUnitRolloverEvent(onTickService);
    clock_init();
}


// int is for compat w/ arduino code
void TimerScheduler::attachPin(uint8_t channel, int pinNum){
    clock_attachIOpin(channel, pinNum);
}

// tagged for bye-bye
//void TimerScheduler::enable(uint8_t channel){
    //clock_TimerEnable(channel);
//}

// enables timer's run flag & isr updates
void TimerScheduler::start(uint8_t channel){
    clock_start(channel);
}

// skip this timer in isr update cycle
void TimerScheduler::stop(uint8_t channel){
    clock_stop(channel);
}

void TimerScheduler::setMasterPeriod(uint8_t mPeriod){
    clock_setMasterPeriod(mPeriod);
}


// stop timer and zero out everything
void TimerScheduler::reset(uint8_t channel){
    clock_reset(channel);
}

// don't stop -- clear current count & flags
void TimerScheduler::clearCount(uint8_t channel){
    clock_clearCount(channel);
}

// only for setting flags and doing callbacks -- not for attached pins
void TimerScheduler::repeat(uint8_t channel, uint16_t value){
    clock_repeat(channel, value);
}

void TimerScheduler::setHighTime(uint8_t channel, uint16_t value){
    clock_writeSetPointHtL(channel, value);
}

void TimerScheduler::setLowTime(uint8_t channel, uint16_t value){
    clock_writeSetPointLtH(channel, value);
}

uint8_t TimerScheduler::flagIsSet(uint8_t channel){
    return clock_checkUserFlag(channel);
} 

//checks AND clears -- used in TimerScheduler::onTickService()
uint8_t TimerScheduler::checkCallbackFlag(uint8_t channel){ 
    return clock_checkCallbackFlag(channel);
} 

// user selects ASTABLE or MONOSTABLE
void TimerScheduler::stableMode(uint8_t channel, uint8_t flag){ 
    clock_astable(channel, flag);
}

void TimerScheduler::mono(uint8_t channel, uint16_t value){
    clock_mono(channel, value);
}

// worker function
// can do many things
// called by the ISR as the alias clock_onUnitTick().
// Calls the Arduino user's callback function.
// which onTick() has aliased to user_onTick().

void TimerScheduler::onTickService(void)
{
    uint8_t i =0;
 
    for (i=0; i<16; i++) {
        // check to see whether a callback exists
        if ((user_onTick[i] > 0)){   // non-null function pointer
            if(Timer.checkCallbackFlag(i)) { // ready to run?
                //Timer.clearCallbackFlag(i);
                user_onTick[i](); // array of function pointers
            }
        }
    }
}


// Preinstantiate Objects //////////////////////////////////////////////////////

TimerScheduler Timer = TimerScheduler();

