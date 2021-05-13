/*
MIT License

Copyright (c) 2020 Matthew Ivey

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/* COMP2215: LaFortuna project */
/* Project can generate various factal sets */
/* Probably the worst project I could do for the hardware but it thought it was cool */
/* Also can calculate values of Ackermann but that crashed my LaFortuna so thats now commented out */
/* Author: Matt Ivey         */


#include <avr/io.h>
#include "lcd/lcd.h"
#include "fractals.h"
#include "functions.h"

#include <avr/interrupt.h>
#include <util/delay.h>

// Button press defns
#define SWC _BV(PE7)
#define SWS _BV(PC4)
#define SWN _BV(PC2)

#define CENTER_PRESSED   ((PINE & SWC) == 0)
#define CENTER_UNPRESSED ((PINE & SWC) == SWC)

#define SOUTH_PRESSED    ((PINC & SWS) == 0)
#define SOUTH_UNPRESSED  ((PINC & SWS) == SWS)

#define NORTH_PRESSED    ((PINC & SWN) == 0)
#define NORTH_UNPRESSED  ((PINC & SWN) == SWN)

// Current button press: -1 No press, 0 Center, 1 North, 2 South.
int pressed = -1;

void init(void);

void mainMenu(void);


void main(void) {
    init();
    sei();
    mainMenu();
}

// simple Main menu, while loop sith a switch stmt to handle button presses
void mainMenu(void) {
    display_string_xy("Welcome to La Fractuna: Fractal viewer!",55,20);
    display_string_xy("Choose how to spend your cpu cycles:",10,60);
    display_string_xy("Generating fractals",30,80);
    display_string_xy("Calculate Ackermann values",30,100);
    display_string_xy("^This crashes the LaFortuna: Uncomment line 70 to enable", 40,115);

    // Red indicators for options
    rectangle fracs = {17, 20, 82, 85};
    rectangle funcs = {17, 20, 102, 105};
    fill_rectangle(fracs, RED);

    int pos = 1;
    while(1) {
        _delay_ms(100);
        switch (pressed)
        {
        case 0:
            pressed = -1;
            if (pos == 1) {
                fracsMenu();
            } else {
                //funcsMenu();
            }
            break;
        case 1:
                pos = 1;
                fill_rectangle(fracs, RED);
                fill_rectangle(funcs, BLACK);
            break;
        case 2:
                pos = 0;
                fill_rectangle(funcs, RED);
                fill_rectangle(fracs, BLACK);
        default:
            break;
        }
        pressed = -1;
    }
}

void fracsMenu(void) {
    pressed = -1;
    int pos = 1;
    clear_screen();
    display_string_xy("Welcome to La Fractuna: Fractal viewer!",55,20);
    display_string_xy("Choose your fractal:",10,60);
    display_string_xy("z' = z^2 + c",30,80);
    display_string_xy("z' = z/(e^z + c)", 30, 100);
    display_string_xy("z' = 1/(z + c)^3", 30, 120);
    display_string_xy("z' = e^z + c", 30, 140);

    rectangle brot = {17, 20, 82, 85};
    rectangle rece = {17, 20, 102, 105};
    rectangle recc = {17, 20, 122, 125};
    rectangle expz = {17, 20, 142, 145};

    fill_rectangle(brot, RED);
    while(1) {
        _delay_ms(100);
        switch (pressed)
        {
        case 0:
            speed(pos - 1);
            break;
        case 1:
                pos--;
                fill_rectangle(brot, BLACK);
                fill_rectangle(rece, BLACK);
                fill_rectangle(recc, BLACK);
                fill_rectangle(expz, BLACK);
                
                if (pos == 4) {
                    fill_rectangle(expz, RED);
                } else if (pos == 2) {
                    fill_rectangle(rece, RED);
                } else if (pos == 3) {
                    fill_rectangle(recc, RED);
                } else {
                    pos = 1;
                    fill_rectangle(brot, RED);
                }
            break;
        case 2:
                pos++;
                fill_rectangle(brot, BLACK);
                fill_rectangle(rece, BLACK);
                fill_rectangle(recc, BLACK);
                fill_rectangle(expz, BLACK);

                if (pos == 1) {
                    fill_rectangle(brot, RED);
                } else if (pos == 2) {
                    fill_rectangle(rece, RED);
                } else if (pos == 3) {
                    fill_rectangle(recc, RED);
                } else {
                    pos = 4;
                    fill_rectangle(expz, RED);
                }
        default:
            break;
        }
        pressed = -1;
    }
}

// Menu for fractal resolution
void speed(int frac) {
    clear_screen();
    display_string_xy("Welcome to La Fractuna: Fractal viewer!",55,20);
    display_string_xy("Choose your resolution (High res is slower):",10,60);
    display_string_xy("High",30,80);
    display_string_xy("Low",30,100);

    rectangle high = {17, 20, 82, 85};
    rectangle low = {17, 20, 102, 105};
    fill_rectangle(high, RED);
    int pos = 0;
    while(1) {
        _delay_ms(100);
        switch (pressed)
        {
        case 0:
            pressed = -1;
            dispFrac(frac, pos);
            break;
        case 1:
                pos = 0;
                fill_rectangle(high, RED);
                fill_rectangle(low, BLACK);
            break;
        case 2:
                pos = 1;
                fill_rectangle(low, RED);
                fill_rectangle(high, BLACK);
        default:
            break;
        }
        pressed = -1;
    }
}

void funcsMenu(void) {
    display_ack();
}

ISR( TIMER0_COMPA_vect )  // This is probably not the best way to do this...
{
    cli();
    if (pressed < 0) {
        if (NORTH_PRESSED)
        {
            pressed = 1;
            sei();
            return;
        }
        if (SOUTH_PRESSED )
        {
            pressed = 2;
            sei();
            return;
        }
        if (CENTER_PRESSED) {
        pressed = 0;
        sei();
        return;
        }
    } else {
        if ((NORTH_UNPRESSED && pressed == 1)||(SOUTH_UNPRESSED && pressed ==  2)||(CENTER_UNPRESSED && pressed == 0)){
            pressed = -1;
            sei();
            return;
        }
    }
}



void init(void) {
    /* 8MHz clock, no prescaling (DS, p. 48) */
    CLKPR = (1 << CLKPCE);
    CLKPR = 0;

    init_lcd();

    PORTC |=  SWS | SWN;
    PORTE |= SWC;

    TCCR0A = _BV(WGM01); 
    TCCR0B = _BV(CS01) | _BV(CS00); 
    OCR0A = (uint8_t)(F_CPU / (64.0 * 1000) - 1); 
    TIMSK0 |= _BV(OCIE0A); 
}
