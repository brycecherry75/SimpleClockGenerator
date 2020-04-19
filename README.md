#SimpleClockGenerator Library#

SimpleClockGenerator is a simple clock generator for OCxx pins on an Arduino board.

Revisions:
v1.0	First release

FEATURES:
Supported boards (and derivatives): Uno (Mega/Leonardo/ATmega8 support planned)
Frequency is rounded to the nearest integer divisor up and down for the closest possible match
Quick resume of clock after being stopped (assuming it has been initialized in the first place)
Ability to stop and resume micros()/millis()/delay() to avoid conflict with these functions and Timer/Counter 0 (delayMicroseconds() is still functional when Timer/Counter 0 is programmed for clock output on OC0x pins)
Frequency on output pin is not changed if frequency is outside its limits ((F_CPU / 2) / (counter max + 1)/ prescaler)

KNOWN LIMITATIONS:
Differenent frequencies on different OCx pins within a pair (e.g. 25 kHz on OC0A and 50 kHz on OC0B) are not supported due to AVR hardware limitations

PIN DETAILS WITH DIVIDER RESOLUTIONS:
Uno (and derivatives): (9/10) (16 bit), (6/5)/(11/3) (8 bit)
Mega (and derivatives): (5/2/3)/(6/7/8)/(11/12)/(46/45/44) (16 bit), (10/9)/(13/4) (8 bit)
Leonardo (and derivatives): (11/3) (8 bit), (13/10) (10 bit), 5/(9/10) (16 bit)