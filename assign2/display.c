/* File: display.c
 * ------------
* design and implement re-usable module for 7-segment display
* encapsulates inner workings of refresh, how digits mapt to segments, etc
* make into tidy package that offers clean interface for client to config display and refresh 4-digit value
*/
#include "display.h"
// #include <stddef.h>
#include "gpio.h"
#include "timer.h"

unsigned int refresh_delay = 2; // default refresh delay for the display in milliseconds
unsigned int display_duration = 1; // default display duration for the display in seconds


unsigned char numsd[10] = {0b00111111,0b00000110, 0b01011011, 0b01001111,0b01100110,0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111};

int gpio_idd[7] = {GPIO_PD17, GPIO_PB6, GPIO_PB12, GPIO_PB11, GPIO_PB10, GPIO_PD11, GPIO_PD13};
int gpio_digitsd[4] = {GPIO_PB4, GPIO_PB3, GPIO_PB2, GPIO_PC0};

static void turn_on_segments(unsigned int number, int digit){  // private helper function to turn on the segments for a given digit on the display
     gpio_set_output(gpio_digitsd[digit]); // set the digit pin to output
     for(int i = 0; i < 4; i ++){
         gpio_set_output(gpio_digitsd[i]); // set all digit pins to output to ensure they are in the correct mode for the display
      }
     for( int i = 0; i < 7; i++){
         gpio_set_output(gpio_idd[i]); // set all segment pins to output to ensure they are in the correct mode for the display
     }
    gpio_write(gpio_digitsd[digit], 1); // turn on the digit by writing 1 to the corresponding digit pin
    for(int i = 0; i < 7; i++){
        int temp = numsd[number] >> i;
        int bit_val = temp & 1;
        gpio_write(gpio_idd[i] ,bit_val); // turn on the segments for the digit by writing the corresponding bit value to each segment pin
     }

}

static void display_refresh_loop(unsigned int *values, int delay){ // private helper function to refresh the display with the current time for a short delay
     for(int i=0; i < display_duration*1000/4/delay; i++){
       for (int j = 0; j < 4; j++) {
          gpio_write(gpio_digitsd[j], 1); // turn on the digit
          turn_on_segments(values[j], j); // turn on the segments for the digit with the corresponding value
          timer_delay_ms(delay); // delay for a short period of time to allow the digit to be visible
          gpio_write(gpio_digitsd[j], 0); // turn off the digit before moving on to the next one 
        }
     }
}

void display_init(){}

void display(unsigned int num) { //public function to refresh 4-digit value on the display by setting the appropriate segments to high or low based on the digit values

          unsigned int digit1 = num/1000;
          unsigned int digit2 = (num%1000)/100;
          unsigned int digit3 = (num%100)/10;
          unsigned int digit4 = num%10;
          unsigned int digits[4] = {digit1, digit2, digit3, digit4}; // calculate the digits for the current time and store them in an array
          display_refresh_loop(digits,refresh_delay); // refresh the display with the current time for a short delay
              
}

void configure_display(unsigned int rdelay, unsigned int dur) { //   configure the display by setting the appropriate refresh delay and display duration for the display, and ensuring that all necessary pins are set to output mode

   set_refresh_delay(rdelay); // set the refresh delay for the display
   set_display_duration(dur); // set the display duration for the display

}

void set_refresh_delay(unsigned int rdelay) { // set the refresh delay for the display

   refresh_delay=rdelay; // update the global variable for the refresh delay

}

unsigned int get_refresh_delay() { // get the refresh delay for the display

   return refresh_delay; //   return the current refresh delay for the display

}

void set_display_duration(unsigned int dur) { // set the display duration for the display

   display_duration=dur;  //  update the global variable for the display duration

}

unsigned int get_display_duration() { //  get the display duration for the display

   return display_duration; // return the current display duration for the display  

}
