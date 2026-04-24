/* File: clock.c
 * -------------
 * turn_on_segments -- a function that turns on the segments for a given digit on Digit 1
 * display_refresh_loop -- a function that refreshes the display with the current time for a short delay
 * do_countdown -- a function that performs the countdown by calculating the digits for the current time and refreshing the display until the countdown reaches 0
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
unsigned char nums[10] = {0b00111111,0b00000110, 0b01011011, 0b01001111,0b01100110,0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111};

int gpio_id[7] = {GPIO_PD17, GPIO_PB6, GPIO_PB12, GPIO_PB11, GPIO_PB10, GPIO_PD11, GPIO_PD13};
int gpio_digits[4] = {GPIO_PB4, GPIO_PB3, GPIO_PB2, GPIO_PC0};

static void turn_on_segments(int number, int digit){
     gpio_set_output(gpio_digits[digit]); // set the digit pin to output
     for(int i = 0; i < 4; i ++){
         gpio_set_output(gpio_digits[i]); // set all digit pins to output to ensure they are in the correct mode for the display
      }
     for( int i = 0; i < 7; i++){
         gpio_set_output(gpio_id[i]); // set all segment pins to output to ensure they are in the correct mode for the display
     }
    gpio_write(gpio_digits[digit], 1); // turn on the digit by writing 1 to the corresponding digit pin
    for(int i = 0; i < 7; i++){
        int temp = nums[number] >> i;
        int bit_val = temp & 1;
        gpio_write(gpio_id[i] ,bit_val); // turn on the segments for the digit by writing the corresponding bit value to each segment pin
     }

}

void display_refresh_loop(int *values, int delay){
     for(int i=0; i < 1000/4/delay; i++){
       for (int j = 0; j < 4; j++) {
          gpio_write(gpio_digits[j], 1); // turn on the digit
          turn_on_segments(values[j], j); // turn on the segments for the digit with the corresponding value
          timer_delay_ms(delay); // delay for a short period of time to allow the digit to be visible
          gpio_write(gpio_digits[j], 0); // turn off the digit before moving on to the next one 
        }
     }
}

static void do_countdown(int num) {
    while (num > 0){
          int digit1 = num/600;
          int digit2 = (num / 60) % 10;
          int digit3 = (num % 60) / 10;
          int digit4 = (num % 60) % 10;
          int time[4] = {digit1, digit2, digit3, digit4}; // calculate the digits for the current time and store them in an array
          display_refresh_loop(time,2); // refresh the display with the current time for a short delay
          num = num - 1;    
    }
}

void main(void) {

    gpio_id_t button = GPIO_PD12; 
    gpio_set_input(button);
    while (gpio_read(button) == 0); // wait for the button to be pressed
    do_countdown(DURATION); // start the countdown with the specified duration
    int finish[4] = {0,0,0,0};
    while(gpio_read(button) == 1){ // wait for the button to be released
       display_refresh_loop(finish,2); // keep displaying 00:00 until the button is released
       timer_delay_ms(100);
    }   
}
