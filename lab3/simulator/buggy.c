/* File: buggy.c
 * -------------
 * This buggy program uses a local variable that is not properly
 * initialized. The contents are undefined/unpredictable.
 * Run this program on the Pi, and then again in gdb under
 * simulation.  Are the garbage contents the same?  Do you get
 * the same results on subsequent runs? 
 */
#include "printf.h"
#include "uart.h"

static void buggy(const char *msg) {
    char buf[260];         // buf contents not initialized
    printf("%s:\n\tbefore init, buf = %s (chars: %x %x %x)\n", msg, buf, buf[0], buf[1], buf[2]);

    buf[0] = 'H'; // put chars Hey into buf
    buf[1] = 'e';
    buf[2] = 'y';
    buf[3] = '\0'; // end with null term
    printf("\tafter init,  buf = %s (chars: %x %x %x)\n", buf, buf[0], buf[1], buf[2]);
}

void main(void) {
    uart_init();
    buggy("First call to buggy");
    buggy("Second call to buggy");
}
