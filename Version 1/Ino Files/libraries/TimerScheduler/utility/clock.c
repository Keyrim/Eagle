/*
clock.c - A timer/scheduler library for fairly accurate, low speed timing of function calls in
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

/*
Unit, counter timer, and channel refer to the same thing. The user allocates a timer unit to a
callback, or uses the timer unit to set flags, or do i/o.  A timer is a dual up-counter with hooks.
A channel is an array index that identifies each timer.

HtL means High-to-Low. It refers to the state transition on an i/o pin connected to the timer.
The label is used to differentiate the two counters in a timer, HtL and LtH, even though there may
be no hardware associated with that timer.
*/


#include <math.h>
#include <stdlib.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#include "clock.h"


// I get a "multiple definitions" error if this array is in the header file.
// The internet seems to associate this with the "extern" kwyword --(used in TimerScheduler.cpp)
volatile uint8_t* pin_to_port[] = {
    &PORTD, /* 0 */
    &PORTD,
    &PORTD,
    &PORTD,
    &PORTD,
    &PORTD,
    &PORTD,
    &PORTD,
    &PORTB, /* 8 */
    &PORTB,
    &PORTB,
    &PORTB,
    &PORTB,
    &PORTB,
    &PORTC, /* 14 */
    &PORTC,
    &PORTC,
    &PORTC,
    &PORTC,
    &PORTC,
};

void clock_init(void)
{
/*
 
This function does setup on the timer 2 hardware to make it generate the interrupts that drive
this library. The code uses output-compare channel A (it does not touch channel B) to fire the
interrupt.

In this setup/initialization function, I'm poking the control bits into the appropriate
hardware regisers in the MCU. Against normal practice, I' poking hard-coded values into the
registers. This is to make it easy to see what bit is doing what in the different regiters. An
example:

For the Mega48, 88, 168, and 328, bit "TOIE2" (Timer Output Interrupt Enable for timer 2) is in bit
position 1 of the TMSK2 register. To set this bit, the hard-coded version looks like TIMSK2 |= 2,
binary b00000010.

Normally, the bit positions in registers would be expressed  using _BV() and mnemonics from the
include file (iom168p.h) for the chip being used. In the include file, TOIE2 is defined as 2. It's
used like this: TIMSK2 |= _BV(TOIE2); This makes code portable across chips where TOIE2 might be in
a different position in the register. For instance, on some other chip TOIE2 might be in position
4 (binary b00010000, 0x10hex, decimal 16).

The code  TIMSK2 |= _BV(TOIE2); would compile for the other chip, without modification, because the
header file for the other chip would have the correct value for TOIE2 (the value 4). On the other
hand, to run on the other chip, the hard-coded value (0x02)used in this piece of code, TIMSK2 |= 2
would have to be changed by hand to TIMSK2 |= 16. And you would have to read the register
description for the other chip to know this. 

This function is written using hard-coded values because it has a didactic purpose in addition to a
functional one. It's an opportunity to see what's happening under the hood when you're poking
registers.


/*
Set up the prescaler.

16MHz crystal is the freq input to TIMER2 prescaler.
The prescaler feeds the TCNT2 counter.

PRESCALE 1 (b00000001) = CLK/1 
PRESCALE 2 (b00000010) = CLK/8
PRESCALE 3 (b00000011) = CLK/32
PRESCALE 4 = CLK/64
PRESCALE 5 = CLK/128
PRESCALE 6 = CLK/256
PRESCALE 7 = CLK/1024

Set the TIMER2 clock prescaler to the Slowest practical value to 
keep from hammering the CPU with IRQ's. With a 16MHz crystal, This 
setting is made with the CS22:0 (Clock Select) control bits. The
bits CS22:0 are located at bit poition 2,1,and 0 in TCCR2B. 

Only I can't get the prescaler to be helpful. It seems to not have the effect
described above. Further, The datasheet says bit CS22 is read-only.?.
*/
    TCCR2B |= 1; // seems safe


// This is the master speed adjuster. It's a numeric value to match 
// w/ TCNT2 (the up-counter) to gen. an IRQ. 
    OCR2A = 200;    // Default can be changed with Timer.masterPeriod()

/* 
Setup  timer 2 for CTC (Clear Timer on Compare match) mode:

Set WGM22:0 = 0x07 (binary 111). (WGM = Waveform Generation Mode)

From the manual: "TOP is defined as OCR2A when WGM2:0 = 7". WGM21 and WGM20 are located at bit
positions 1 and 0 in TCCR2A. Bit WGM22 is located at bit position 3 in TCCR2B. ICK!

WGM value = 0x03 (111 binary) sets CTC mode to have the settings of: Fast PWM, OCR2A updated on
BOTTOM, and TOV set on TOP. Note that CTC mode requires only WGM21 to be set, but the timer
generates interrupts faster with WGM22:0 set. (hmmmm...)

Output pins are in "normal" mode i.e. disconnected from the timer, when COM2A1:0 = 0x00 and
COM2B1:0 = 0x00. This is the default value at startup.

*/
   TCCR2A |= 3; // binary 00000011
   TCCR2B |= 8; // binary 00001000

/*
Enable OCF2A flag to generate the ISR(TIMER2_COMPA_vect){} interrupt on a match between TCNT2 and
OCR2A (the counter "TOP" value). To enable the OCF2A flag interrupt, enable the OCIE2A (Timer2
Output Compare Match A Enable) bit. The OCIE2A bit is located at bit position 1 in TMSK2.
*/
    TIMSK2 |= 2; // binary 00000010
// set global interrupts enable
    sei(); 

}


// Aliases the main worker functio n, TinerScheduler::onTickService(),
// to clock_onUnitTick(), which will be called by the ISR

void clock_attachUnitRolloverEvent(void (*function)(void))
{
    clock_onUnitTick = function;
}

void clock_attachIOpin(uint8_t channel, int pinNum) {
    clock_timerChannel[channel].arduinoPinNumber = pinNum;
}

// enables timer's run flag & isr updates
void clock_start(uint8_t channel) {
    clock_timerChannel[channel].runStopHtL = RUN; //runStop = RUN;
    clock_timerChannel[channel].enabled = ENABLED;  // timer ENABLE
}

// completely skip this timer in isr update
void clock_stop(uint8_t channel) { 
    clock_timerChannel[channel].enabled = DISABLED;  // timer ENABLE
}

void clock_setMasterPeriod(uint8_t mperiod){
    OCR2A = mperiod;
}

// stop and zero out everything
void clock_reset(uint8_t channel) {
    clock_timerChannel[channel].enabled == DISABLED;
    clock_timerChannel[channel].runStopLtH = STOP;
    clock_timerChannel[channel].timerCountLtH = 0;
    clock_timerChannel[channel].timerCountHtL = 0;
    clock_timerChannel[channel].timerSetpointLtH = 0;
    clock_timerChannel[channel].timerSetpointHtL = 0;    
    clock_timerChannel[channel].callbackFlag = CLEAR;
    clock_timerChannel[channel].userFlag = CLEAR;
    clock_timerChannel[channel].astableMonostable = ASTABLE;   
}

// timer keeps running, but counters & flags are cleared -- for 
// syncing running timers, etc
void clock_clearCount(uint8_t channel) {
    clock_timerChannel[channel].timerCountHtL = 0;
    clock_timerChannel[channel].timerCountLtH = 0;    
    clock_timerChannel[channel].callbackFlag = CLEAR;
    clock_timerChannel[channel].userFlag = CLEAR;
    clock_timerChannel[channel].astableMonostable = ASTABLE; // ?
}


void clock_writeSetPointHtL(uint8_t channel, uint16_t value) {
    clock_timerChannel[channel].timerSetpointHtL = value;
}

void clock_writeSetPointLtH(uint8_t channel, uint16_t value) {
    clock_timerChannel[channel].timerSetpointLtH = value;
}

void clock_astable(uint8_t channel, uint8_t flag) { //values are ASTABLE and MONOSTABLE
    clock_timerChannel[channel].astableMonostable = flag;
}

void clock_mono(uint8_t channel, uint16_t value){
    clock_timerChannel[channel].astableMonostable = MONOSTABLE;
    clock_writeSetPointHtL(channel, value);
    clock_start(channel);
}

// only for setting flags and doing callbacks -- not for attached pins
void clock_repeat(uint8_t channel, uint16_t value) {
    clock_timerChannel[channel].timerSetpointHtL = value;
    clock_timerChannel[channel].timerSetpointLtH = 0;
}

uint8_t clock_checkUserFlag(uint8_t channel) {
    if(clock_timerChannel[channel].userFlag){
        clock_timerChannel[channel].userFlag = CLEAR;
        return SET;
    }
    return CLEAR;
}

uint8_t clock_checkCallbackFlag(uint8_t channel) {
    if(clock_timerChannel[channel].callbackFlag){
        clock_timerChannel[channel].callbackFlag = CLEAR;
        return SET;
    }
    return CLEAR;
}


void clock_set_bits_func_correct (volatile uint8_t *port, uint8_t mask){
    *port |= mask;
}

void clock_clear_bits_func_correct (volatile uint8_t *port, uint8_t mask){
    *port &= ~(mask);
}


// clears everything

ISR(TIMER2_COMPA_vect){ 
//ISR(TIMER2_OVF_vect) {

    uint8_t channel;
    int pinNumScratch;

    //clock_ticks++; // IRQ counter -- sort of like the millis
    //in millis()
    
    for (channel=0; channel<16; channel++) {//outer for
        // core of the ISR
        
        pinNumScratch = clock_timerChannel[channel].arduinoPinNumber;

/*
The user starts a monostable or an astable count sequence by setting
the runStopHtL RUN flag. The HtL counter stops itself by clearing its own RUN 
flag upon a match between its setpoint and the counter values. 

If the user has set the counter unit
to astable mode (astableMonoststable flag = ASTABLE), the LtH counter will
automatically set the runStopHtL flag. The counters alternately enable each
other in an endless loop. This repeatedly recycles the timer unit.
*/

        // This timer unit is turned on -- it'll get service in the isr.
        // If timer setpoint is zero, then don't run.
        if ((clock_timerChannel[channel].enabled == ENABLED) && 
                (clock_timerChannel[channel].timerSetpointHtL >0)) { //if- chan enab?
            
            // We're in HtL. Do we go now?
            if ((clock_timerChannel[channel].runStopHtL == RUN) ) {// HtL = run?

                // HtL counter is in control of the unit until HtL
                // reaches its setpoint
                if (clock_timerChannel[channel].timerCountHtL <
                        clock_timerChannel[channel].timerSetpointHtL -1){ // incr cnt// try adding"-1"

                    clock_timerChannel[channel].timerCountHtL++;
                    //set output pin high -- it's cheaper to do
                    // this every time than to test the
                    // pin for being high before setting it high
                    
                    if (pinNumScratch > 0) { //0 means no pin assigned
                        clock_set_bits_func_correct((pin_to_port[pinNumScratch]),
                            digital_pin_to_bit_mask[pinNumScratch]);
                    } // pin assigned?
                } // incr cnt
                else { // HtL == expired

                    clock_timerChannel[channel].runStopHtL = STOP;
                    //disable self
                    clock_timerChannel[channel].timerCountHtL = 0; //clear counter
                    
                    // user flag to read and then clear inside
                    // the Arduino sketch
                    clock_timerChannel[channel].userFlag = SET;
                    
                    // callback function will run
                    clock_timerChannel[channel].callbackFlag = SET; 
                    if(pinNumScratch > 0) { // 0 means no pin assigned
                        //set output pin low
                       
                        clock_clear_bits_func_correct((pin_to_port[pinNumScratch]),
                                                     
                        digital_pin_to_bit_mask[pinNumScratch]);
                    } // pin assigned?

                    // enable LtH (the other counter)
                    // It's cheaper to always do it than to check first
                    clock_timerChannel[channel].runStopLtH = RUN;
                }// HtL == expired
            } // HtL = run?

            else if (clock_timerChannel[channel].astableMonostable == ASTABLE) { // astable/mono
                
                //The runStopLtH flag can be set only by the HtL counter.
                if (clock_timerChannel[channel].runStopLtH == RUN) { // run LtH
                    // We're LtH now.
                    if (clock_timerChannel[channel].timerCountLtH < 
                        clock_timerChannel[channel].timerSetpointLtH-1) { // incr LtH? // try adding"-1"
                        (clock_timerChannel[channel].timerCountLtH)++;
                    }// incr LtH?
                    else {     // LtH done                    
                        if (pinNumScratch > 0) { //0 means no pin assigned
                            clock_set_bits_func_correct((pin_to_port[pinNumScratch]),
                               digital_pin_to_bit_mask[pinNumScratch]);
                        } // pin > 0?
                        clock_timerChannel[channel].runStopLtH = STOP;
                        clock_timerChannel[channel].timerCountLtH = 0; // clear counter
                        // enable HtL (the other) counter
                        clock_timerChannel[channel].runStopHtL = RUN; 

                    }// else- LtH done
                } //if- run LtH?
            }//if- astable/mono?
        } //if- chan enab?
    } // outer for
 
    // HELLO! 
    clock_onUnitTick();  // call user defined callback
}

