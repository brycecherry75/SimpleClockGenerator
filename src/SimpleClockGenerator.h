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
    void RestartMillisMicros();
};

extern SimpleClockGeneratorClass SimpleClockGenerator;

    // Arduino Leonardo etc
// #elif defined(__AVR_ATmega32U4__)

    // Arduino Mega etc
// #elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

#else
#error "Unknown chip, please edit SimpleClockGenerator library with timer+counter definitions"
#endif

#endif