/* File: clock.c
 * -------------
 * ***** TODO: add your file header comment here *****
 */
#include "gpio.h"
#include "timer.h"

/* WARNING: Do not make any edits to the 3 lines below that start with #
 * These preprocessor directives cooperate with Makefile to allow
 * setting DURATION as part of the build process. You can test the
 * clock application on duration of your choice by specifying value
 * when invoking make:
 *       make run DURATION=30
 * DURATION is expressed as number of seconds, value will default to
 * 67 seconds if not explicitly set when invoking make
 */
#ifndef DURATION
#define DURATION 67
#endif


static void do_countdown(int num) {
    /***** TODO: Your code goes here *****/
}

void main(void) {
    do_countdown(DURATION);
}
