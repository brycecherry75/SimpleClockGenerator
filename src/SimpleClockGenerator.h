#ifndef SimpleClockGenerator_h
#define SimpleClockGenerator_h

#include <Arduino.h>

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega88__) || defined(__AVR_ATmega88__) || defined(__AVR_ATmega48__)
// Arduino Uno, Duemilanove, Diecimila, LilyPad, Mini, Fio, etc
class SimpleClockGeneratorClass {
  public:
    void init(uint8_t pin);
    uint32_t start(uint8_t pin, uint32_t frequency); // using Pin 5 or 6 will stop millis()/micros() function including delay()/delayMicroseconds()
    void stop(uint8_t pin);
    void resume(uint8_t pin);
    void RestartMillisMicros(); // re-enables millis()/micros()/delay() from its last count and delay()/delayMicroseconds()

    // registers for the previous value of millis()/micros() which uses Timer 0 should it be required again

    byte Old_TCCR0A;
    byte Old_TCCR0B;
    byte Old_TCNT0;
    byte Old_OCR0A;
    byte Old_OCR0B;
    byte Old_TIFR0;
    byte Old_TIMSK0;
    bool MillisMicrosStopped = false;

};

    // Arduino Leonardo etc
// #elif defined(__AVR_ATmega32U4__)

    // Arduino Mega etc
// #elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

#else
#error "Unknown chip, please edit SimpleClockGenerator library with timer+counter definitions"
#endif

extern SimpleClockGeneratorClass SimpleClockGenerator;

#endif