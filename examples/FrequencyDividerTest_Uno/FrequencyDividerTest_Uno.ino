#include <SimpleClockGenerator.h>

void setup() {
  pinMode(9, OUTPUT); // OCR1A divided frequency output
  SimpleCLK.init(11);
  SimpleCLK.start(11, 1000000); // feed this output to Pin 5 (PD5/T1 - becomes an input on reset)
  TIMSK1 = 0; // no interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = 499; // 1000 kHz / (OCR1A + 1) / 2 = 1 kHz 50% duty cycle at OCR1A pin
  TCCR1B = 0b00001111; // WGM12/13 = 1/0 - CTC on OCR1A, rising edge clock on T1 pin
  TCCR1A = 0b01000000; // CTC toggle - WGM10/11 = 0/0 - CTC on OCR1A
}

void loop() {
}