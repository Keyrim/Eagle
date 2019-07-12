
// there's an issue with the Timer.repeat() function.
// Until it's fixed, use this instead.
void Timer_repeat(uint8_t channel, uint16_t interval){
  if(interval < 1) interval =1;
  Timer.setHighTime(channel, interval-1);
  Timer.setLowTime(channel, 1);
}

