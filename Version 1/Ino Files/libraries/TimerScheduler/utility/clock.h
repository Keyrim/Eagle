/*
clock.h - A timer/scheduler library for fairly accurate, low speed timing of function calls in
Arduino sketches. Copyright (c) 2010 Ed Bennett.  All rights reserved.

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

#ifndef clock_h
#define clock_h
#endif

//#include <inttypes.h>
#include <avr/io.h>  //This is used in lots of places -- look it up! (/usr/lib/avr/include/avr/io.h)

#ifndef CPU_FREQ
#define CPU_FREQ 16000000L
#endif

#define FALSE       0
#define TRUE        1
#define STOP        0
#define RUN         1
#define DISABLED    0
#define ENABLED     1
#define ASTABLE     0
#define MONOSTABLE  1
#define CLEAR       0
#define SET         1

typedef volatile struct {
    volatile uint16_t timerSetpointLtH; // setpoints
    volatile uint16_t timerSetpointHtL;
    volatile uint16_t timerCountLtH;    // counters
    volatile uint16_t timerCountHtL;
    volatile uint8_t runStopLtH :1;        // allows counter to RUN.
    volatile uint8_t runStopHtL :1;
    volatile uint8_t enabled :1;     //ENABLES counter unit to run
    
    //ASTABLE == counter runs freely
    volatile uint8_t astableMonostable :1; 

    // flag is set on HtL timer expire and cleared
    // in clock_onUnitTick()   
    volatile uint8_t callbackFlag :1;

    // flag is set on HtL timer expire, and read and cleared 
    // in the Arduino sketch  
    volatile uint8_t userFlag :1;

    // Ard. DIO pin  -- pin 0 means no pin for this timer --
    // best not to use pin 1 either (serial port)
    volatile int arduinoPinNumber;
    
}clock_timerCounter;

uint16_t clock_ticks;

// from pins_arduino.c
const uint8_t digital_pin_to_bit_mask[] = {
    _BV(0), /* 0, port D */
    _BV(1),
    _BV(2),
    _BV(3),
    _BV(4),
    _BV(5),
    _BV(6),
    _BV(7),
    _BV(0), /* 8, port B */
    _BV(1),
    _BV(2),
    _BV(3),
    _BV(4),
    _BV(5),
    _BV(0), /* 14, port C */
    _BV(1),
    _BV(2),
    _BV(3),
    _BV(4),
    _BV(5),
};


static void (*clock_onUnitTick)(void);
// HtL is the time until the falling edge of the square wave
// LtH is the counter which times a low to high riging edge of a square wave

// test!
clock_timerCounter clock_timerChannel[16]; // array of timer structs

void clock_init(void);

void clock_start(uint8_t);          // enables timer's run flag & isr updates         
void clock_stop(uint8_t);           // completely skip this timer in isr update
void clock_setMasterPeriod(uint8_t);   // set timer2's end value -- controls hdwe IRQ rate
void clock_clearCount(uint8_t);     // don't stop -- clear current count & flags
void clock_reset(uint8_t);          // stop and zero out everything
void clock_repeat(uint8_t channel, uint16_t value);
void clock_mono(uint8_t, uint16_t); // one-shot event

//void clock_TimerEnable(uint8_t);

void clock_attachUnitRolloverEvent(void (*)(void));
void clock_attachIOpin(uint8_t, int);

// bye
void clock_timerRun(uint8_t);

void clock_writeSetPointLtH(uint8_t, uint16_t);
void clock_writeSetPointHtL(uint8_t, uint16_t);

void clock_astable(uint8_t, uint8_t);

// ----------- for flags ----------
uint8_t clock_checkUserFlag(uint8_t);   
//void clock_clearUserFlag(uint8_t);  // TAGGED FOR BYE-BYE
uint8_t clock_checkCallbackFlag(uint8_t);
// void clock_clearCallbackFlag(uint8_t);
// ----------- /flags ----------

void clock_timerRunLtH(uint8_t);
void clock_TimerEnable(uint8_t);
void clock_set_bits_func_correct (volatile uint8_t *, uint8_t);
void clock_clear_bits_func_correct (volatile uint8_t*, uint8_t);


