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


// This was a nice layout for ackermann values, unfortunately it wont be seen
void display_ack(void) {
    clear_screen();
    display_string_xy("\\", 15, 15);
    display_string_xy("\\", 20, 20);
    display_string_xy("\\", 25, 25);
    display_string_xy("n",35,15);
    display_string_xy("m",15,35);

    for (int i = 0; i < 7; i ++) {
        char str[1];
        sprintf(str, "%d", i);
        
        display_string_xy(str,35 + 32 * (i+1),15);
        display_string_xy(str,15,35 + 32 * (i+1));
    }
    for (int m = 0; m < 8; m++) {
        for (int n =0; n < 8; n++) {
            char str[32];
            sprintf(str, "%i", ack(m, n));
            display_string_xy(str, 35 + 32*(n+1), 35 + 32*(m+1));
        }
    }
}

int ack(int m, int n) { // Dont run this!
    char str[8];
    sprintf(str, "m:%d, n:%d", m, n);
    display_string_xy(str, 24, 15);
    if (m == 0) return n + 1;
    if (n == 0) return ack(m-1, 1);
    return ack(m - 1, ack(m, n - 1));
}
