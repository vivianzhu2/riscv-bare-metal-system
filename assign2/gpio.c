/* File: gpio.c
 * ------------
* Create four functions: gpio_set_function, gpio_get_function, gpio_read, gpio_write 
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
// ---------------------------------------------------------
// Checks if the input pin is valid to continue
// initialize the group and index of the input pin
// establish the base address and adding the according index for 
// the appropriate address
// read in the current state for all pins in the cfg reg
// modify the bits of interest by shfiting function to the left in bit_pos
// write back the updated state 
void gpio_set_function(gpio_id_t pin, unsigned int function) {
// configures a pin to a selected function
   /***** TODO: Your code goes here *****/
   if (gpio_id_is_valid(pin)== false){
      return;
   } 
   if (function>= 16){ // check if function is valid, since only 4 bits are used to specify function, the max value is 15
      return;
   }    
   gpio_pin_t target_pin;
   target_pin = get_group_and_index(pin); 
   unsigned int group = target_pin.group;
   unsigned int index = target_pin.pin_index;
   volatile unsigned int *base_reg = get_cfg0_reg(group);
   volatile unsigned int *targetreg = base_reg + index / 8;
   unsigned int targetval = *targetreg;
   int bit_pos = (index % 8) * 4;
   unsigned int shiftedfunction = (function << bit_pos);
   unsigned int shiftedpattern = ~ (0xf << bit_pos);
   unsigned int results =( shiftedpattern & targetval) | shiftedfunction;
   *targetreg = results; 
   return;
}
// Function that configures a pin to a selected function. 
// ---------------------------------------------------------
// Checks if the input pin is valid to continue
// initialize the group and index of the input pin
// establish the base address and adding the according index for 
// the appropriate address
// read in the current state for all pins in the cfg reg
// modify the bits of interest by shfiting function to the left in bit_pos
// write back the updated state 
unsigned int gpio_get_function(gpio_id_t pin) {
   if (gpio_id_is_valid(pin)== false){
      return -1;
   } 
   gpio_pin_t target_pin;
   target_pin = get_group_and_index(pin);
   unsigned int group = target_pin.group;
   unsigned int index = target_pin.pin_index;
   volatile unsigned int *reg = get_cfg0_reg(group) + index / 8;
   int bit_pos = (index % 8) * 4;
   unsigned int target_val = *reg;
   unsigned int shiftedpattern = (0xf << bit_pos);
   unsigned int results = shiftedpattern & target_val;
   return (results>>bit_pos);
}

   // unsigned int target_val = *reg;
   // unsigned int shiftedpattern = (0xf << bit_pos);
   // unsigned int results = shiftedpattern & target_val;
// Function that configures a pin to a selected function. 
// ---------------------------------------------------------
// Checks if the input pin is valid to continue
// initialize the group and index of the input pin
// establish the base address and adding the according index for 
// the appropriate address
// read in the current state for all pins in the cfg reg
// modify the bits of interest by shfiting function to the left in bit_pos
// write back the updated state 
void gpio_write(gpio_id_t pin, int value) {
   if (gpio_id_is_valid(pin)== false){
      return;
   }
   gpio_pin_t target_pin;
   target_pin = get_group_and_index(pin);
   unsigned int group = target_pin.group;
   unsigned int index = target_pin.pin_index;
   volatile unsigned int *base_reg = get_data_reg(group);
   unsigned int targetval = *base_reg;
   int bit_pos = index;
   unsigned int shiftedfunction = (value << bit_pos);
   unsigned int shiftedpattern = ~ (0x1 << bit_pos);
   unsigned int results = (shiftedpattern & targetval) | shiftedfunction;
   *base_reg = results; 
   return;

}
// Function that configures a pin to a selected function. 
// ---------------------------------------------------------
// Checks if the input pin is valid to continue
// initialize the group and index of the input pin
// establish the base address and adding the according index for 
// the appropriate address
// read in the current state for all pins in the cfg reg
// modify the bits of interest by shfiting function to the left in bit_pos
// write back the updated state 
int gpio_read(gpio_id_t pin) {
   if (gpio_id_is_valid(pin)== false){
      return -1; 
   }   
   gpio_pin_t target_pin;
   target_pin = get_group_and_index(pin);
   unsigned int group = target_pin.group;
   unsigned int index = target_pin.pin_index;
   volatile unsigned int *reg = get_data_reg(group); 
   int bit_pos = index;
   unsigned int targetval = *reg;
   unsigned int shiftedpattern = (0x1 << bit_pos);
   unsigned int results = shiftedpattern & targetval;
   return (results>>bit_pos);
}

