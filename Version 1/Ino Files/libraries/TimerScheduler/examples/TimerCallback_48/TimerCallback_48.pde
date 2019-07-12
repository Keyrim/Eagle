

/* svn 48
TimerCallback_48.pde -- Uses user-written functions outside of loop() --first example with fixed speeds.
 
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

//
//
// LED's blinking together at different rates.
// There are 16 timer channels total. This demo uses five.
//
// Timer commands usually take a channel number and/or a 
// time-value. 
//
// The unit of time is is called a blink. The duration 
// of a blink is related to an AVR jiffy. See
// http://en.wikipedia.org/wiki/Jiffy_%28time%29
// The duration of a blink is determined by the master 
// timer setting: Timer.setMasterPeriod(200);
//
// There are three ways to access a timer. (1) pin-direct,
// (2) using flags, and (3) using callbacks
//
// This example runs callback functions that you write. Your
// function is registered with a timer. When the timer fires off,
// it runs your function. Timer intervals can be up to 65535.
//
// Your callback functions should be very short and very fast. Don't
// use delay() or anything else that eats a lot of time. Serial.print()
// is very slow, for example. While your callback is running, the chip is 
// stuck on a very low level that affects other things. It's a good
// idea to add a blinking LED to your program just so that you can see
// whether it locks up due to a callback that runs too slowly.


#include <TimerScheduler.h>
#include <TimerRepeat.h>

// hardware pins
int LED0 = 13;
int LED1 = 12;
int LED2 = 11;
int LED3 = 10;
int LED4 = 9;
int LED5 = 8;

int outval0 = 0;
int outval1 = 0;
int outval2 = 0;
int outval3 = 0;
int outval4 = 0;
int outval5 = 0;


void setup()
{   
  pinMode(LED0, OUTPUT); 
  pinMode(LED1, OUTPUT); 
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT); 
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  
  // start the hardware timer
  Timer.begin();
  
  // Set the master timer. WARNING: a small number will hang
  // the cpu. The default is 200. Max is 255. The Minimum 
  // value I've tested is 20.
  Timer.setMasterPeriod(200); 
 
  // Register user's callback function names with their timer
  // channels. In this example, "blinkLED0" is a function I 
  // wrote further down on this page.

  Timer.onTick(0, blinkLED0); //(channelNumber, functionName)
  Timer.onTick(1, blinkLED1); 
  Timer.onTick(2, blinkLED2);  
  Timer.onTick(3, blinkLED3); 
  Timer.onTick(4, blinkLED4);
  Timer.onTick(5, blinkLED5);

  Timer_repeat(0, 100); //(channelNumber, repeatInterval)
  Timer_repeat(1, 200); 
  Timer_repeat(2, 400);
  Timer_repeat(3, 800);
  Timer_repeat(4, 1600);
  Timer_repeat(5, 3200);

  Timer.start(0);       // (channel)
  Timer.start(1); 
  Timer.start(2);
  Timer.start(3); 
  Timer.start(4); 
  Timer.start(5); 

}


// You can put things in loop(), if you like,
// but it's not required for the callbacks.

void loop()
{      
}

// Here are the user's callbacks. A callback function runs
// when its timer fires off.

// People keep asking about the ^= thing. It's a quick way
// to flip a bit. If the bit is "1" it becomes "0", and 
// vice versa. Great for blinkers.
void blinkLED0(void){
  outval0 ^= 1;          // toggle the pin
  digitalWrite(LED0, outval0);
}

void blinkLED1(void){
  outval1 ^= 1;
  digitalWrite(LED1, outval1);
}

void blinkLED2(void){
  outval2 ^= 1;
  digitalWrite(LED2, outval2);
}

void blinkLED3(void){
  outval3 ^= 1;
  digitalWrite(LED3, outval3);
}

void blinkLED4(void){
  outval4 ^= 1;
  digitalWrite(LED4, outval4);
}

void blinkLED5(void){
  outval5 ^= 1;
  digitalWrite(LED5, outval5);
}



