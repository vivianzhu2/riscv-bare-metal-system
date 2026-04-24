/* File: extension.c
 * -----------------
* design and implement re-usable module for 7-segment display
* encapsulates inner workings of refresh, how digits mapt to segments, etc
* make into tidy package that offers clean interface for client to config display and refresh 4-digit value
* clock client app becomes simple loop that asks 7-segment to refresh current value
  */

#include "display.h"


/* WARNING: Do not make any edits to the 3 lines below that start with #
 * These preprocessor directives cooperate with Makefile to allow
 * setting DISPLAY_VALUE as part of the build process. You can test the
 * extension application on duration of your choice by specifying value
 * when invoking make:
 *       make run DISPLAY_VALUE 2026
 * DISPLAY_VALUE  is expressed as a four digit number, value will default to
 * 2026 if not explicitly set when invoking make
 */
#ifndef DISPLAY_VALUE
#define DISPLAY_VALUE 2026
#endif

void main(void) {

    display_init(); // initialize the display module
    
    // config display and refresh 4-digit value
    set_display_duration(1); // set the display duration for the display to 1 second
    set_refresh_delay(2); // set the refresh delay for the display to a short delay to allow the digits to be visible 
    
    if (get_display_duration() == 0 || get_refresh_delay() == 0) { // check if the display duration or refresh delay is set to 0, if so, do not refresh the display and return
        return;
    }   

    // clock client app becomes simple loop that asks 7-segment to refresh current value
    while (1) {        
        display(DISPLAY_VALUE); // ask 7-segment to refresh current value
    }  
}

