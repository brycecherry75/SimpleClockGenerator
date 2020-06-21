#include "SimpleClockGenerator.h"

// Arduino Uno, Duemilanove, Diecimila, LilyPad, Mini, Fio, etc
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega88__) || defined(__AVR_ATmega88__) || defined(__AVR_ATmega48__)

void SimpleClockGeneratorClass::init(uint8_t pin) {
  if (pin == 6 || pin == 5) { // OC0 - 8-bit - will stop millis() and micros() functions and disable delay() but not delayMicroseconds()
    if (MillisMicrosStopped == false) {
      MillisMicrosStopped = true;
      Old_TIMSK0 = TIMSK0;
      TIMSK0 = 0; // disabling the interrupt first will ensure proper restoration
      Old_TIFR0 = TIFR0;
      TIFR0 = 0;
      Old_OCR0B = OCR0B;
      OCR0B = 0;
      Old_OCR0A = OCR0A;
      OCR0A = 0;
      Old_TCNT0 = TCNT0;
      TCNT0 = 0;
      Old_TCCR0B = TCCR0B;
      TCCR0B = 0;
      Old_TCCR0A = TCCR0A;
      TCCR0A = 0;
    }
    pinMode(pin, OUTPUT);
    TIMSK0 = 0; // no interrupts
    TCCR0A = 0;
    TCCR0B = 0;
    TCCR0A = 0b00000010; // CTC toggle on OCR0A
  }
  else if (pin == 9 || pin == 10) { // OC1 - 16 bit
    pinMode(pin, OUTPUT);
    TIMSK1 = 0; // no interrupts
    TCCR1A = 0;
    TCCR1B = 0; // WGM10/11 = 0/0 - CTC on OCR1A
    TCCR1B = 0b00001000; // WGM12/13 = 1/0 - CTC on OCR1A
  }
  else if (pin == 11 || pin == 3) { // OC2 - 8 bit
    pinMode(pin, OUTPUT);
    TIMSK2 = 0; // no interrupts
    TCCR2A = 0;
    TCCR2A = 0b00000010; // CTC toggle on OCR2A
    TCCR2B = 0;
  }
}

uint32_t SimpleClockGeneratorClass::start(uint8_t pin, uint32_t frequency) {
  unsigned long ActualFrequency = 0;
  unsigned long FrequencyHighCount = F_CPU;
  FrequencyHighCount /= 2; // toggle frequency is half CPU speed
  if (frequency <= FrequencyHighCount) { // deal with exceeding CPU frequency
    unsigned long FrequencyLowCount;
    FrequencyHighCount /= frequency; // rounds down
    FrequencyLowCount = FrequencyHighCount;
    FrequencyLowCount++; // rounds up
    unsigned long ActualFrequencyHigh;
    unsigned long ActualFrequencyLow;
    ActualFrequencyHigh = F_CPU;
    ActualFrequencyHigh /= 2; // toggle frequency is half CPU speed
    ActualFrequencyLow = ActualFrequencyHigh;
    ActualFrequencyHigh /= FrequencyHighCount;
    ActualFrequencyLow /= FrequencyLowCount;
    unsigned long Divider;
    // Compare rounded down value with rounded down value + 1 to check for closest match to target - deal with exceeding half CPU MHz
    // If output is outside F_CPU / 2 / OCx for a given prescaler and the other OCx is set for a frequency in a different prescaler, the first OCx frequency will be set to the new prescaler
    if ((ActualFrequencyHigh - frequency) < (frequency - ActualFrequencyLow)) { // choose the closest possible match
      Divider = FrequencyHighCount;
    }
    else {
      Divider = FrequencyLowCount;
    }
    byte Prescaler = 1;
    unsigned int PrescalerRatio = 1;
    if (pin == 6 || pin == 5) { // Timer 0 - 8 bit with prescaler ratios of 1/8/64/256/1024 available
      if (Divider > 256ULL && Divider <= 2048ULL) { // prescaler 8
        Prescaler = 2;
        PrescalerRatio = 8;
      }
      else if (Divider > 2048ULL && Divider <= 16384ULL) { // prescaler 64
        Prescaler = 3;
        PrescalerRatio = 64;
      }
      else if (Divider > 16384ULL && Divider <= 65536ULL) { // prescaler 256
        Prescaler = 4;
        PrescalerRatio = 256;
      }
      else if (Divider > 65536ULL && Divider <= 262144ULL) { // prescaler 1024
        Prescaler = 5;
        PrescalerRatio = 1024;
      }
    }
    else if (pin == 11 || pin == 3) { // Timer 2 - 8 bit which also supports prescaler ratios of 32/128
      if (Divider > 256ULL && Divider <= 2048ULL) { // prescaler 8
        Prescaler = 2;
        PrescalerRatio = 8;
      }
      else if (Divider > 2048ULL && Divider <= 8192ULL) { // prescaler 32
        Prescaler = 3;
        PrescalerRatio = 32;
      }
      else if (Divider > 8192ULL && Divider <= 16384ULL) { // prescaler 64
        Prescaler = 4;
        PrescalerRatio = 64;
      }
      else if (Divider > 8192ULL && Divider <= 32768ULL) { // prescaler 128
        Prescaler = 5;
        PrescalerRatio = 128;
      }
      else if (Divider > 32768ULL && Divider <= 65536ULL) { // prescaler 256
        Prescaler = 6;
        PrescalerRatio = 256;
      }
      else if (Divider > 65536ULL && Divider <= 262144ULL) { // prescaler 1024
        Prescaler = 7;
        PrescalerRatio = 1024;
      }
    }
    else if (pin == 9 || pin == 10) { // 16 bit with prescaler ratios of 1/8/64/256/1024 available
      if (Divider > 65536ULL && Divider <= 524288ULL) { // prescaler 8
        Prescaler = 2;
        PrescalerRatio = 8;
      }
      else if (Divider > 524288ULL && Divider <= 4194304ULL) { // prescaler 64
        Prescaler = 3;
        PrescalerRatio = 64;
      }
      else if (Divider > 4194304ULL && Divider <= 16777216ULL) { // prescaler 256
        Prescaler = 4;
        PrescalerRatio = 256;
      }
      else if (Divider > 16777216ULL && Divider <= 67108864ULL) { // prescaler 1024
        Prescaler = 5;
        PrescalerRatio = 1024;
      }
    }
    Divider /= PrescalerRatio;
    // recalculate the actual frequency after going through the prescaler if used
    ActualFrequency = F_CPU;
    ActualFrequency /= 2;
    ActualFrequency /= PrescalerRatio;
    ActualFrequency /= Divider;
    Divider--; // Frequency = F_CPU / 2 / (Divider + 1) / Prescaler
    if (Divider <= 255 && (pin == 6 || pin == 5 || pin == 11 || pin == 3)) { // 8 bit divider
      if (pin == 6 || pin == 5) {
        TCCR0B = 0; // reset prescaler
        OCR0A = Divider;
        TCCR0B |= Prescaler;
        if (pin == 6) {
          TCCR0A |= 0b01000000;
        }
        else {
          TCCR0A |= 0b00010000;
        }
      }
      else if (pin == 11 || pin == 3) {
        TCCR2B = 0; // reset prescaler
        OCR2A = Divider;
        TCCR2B |= Prescaler;
        if (pin == 11) {
          TCCR2A |= 0b01000000;
        }
        else {
          TCCR2A |= 0b00010000;
        }
      }
    }
    else if (Divider <= 65535 && (pin == 9 || pin == 10)) { // 16 bit divider
      TCCR1B &= 0b11111000; // reset prescaler
      OCR1A = Divider;
      TCCR1B |= Prescaler;
      if (pin == 9) {
        TCCR1A |= 0b01000000;
      }
      else {
        TCCR1A |= 0b00010000;
      }
    }
  }
  return ActualFrequency;
}

void SimpleClockGeneratorClass::resume(uint8_t pin) {
  switch (pin) {
    case 6: // OC0A
      TCCR0A |= 0b01000000; // CTC toggle
      break;
    case 5: // OC0A
      TCCR0A |= 0b00010000; // CTC toggle
      break;
    case 9: // OC1A
      TCCR1A |= 0b01000000; // CTC toggle
      break;
    case 10: // OC1A
      TCCR1A |= 0b00010000; // CTC toggle
      break;
    case 11: // OC2A
      TCCR2A |= 0b01000000; // CTC toggle
      break;
    case 3: // OC2A
      TCCR2A |= 0b00010000; // CTC toggle
      break;
  }
}

void SimpleClockGeneratorClass::stop(uint8_t pin) {
  switch (pin) {
    case 6: // OC0A
      TCCR0A &= 0b00111111; // disconnect the pin
      break;
    case 5: // OC0A
      TCCR0A &= 0b11001111; // disconnect the pin
      break;
    case 9: // OC1A
      TCCR1A &= 0b00111111; // disconnect the pin
      break;
    case 10: // OC1A
      TCCR1A &= 0b11001111; // disconnect the pin
      break;
    case 11: // OC2A
      TCCR2A &= 0b00111111; // disconnect the pin
      break;
    case 3: // OC2A
      TCCR2A &= 0b11001111; // disconnect the pin
      break;
  }
}

void SimpleClockGeneratorClass::RestartMillisMicros() { // will start millis() and micros() functions from the count at the time of disabling
  TCCR0A = Old_TCCR0A;
  TCCR0B = Old_TCCR0B;
  TCNT0 = Old_TCNT0;
  OCR0A = Old_OCR0A;
  OCR0B = Old_OCR0B;
  TIFR0 = Old_TIFR0;
  TIMSK0 = Old_TIMSK0; // enabling the interrupt last will ensure proper restoration
  MillisMicrosStopped = false;
}

#else
#error "Unsupported chip, please edit SimpleClockGenerator library with timer+counter definitions"
#endif

SimpleClockGeneratorClass SimpleClockGenerator;