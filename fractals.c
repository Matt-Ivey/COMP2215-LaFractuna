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

#include "lcd/lcd.h"
#include "math.h"


typedef struct HsvColour { 
    unsigned char h;
    unsigned char s;
    unsigned char v;
} HsvColour;

// HSV to hex, modified from 
// https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
uint16_t hsvConvert(HsvColour hsv) {
    uint16_t col;

    unsigned char region, remainder, p, q, t, r, g, b;

    if (hsv.s == 0)
    {
        col = ((hsv.v<<11)|(hsv.v<<5)|(hsv.v));
        return col;
    }

    region = hsv.h / 43;
    remainder = (hsv.h - (region * 43)) * 6; 

    p = (hsv.v * (255 - hsv.s)) >> 8;
    q = (hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8;
    t = (hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8;

    switch (region)
    {
        case 0:
            r = hsv.v; g = t; b = p;
            break;
        case 1:
            r = q; g = hsv.v; b = p;
            break;
        case 2:
            r = p; g = hsv.v; b = t;
            break;
        case 3:
            r = p; g = q; b = hsv.v;
            break;
        case 4:
            r = t; g = p; b = hsv.v;
            break;
        default:
            r = hsv.v; g = p; b = q;
            break;
    }

    col = ((r<<11)|(g<<5)|(b));

    return col;
}

// Complex number
typedef struct Cnum
{
    double re;
    double im;
} Cnum;

// Mandelbrot: Z' = Z^2 + c
Cnum mandel(Cnum z, Cnum init)  {
    Cnum newZ;

    newZ.re = z.re * z.re - z.im * z.im + init.re;
    newZ.im = 2 * z.re * z.im + init.im;

    return newZ;
}

// z' = z/(e^z + c)
Cnum fun1(Cnum z, Cnum init) {
    Cnum newZ;
    double co, si;
    co = exp(z.re) *cos(z.im) + init.re;
    si = exp(z.re) * sin(z.im) + init.im;
    double denom = (co * co + si * si);

    newZ.re = (z.re * co + z.im * si) / denom;
    newZ.im = (z.im * co - z.re * si) / denom;

    return newZ;
}

// Z' = 1/(z + c)^3
Cnum fun2(Cnum z, Cnum init) {
    Cnum newZ;
    double apm, bpn, rel, ima, denom;
    apm = z.re + init.re;
    bpn = z.im + init.im;
    rel = apm * apm * apm - 3 * apm * bpn * bpn;
    ima = bpn * bpn * bpn - 3 * apm * apm * bpn;
    denom = rel * rel + ima * ima;
    if (denom == 0) {
        newZ.re = 4;
        newZ.im = 4;
    } else {
        newZ.re = rel / denom;
        newZ.im = ima / denom;
    }
    return newZ;
}

// Z' = e^z + c
Cnum test(Cnum z, Cnum init) {
    Cnum newZ;
    newZ.re = exp(z.re)*cos(z.im) + init.re;
    newZ.im = exp(z.re)*sin(z.im) + init.im;
    return newZ;
}

// squared dist from 0 for a Cnum
double sqr_rad(Cnum z) {
   return z.re * z.re + z.im * z.im;
}

// Main deriver
void dispFrac(int func, int fast) {
    clear_screen();
    int res;
    if (fast == 1) {
        res = 4;
    } else {
        res = 1;
    }
    int max_its, it_count, hmul;
    double esc_rad_sqr = 4.0;
    double xm, xc, ym, yc;

    switch (func) // switch to give reasonable starting values for fractals for each function
    {
    case 0:
        xc = 2.1;
        yc = 1;
        max_its = 16;
        ym = 2*yc / 240;
        xm = 3.1 / 320;
        break;
    case 1:
        xc = 2.0;
        yc = 1.0;
        if (fast == 1) {
            max_its = 16;
        } else {
            max_its = 32;            
        }
        esc_rad_sqr = 4;
        xm = xc / 320;
        ym = 2*yc / 240;
        break;
    case 2:
        xc = 1.2;
        yc = 1.2;
        if (fast == 1) {
            max_its = 16;
        } else {
            max_its = 32;
        }
        ym = 2*yc /240;
        xm = 2*xc /320;
        break;
    case 3:
        xc = 1.0;
        yc = 2.75;
        xm = 3.21/320;
        ym = 5.51/240;
        max_its = 16;
        esc_rad_sqr = 100;
        break;
    default:
        xc = 2.1;
        yc = 1;
        max_its = 16;
        ym = 2*yc / 240;
        xm = 3.1 / 320;
        break;
    }

    hmul = 256/max_its;
    

    for (int y = 0; y < 240; y+=res) {
        for (int x = 0; x < 320; x+=res) {
            Cnum z, init;

            z.re = x * xm - xc;
            z.im =  yc - y * ym;
            init.re = z.re;
            init.im = z.im;

            for (it_count = 0; ( sqr_rad(z) < esc_rad_sqr) && (it_count < max_its);) { // iterates given function

                switch (func)
                {
                case 0:
                    z = mandel(z, init);
                    if (it_count <= 4) {
                        it_count+=3;
                    } else {
                        it_count++;
                    }
                    break;
                case 1:
                    z = fun1(z, init);
                    it_count++;
                    break;
                case 2:
                    z = fun2(z, init);
                    it_count++;
                    break;
                case 3: 
                    if ((x < 80 && y < 80) || (x < 80 && y > 160)) {
                        it_count = max_its;
                        break;
                    }
                    z = test(z, init);
                    it_count++;
                    break;
                default:
                    z = mandel(z, init);
                    if (it_count <= 4) {
                        it_count+=3;
                    } else {
                        it_count++;
                    }
                    break;
                }
            }

            HsvColour t;
            t.h = it_count * hmul;
            t.s = 255;
            t.v = 255* (it_count < max_its);
            rectangle r = {x, x+res, y, y+res};
            fill_rectangle(r, hsvConvert(t));
        }
    }
}