/* File: gpio.c
 * ------------
* Create four functions: gpio_set_function, gpio_get_function, 
* gpio_read, gpio_write 
*/
#include "gpio.h"
#include <stddef.h>

enum { GROUP_B = 0, GROUP_C, GROUP_D, GROUP_E, GROUP_F, GROUP_G };
// This struct is for the gpio_pin_t where there is a change
typedef struct  {
    unsigned int group;
    unsigned int pin_index;
} gpio_pin_t;

// The gpio_id_t enumeration assigns a symbolic constant for each
// in such a way to use a single hex constant. The more significant
// hex digit identifies the group and lower 2 hex digits are pin index:
//       constant 0xNnn  N = which group,  nn = pin index within group
//
// This helper function extracts the group and pin index from a gpio_id_t
// e.g. GPIO_PB4 belongs to GROUP_B and has pin_index 4
static gpio_pin_t get_group_and_index(gpio_id_t gpio) {
    gpio_pin_t gp;
    gp.group = gpio >> 8;
    gp.pin_index = gpio & 0xff; // lower 2 hex digits
    return gp;
}

// The gpio groups are differently sized, e.g. B has 13 pins, C only 8.
// This helper function confirms that a gpio_id_t is valid (group
// and pin index are valid)
bool gpio_id_is_valid(gpio_id_t pin) {
    gpio_pin_t gp = get_group_and_index(pin);
    switch (gp.group) {
        case GROUP_B: return (gp.pin_index <= GPIO_PB_LAST_INDEX);
        case GROUP_C: return (gp.pin_index <= GPIO_PC_LAST_INDEX);
        case GROUP_D: return (gp.pin_index <= GPIO_PD_LAST_INDEX);
        case GROUP_E: return (gp.pin_index <= GPIO_PE_LAST_INDEX);
        case GROUP_F: return (gp.pin_index <= GPIO_PF_LAST_INDEX);
        case GROUP_G: return (gp.pin_index <= GPIO_PG_LAST_INDEX);
        default:      return false;
    }
}

// Helper function suggestion that returns address of config0
// register for a gpio group, i.e. get_cfg0_reg(GROUP_B)
// Refer to D1-H user manual to identify address of config0 register
// for each group. It can be helpful to note how address of config1 and
// config2 registers can be computed as relative offset from config0.
// (can discard this function if it doesn't fit with your design)
static volatile unsigned int *get_cfg0_reg(unsigned int group) {
    switch (group) {
        case GROUP_B: return (unsigned int*)0x02000030;
        case GROUP_C: return (unsigned int*)0x02000060;
        case GROUP_D: return (unsigned int*)0x02000090;
        case GROUP_E: return (unsigned int*)0x020000c0;
        case GROUP_F: return (unsigned int*)0x020000f0;
        case GROUP_G: return (unsigned int*)0x02000120;
        default:      return (unsigned int*)0x00000000;
     }
}
// Similar to above, get address of data register for a gpio group
// (can discard this function if it doesn't fit with your design)
static volatile unsigned int *get_data_reg(unsigned int group) {
   switch (group) {
        case GROUP_B: return (unsigned int*)0x02000040;
        case GROUP_C: return (unsigned int*)0x02000070;
        case GROUP_D: return (unsigned int*)0x020000a0;
        case GROUP_E: return (unsigned int*)0x020000d0;
        case GROUP_F: return (unsigned int*)0x02000100;
        case GROUP_G: return (unsigned int*)0x02000130;
        default:      return (unsigned int*)0x00000000;
   }
}

void gpio_init(void) {
    // no initialization required for this peripheral
}

void gpio_set_input(gpio_id_t pin) {
    gpio_set_function(pin, GPIO_FN_INPUT);
}

void gpio_set_output(gpio_id_t pin) {
    gpio_set_function(pin, GPIO_FN_OUTPUT);
}
// Function that configures a pin to a selected function.
void gpio_set_function(gpio_id_t pin, unsigned int function) {


   if (gpio_id_is_valid(pin)== false){  // if the pin is invalid, do not continue and return
      return;
   } 
   gpio_pin_t target_pin;
   target_pin = get_group_and_index(pin); // get the group and index of the pin
   unsigned int group = target_pin.group;
   unsigned int index = target_pin.pin_index;
   volatile unsigned int *base_reg = get_cfg0_reg(group); // get the base address of the cfg reg for the group
   volatile unsigned int *targetreg = base_reg + index / 8; // get the address of the cfg0/1/2 reg 
   unsigned int targetval = *targetreg;  // read in the current state of the cfg reg
   int bit_pos = (index % 8) * 4;  // calculate the bit position of the function bits 
   unsigned int shiftedfunction = (function << bit_pos); // shift the function to the left to align with the bit position
   unsigned int shiftedpattern = ~ (0xf << bit_pos); // create a pattern to clear the function bits for the pin 
   unsigned int results =( shiftedpattern & targetval) | shiftedfunction; // clear the function bits for the pin and set the new function 
   *targetreg = results; // write back the updated state to the cfg reg
   return;
}

// Function that gets the current function of a pin.
// ---------------------------------------------------------
unsigned int gpio_get_function(gpio_id_t pin) {
   if (gpio_id_is_valid(pin)== false){  // if the pin is invalid, do not continue and return
      return -1;
   } 
   gpio_pin_t target_pin;
   target_pin = get_group_and_index(pin); // get the group and index of the pin
   unsigned int group = target_pin.group;
   unsigned int index = target_pin.pin_index;
   volatile unsigned int *reg = get_cfg0_reg(group) + index / 8; // get the address of the cfg0/1/2 reg
   int bit_pos = (index % 8) * 4; // calculate the bit position of the function bits for the pin
   unsigned int target_val = *reg; // read in the current state of the cfg reg
   unsigned int shiftedpattern = (0xf << bit_pos); // create a pattern to isolate the function bits 
   unsigned int results = shiftedpattern & target_val; // isolate the function bits for the pin 
   return (results>>bit_pos); // shift the results to the right to get the function value and return it
}
// Function that writes a value to a pin configured as output.
// ---------------------------------------------------------
void gpio_write(gpio_id_t pin, int value) {
   if (gpio_id_is_valid(pin)== false){ // if the pin is invalid, do not continue and return
      return;
   }
   gpio_pin_t target_pin;
   target_pin = get_group_and_index(pin); // get the group and index of the pin
   unsigned int group = target_pin.group; 
   unsigned int index = target_pin.pin_index;
   volatile unsigned int *base_reg = get_data_reg(group);  //  get the base address of the data reg for the group
   unsigned int targetval = *base_reg;  // read in the current state of the data reg for the group
   int bit_pos = index;
   unsigned int shiftedfunction = (value << bit_pos); // shift the value to the left to align with the bit position of the pin
   unsigned int shiftedpattern = ~ (0x1 << bit_pos);  // create a pattern to clear the bit for the pin
   unsigned int results = (shiftedpattern & targetval) | shiftedfunction; //modify the results
   *base_reg = results; // write back the updated state to the data reg for the group
   return;

}
// Function that reads the value of a pin configured as input.
// ---------------------------------------------------------
int gpio_read(gpio_id_t pin) {
   if (gpio_id_is_valid(pin)== false){ // if the pin is invalid, do not continue and return
      return -1; 
   }   
   gpio_pin_t target_pin;
   target_pin = get_group_and_index(pin); // get the group and index of the pin
   unsigned int group = target_pin.group;
   unsigned int index = target_pin.pin_index;
   volatile unsigned int *reg = get_data_reg(group); // get the address of the data reg for the group
   int bit_pos = index;
   unsigned int targetval = *reg;  // read in the current state of the data reg for the group
   unsigned int shiftedpattern = (0x1 << bit_pos);  // create a pattern to isolate the bit for the pin
   unsigned int results = shiftedpattern & targetval;  // isolate the bit for the pin
   return (results>>bit_pos);  // shift the results to the right to get the bit value and return it
}
