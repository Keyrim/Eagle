/*
TimerPinDirect_48.pde -- timers act directly on pins --first example with fixed speeds

Copyright (c) 2010 Ed Bennett.  All rights reserved. 

This program is free software: you can redistribute it and/or modify
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


// Timer commands usually take a channel number and/or a 
// time-value. 
//
// The unit of time is is called a blink. The duration 
// of a blink is related to an AVR jiffy. See
// http://en.wikipedia.org/wiki/Jiffy_%28time%29
//
// There are three ways to access a timer. (1) pin-direct,
// (2) using flags, and (3) using callbacks
//
// This example uses direct control of the pins from the timer channels.
// This method uses less processor power than callbacks or flag polling.
// (see the other examples)


#include <TimerScheduler.h>

// hardware pins
int LED0 = 13;
int LED1 = 12;
int LED2 = 11;
int LED3 = 10;
int LED4 = 9;
int LED5 = 8;
int LED6 = 7;
int LED7 = 6;
int LED8 = 5;
int LED9 = 4;

int outval0 = 0;
int outval1 = 0;
int outval2 = 0;
int outval3 = 0;
int outval4 = 0;
int outval5 = 0;
int outval6 = 0;
int outval7 = 0;
int outval8 = 0;
int outval9 = 0;

void setup()
{   
  pinMode(LED0, OUTPUT); 
  pinMode(LED1, OUTPUT); 
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT); 
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT); 
  pinMode(LED6, OUTPUT); 
  pinMode(LED7, OUTPUT);
  pinMode(LED8, OUTPUT); 
  pinMode(LED9, OUTPUT);
  
  Timer.begin();        // start the hardware timer
  
  // attach LED pins to timer channels
  Timer.attachPin(0, LED0); // (channel, value)
  Timer.attachPin(1, LED1);
  Timer.attachPin(2, LED2); 
  Timer.attachPin(3, LED3);
  Timer.attachPin(4, LED4);
  Timer.attachPin(5, LED5);
  Timer.attachPin(6, LED6);
  Timer.attachPin(7, LED7); 
  Timer.attachPin(8, LED8);
  Timer.attachPin(9, LED9);
  
  // using pin-direct mode, you need to set the pin-high
  // and pin-low times seperately
  Timer.setHighTime(0, 10); // (channel, value)
  Timer.setLowTime(0, 10); 
  
  Timer.setHighTime(1,20);
  Timer.setLowTime(1, 20);
  
  Timer.setHighTime(2,40);
  Timer.setLowTime(2, 40);
  
  Timer.setHighTime(3,80);
  Timer.setLowTime(3, 80);
  
  Timer.setHighTime(4,160);
  Timer.setLowTime(4, 160);
  
  Timer.setHighTime(5, 320); 
  Timer.setLowTime(5, 320); 
  
  Timer.setHighTime(6,640);
  Timer.setLowTime(6, 640);
  
  Timer.setHighTime(7,1280);
  Timer.setLowTime(7, 1280);
  
  Timer.setHighTime(8,2560);
  Timer.setLowTime(8, 2560);
  
  Timer.setHighTime(9,5120);
  Timer.setLowTime(9, 5120);
  
  Timer.start(0);       // (channel)
  Timer.start(1); 
  Timer.start(2); 
  Timer.start(3); 
  Timer.start(4);
  Timer.start(5);       
  Timer.start(6); 
  Timer.start(7); 
  Timer.start(8); 
  Timer.start(9);
  
}


// You can put things in loop(), if you like,
// but it's not required.

void loop()
{
}
