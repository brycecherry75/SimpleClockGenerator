/*

   SimpleClockGenerator demo for Arduno Uno and compatibles by Bryce Cherry

   Commands:
   START pin frequency - starts clock output on a given pin
   (DIVIDE_EXT/DIVIDER) pin value - DIVIDE_EXT divides clock output at the counter/divider input T pin and outputs it to a pin on the same counter/divider while DIVIDER only divides the value + 1 at the current pin
   RESUME pin - resumes clock output on a given pin
   STOP pin - stops clock output on a given pin
   READ pin - reads the current divider value on a given set of pins
   INCREMENT pin value - increments divider on a given set of pins by a given value
   DECREMENT pin value - decrements divider on a given set of pins by a given value
   PRESCALER pin (1/8/32/64/128/256/1024) - sets internal clock prescaler on a given set of pins
   RESTART_MILLIS_MICROS (DELAY_TEST) - disables clock output on pins used by Timer 0 and restores millis()/micros()/delay() - DELAY_TEST tests delay()

*/

#if (!defined(__AVR_ATmega328P__) && !defined(__AVR_ATmega168__) && !defined(__AVR_ATmega88__) && !defined(__AVR_ATmega48__))
#error This demo sketch can only run on Uno or derivatives
#endif

#include <SimpleClockGenerator.h>

// ensures that the serial port is flushed fully on request
const unsigned long SerialPortRate = 9600;
const byte SerialPortRateTolerance = 5; // percent - increase to 50 for rates above 115200 up to 4000000
const byte SerialPortBits = 10; // start (1), data (8), stop (1)
const unsigned long TimePerByte = ((((1000000ULL * SerialPortBits) / SerialPortRate) * (100 + SerialPortRateTolerance)) / 100); // calculated on serial port rate + tolerance and rounded down to the nearest uS, long caters for even the slowest serial port of 75 bps

const byte commandSize = 50;
char command[commandSize];
const byte FieldSize = 25;

void FlushSerialBuffer() {
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

void getField (char* buffer, int index) {
  int CommandPos = 0;
  int FieldPos = 0;
  int SpaceCount = 0;
  while (CommandPos < commandSize) {
    if (command[CommandPos] == 0x20) {
      SpaceCount++;
      CommandPos++;
    }
    if (command[CommandPos] == 0x0D || command[CommandPos] == 0x0A) {
      break;
    }
    if (SpaceCount == index) {
      buffer[FieldPos] = command[CommandPos];
      FieldPos++;
    }
    CommandPos++;
  }
  for (int ch = 0; ch < strlen(buffer); ch++) { // correct case of command
    buffer[ch] = toupper(buffer[ch]);
  }
  buffer[FieldPos] = '\0';
}

void setup() {
  Serial.begin(SerialPortRate);
}

void loop() {
  static int ByteCount = 0;
  if (Serial.available() > 0) {
    char inData = Serial.read();
    if (inData != '\n' && ByteCount < commandSize) {
      command[ByteCount] = inData;
      ByteCount++;
    }
    else {
      ByteCount = 0;
      bool ValidField = true;
      char field[FieldSize];
      getField(field, 0);
      if (strcmp(field, "START") == 0) {
        getField(field, 1);
        byte pin = atoi(field);
        if (pin == 6 || pin == 5 || pin == 9 || pin == 10 || pin == 11 || pin == 3) { // must be an OCxx pin
          getField(field, 2);
          unsigned long frequency = atol(field);
          SimpleClockGenerator.init(pin);
          unsigned long ActualFrequency = SimpleClockGenerator.start(pin, frequency);
          Serial.print(F("Actual frequency is "));
          Serial.print(ActualFrequency);
          Serial.println(F(" Hz"));
        }
        else {
          ValidField = false;
        }
      }
      else if (strcmp(field, "DIVIDE_EXT") == 0 || strcmp(field, "DIVIDER") == 0) {
        bool InitExternalDivider = true;
        if (strcmp(field, "DIVIDER") == 0) {
          InitExternalDivider = false;
        }
        getField(field, 1);
        byte pin = atoi(field);
        getField(field, 2);
        unsigned long DividerValue = atol(field);
        if (((pin != 9 && pin != 10 && DividerValue > 255) || DividerValue > 65535UL) || ((pin == 11 || pin == 3) && InitExternalDivider == true)) { // Timer/Counter 1 is 16 bit (all others are 8 bit), T2 is not fitted to Timer/Counter 2 (OC2A/OC2B pins 11/3)
          ValidField = false;
        }
        if (InitExternalDivider == true) {
          SimpleClockGenerator.initDividerAtTpin(pin);
        }
        SimpleClockGenerator.writeDivider(pin, DividerValue);
        if (InitExternalDivider == true) {
          SimpleClockGenerator.resume(pin);
        }
      }
      else if (strcmp(field, "RESUME") == 0) {
        getField(field, 1);
        byte pin = atoi(field);
        if (pin == 6 || pin == 5 || pin == 9 || pin == 10 || pin == 11 || pin == 3) { // must be an OCxx pin
          SimpleClockGenerator.resume(pin);
        }
        else {
          ValidField = false;
        }
      }
      else if (strcmp(field, "STOP") == 0) {
        getField(field, 1);
        byte pin = atoi(field);
        if (pin == 6 || pin == 5 || pin == 9 || pin == 10 || pin == 11 || pin == 3) { // must be an OCxx pin
          SimpleClockGenerator.stop(pin);
        }
        else {
          ValidField = false;
        }
      }
      else if (strcmp(field, "READ") == 0) {
        getField(field, 1);
        byte pin = atoi(field);
        if (pin == 6 || pin == 5 || pin == 9 || pin == 10 || pin == 11 || pin == 3) { // must be an OCxx pin
          unsigned long DividerValue = SimpleClockGenerator.readDivider(pin);
          DividerValue++;
          word PrescalerValue = SimpleClockGenerator.readPrescaler(pin);
          Serial.print(F("Divider value is "));
          Serial.println(DividerValue);
          Serial.print(F("Prescaler value is "));
          Serial.println(PrescalerValue);
          DividerValue *= PrescalerValue;
          DividerValue *= 2; // toggle halves frequency
          Serial.print(F("Total division value including half frequency toggle is "));
          Serial.println(DividerValue);
        }
        else {
          ValidField = false;
        }
      }
      else if (strcmp(field, "INCREMENT") == 0) {
        getField(field, 1);
        byte pin = atoi(field);
        getField(field, 2);
        unsigned long value = atol(field);
        if (((pin == 3 || pin == 5 || pin == 6 || pin == 11) && value <= 255) || ((pin == 9 || pin == 10) && value <= 65535)) {
          SimpleClockGenerator.incrementDivider(pin, value);
        }
        else {
          ValidField = false;
        }
      }
      else if (strcmp(field, "DECREMENT") == 0) {
        getField(field, 1);
        byte pin = atoi(field);
        getField(field, 2);
        unsigned long value = atol(field);
        if (((pin == 3 || pin == 5 || pin == 6 || pin == 11) && value <= 255) || ((pin == 9 || pin == 10) && value <= 65535)) {
          SimpleClockGenerator.decrementDivider(pin, value);
        }
        else {
          ValidField = false;
        }
      }
      else if (strcmp(field, "PRESCALER") == 0) {
        getField(field, 1);
        byte pin = atoi(field);
        getField(field, 2);
        unsigned long value = atol(field);
        if (((pin == 3 || pin == 11) && (value == 32 || value == 128)) || value == 1 || value == 8 || value == 64 || value == 256 || value == 1024) {
          SimpleClockGenerator.setPrescaler(pin, value);
        }
        else {
          ValidField = false;
        }
      }
      else if (strcmp(field, "RESTART_MILLIS_MICROS") == 0) {
        getField(field, 1);
        if (strcmp(field, "DELAY_TEST") == 0) {
          Serial.println(F("Start of delay"));
          delay(5000);
          Serial.println(F("If you see this line immediately after the previous, delay() is inoperative"));
        }
        Serial.print(F("millis() is currently "));
        Serial.println(millis());
        for (int i = 0; i < 500; i++) {
          delayMicroseconds(10000);
        }
        Serial.println(F("If you see this line 5 seconds after the previous, delayMicroseconds() is functional"));
        Serial.print(F("millis() is now "));
        Serial.println(millis());
        SimpleClockGenerator.RestartMillisMicros();
        delay(5000);
        Serial.print(F("millis() is now "));
        Serial.println(millis());
        Serial.println(F("If you see the above line 5 seconds from the previous line, millis()/micros()/delay() has been successfully restored"));
      }
      else {
        ValidField = false;
      }
      FlushSerialBuffer();
      if (ValidField == true) {
        Serial.println(F("OK"));
      }
      else {
        Serial.println(F("ERROR"));
      }
    }
  }
}