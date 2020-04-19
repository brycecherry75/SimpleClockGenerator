/*
   SimpleCLK demo for Arduno Uno and compatibles by Bryce Cherry
   To proceed to the next stage, press any key
*/

#include <SimpleClockGenerator.h>

// ensures that the serial port is flushed fully on request
const unsigned long SerialPortRate = 9600;
const byte SerialPortRateTolerance = 5; // percent - increase to 50 for rates above 115200 up to 4000000
const byte SerialPortBits = 10; // start (1), data (8), stop (1)
const unsigned long TimePerByte = ((((1000000ULL * SerialPortBits) / SerialPortRate) * (100 + SerialPortRateTolerance)) / 100); // calculated on serial port rate + tolerance and rounded down to the nearest uS, long caters for even the slowest serial port of 75 bps

void setup() {
  Serial.begin(SerialPortRate);
}

void loop() {
  Serial.print(F("millis() is currently "));
  Serial.println(millis());
  SimpleCLK.init(6);
  SimpleCLK.init(5);
  SimpleCLK.init(9);
  SimpleCLK.init(10);
  SimpleCLK.init(11);
  SimpleCLK.init(3);
  Serial.println(F("Pins 6/9/11 are now in Hz:"));
  Serial.println((SimpleCLK.start(6, 250000)));
  Serial.println((SimpleCLK.start(9, 38000)));
  Serial.println((SimpleCLK.start(11, 455000)));
  for (int i = 0; i < 100; i++) {
    delayMicroseconds(10000);
  }
  Serial.println(F("If you see this line one second after the previous, delayMicroseconds() is functional"));
  WaitForSerialActivity();
  SimpleCLK.stop(6);
  SimpleCLK.stop(9);
  SimpleCLK.stop(11);
  Serial.println(F("Stopping clock on all pins"));
  WaitForSerialActivity();
  SimpleCLK.resume(6);
  SimpleCLK.resume(9);
  SimpleCLK.resume(11);
  Serial.println(F("Resuming clock on all pins"));
  WaitForSerialActivity();
  SimpleCLK.stop(9);
  Serial.println(F("Stopping clock on pin 9"));
  WaitForSerialActivity();
  Serial.println(F("Pin 9 is now in Hz: "));
  Serial.println((SimpleCLK.start(9, 256)));
  WaitForSerialActivity();
  SimpleCLK.resume(5);
  SimpleCLK.resume(10);
  SimpleCLK.resume(3);
  Serial.println(F("Pins 6/5 and 9/10 and 11/3 now have identical frequencies"));
  WaitForSerialActivity();
  SimpleCLK.stop(6);
  SimpleCLK.stop(9);
  SimpleCLK.stop(11);
  Serial.println(F("Pins 5/10/3 now only have clock output"));
  WaitForSerialActivity();
  SimpleCLK.RestartMillisMicros();
  Serial.println(F("Stopping clock on Pin 5 and restoring millis() and micros()"));
  Serial.print(F("millis() is now "));
  Serial.println(millis());
  delay(5000);
  Serial.println(F("If you see this line 5 seconds from the previous line, millis() / micros() / delay() has been fully restored"));
  WaitForSerialActivity();
}

void WaitForSerialActivity() {
  while (Serial.available() == 0) {
  }
  while (true) {
    if (Serial.available() > 0) {
      byte dummy = Serial.read();
      while (Serial.available() > 0) { // flush additional bytes from serial buffer if present
        dummy = Serial.read();
      }
      if (TimePerByte <= 16383) {
        delayMicroseconds(TimePerByte); // delay in case another byte may be received via the serial port
      }
      else { // deal with delayMicroseconds limitation
        unsigned long DelayTime = TimePerByte;
        DelayTime /= 1000;
        if (DelayTime > 0) {
          delay(DelayTime);
        }
        DelayTime = TimePerByte;
        DelayTime %= 1000;
        if (DelayTime > 0) {
          delayMicroseconds(DelayTime);
        }
      }
    }
    else {
      break;
    }
  }
}