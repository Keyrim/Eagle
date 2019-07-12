/* 
TimerWithFlag_48.pde Copyright (c) 2010 Ed Bennett.  All rights reserved.

Using flags in loop() --first example with fixed speeds. Three LED's blinking together at different rates. Three timer channels are used. The channels are numbered 0, 1, and 2. 

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
// Timer commands usually take a channel number and/or a 
// time-value. 
//
// There are three ways to access a timer. (1) pin-direct,
// (2) using flags, and (3) using callbacks. This example
// uses flags. Each timer has a flag. Flags are "set" (made TRUE)
// when a timer reaches the end of its count interval. 
// Timer_repeat(1, 200) gives Timer 1 a count interval of 200.
// Timer intervals can be up to 65535.
// 
// Reading a flag tells you if the timer has fired off so you can do 
// something useful. If its flag is set when you check (poll)
// timer 3, "Timer.flagIsSet(3)" will be TRUE. The flag is then
// automatically cleared to be ready for the next cycle.
// 

// The unit of time is is called a blink. The duration 
// of a blink is related to an AVR jiffy. See
// http://en.wikipedia.org/wiki/Jiffy_%28time%29
//


#include <TimerScheduler.h>
#include <TimerRepeat.h>

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
  Timer.setMasterPeriod(50); // must follow Timer.begin()
  
  
  Timer_repeat(0, 100); // (channel, intercval)
  
  Timer_repeat(1, 200);
  
  Timer_repeat(2, 400);
  
  Timer_repeat(3, 800);
  
  Timer_repeat(4, 1600);
  
  Timer_repeat(5, 3200);
  
  Timer_repeat(6, 6400);
  
  Timer_repeat(7, 12800);
  
  Timer_repeat(8, 25600);
  
  Timer_repeat(9, 51200);
  
  
  

  
  Timer.start(0);       // (channel)
  Timer.start(1); 
  Timer.start(2); 
  Timer.start(3); 
  Timer.start(4);
  Timer.start(5);       // (channel)
  Timer.start(6); 
  Timer.start(7); 
  Timer.start(8); 
  Timer.start(9);
  
  
;  
}

  // this is how to poll a timer flag
void loop()
{
  // for LED0
  if(Timer.flagIsSet(0)){  // channel 0 flag
    outval0 ^= 1;          // toggle the pin
    digitalWrite(LED0, outval0);
  }

  // for LED1   
  if(Timer.flagIsSet(1)){  // channel 1 flag
    outval1 ^= 1;
    digitalWrite(LED1, outval1);
  }

  //for LED2  
  if(Timer.flagIsSet(2)){  // channel 2 flag
    outval2 ^= 1;
    digitalWrite(LED2, outval2);
  }
  
    //for LED3  
  if(Timer.flagIsSet(3)){  // channel 3 flag
    outval3 ^= 1;
    digitalWrite(LED3, outval3);
  }
    //for LED4  
  if(Timer.flagIsSet(4)){  // channel 4 flag
    outval4 ^= 1;
    digitalWrite(LED4, outval4);
  }
  
  // for LED5
  if(Timer.flagIsSet(5)){  // channel 0 flag
    outval5^= 1;          // toggle the pin
    digitalWrite(LED5, outval5);
  }

  // for LED6   
  if(Timer.flagIsSet(6)){  // channel 6 flag
    outval6 ^= 6;
    digitalWrite(LED6, outval6);
  }

  //for LED7  
  if(Timer.flagIsSet(7)){  // channel 7 flag
    outval7 ^= 1;
    digitalWrite(LED7, outval7);
  }
  
    //for LED8  
  if(Timer.flagIsSet(8)){  // channel 8 flag
    outval8 ^= 1;
    digitalWrite(LED8, outval8);
  }
    //for LED9  
  if(Timer.flagIsSet(9)){  // channel 9 flag
    outval9 ^= 1;
    digitalWrite(LED9, outval9);
  }
  
}

