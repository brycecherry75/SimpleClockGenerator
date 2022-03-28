#SimpleClockGenerator Library#

SimpleClockGenerator is a simple clock generator for OCxx pins on an Arduino board.

Revisions:
v1.0.0	First release
v1.1.0	Change of function wording of SimpleCLK.xxx to SimpleClockGenerator.xxx to correspond with library name
v1.1.1	Eliminated requirement for backup registers affecting millis()/micros()/delay() functions
v1.1.2	Demo sketch is now fully interactive
v1.2.0	Added external source frequency divider; FrequencyDividerTest is now incorporated in the main demo sketch
v1.2.1	Added function to return available prescalers on a given pin

FEATURES:
Supported boards (and derivatives): Uno (Mega/Leonardo/ATmega8 support planned)
Frequency is rounded to the nearest integer divisor up and down for the closest possible match
Quick resume of clock after being stopped (assuming it has been initialized in the first place)
Ability to stop and resume micros()/millis()/delay() to avoid conflict with these functions and Timer/Counter 0 (delayMicroseconds() is still functional when Timer/Counter 0 is programmed for clock output on OC0x pins)
Frequency on output pin is not changed if frequency is outside its limits ((F_CPU / 2) / (counter max + 1) / prescaler)
Can divide frequency at a Tx pin connected to a timer/counter and output it on a pin which is connected to the same timer/counter where frequency output = (frequency input / 2) / (divider value + 1)

KNOWN LIMITATIONS:
Differenent frequencies on different OCx pins within a set (e.g. 25 kHz on OC0A and 50 kHz on OC0B) are not supported due to AVR hardware limitations

USAGE:
init(pin): Initialize clock output for a given pin
initDividerAtTpin(pin): Initialize clock for a given pin using the counter/timer Tx clock input pin
writeDivider(pin, value): Direct write to divider - especially useful if initialized by initDivider()
readDivider(pin): Returns a uint16_t value from the timer divider connected to this pin
incrementDivider(pin, value): Increments divider by a given value on a given pin (will not overflow)
decrementDivider(pin, value): Decrements divider by a given value on a given pin (will not underflow)
readPrescaler(pin);  Returns a uint16_t value from the timer prescaler connected to this pin
start(pin, frequency): Start clock output at a given frequency for a certain pin - return value is actual frequency rounded to its closest integer
resume(pin): Resume clock output on a certain pin
stop(pin): Stop clock output on a certain pin
RestartMillisMicros(): Disables clock output on pins used by the timer (usually Timer/Counter 0 and its corresponding OCxx pins) and restarts millis()/micros()/delay()
ReturnAvailablePrescalers(pin, *AvaiablePrescalers): Returns available internal prescalers to *AvailablePrescalers (word) based on a given pin; a 0 indicates the end of the number of prescalers and the array passed to *AvailablePrescalers is AvailablePrescalersPerTimer

PIN DETAILS WITH DIVIDER RESOLUTIONS (First pin with Tx can be used for Tx input frequency division - T pin is not used by millis/micros/delay):
Uno (and derivatives): (9/10/(T1: 5) - used by millis/micros/delay) (16 bit), (6/5/(T0: 4))/(11/3) (8 bit)
Mega (and derivatives): (5/2/3/T3)/(6/7/8/T4)/(11/12/TOSC1)/(46/45/44/T5: 47) (16 bit), (10/9/T1)/(13/4/T0: 38 - used by millis/micros/delay) (8 bit) - T1/T2/T3/T4/TOSC1 have no traces connected to its headers on the Mega board (official and most of the compatibles)
Leonardo (and derivatives): (11/3/T0: 6 - used by millis/micros/delay) (8 bit), (13/10/T1: 12) (10 bit), 5/(9/10) (16 bit)