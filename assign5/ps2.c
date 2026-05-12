/* File: ps2_assign5.c
 * -------------------
 * ***** TODO: add your file header comment here *****
 */
#include "gpio.h"
#include "gpio_extra.h"
#include "malloc.h"
#include "ps2.h"
#include "printf.h"
#include "timer.h"

// A ps2_device is a structure that stores all of the state and information
// needed for a PS2 device. The clock field stores the gpio id for the
// clock pin, and the data field stores the gpio id for the data pin.
// Read ps2_new for example code that sets and uses these fields.
//
// You may extend the ps2_device structure with additional fields as needed.
// A pointer to the current ps2_device is passed into all ps2_ calls.
// Storing state in this structure is preferable to using global variables:
// it allows your driver to support multiple PS2 devices accessed concurrently
// (e.g., a keyboard and a mouse).
//
// This definition fills out the structure declared in ps2.h.
struct ps2_device {
    gpio_id_t clock;
    gpio_id_t data;
};

// Creates a new PS2 device connected to given clock and data pins,
// The gpios are configured as input and set to use internal pull-up
// (PS/2 protocol requires clock/data to be high default)
ps2_device_t *ps2_new(gpio_id_t clock_gpio, gpio_id_t data_gpio) {
    // consider why must malloc be used to allocate device
    ps2_device_t *dev = malloc(sizeof(*dev));

    dev->clock = clock_gpio;
    gpio_set_input(dev->clock);
    gpio_set_pullup(dev->clock);

    dev->data = data_gpio;
    gpio_set_input(dev->data);
    gpio_set_pullup(dev->data);
    return dev;
}

// Read a single PS2 scancode. Always returns a correctly received scancode:
// if an error occurs (e.g., start bit not detected, parity is wrong), the
// function should read another scancode.
// uint8_t ps2_read(ps2_device_t *dev) {
//     /***** TODO: Your code goes here *****/

//     // Writing a separate helper function read_bit() is highly
//     // recommended: this function waits for a clock falling
//     // edge then reads the data pin. (Use code you wrote in lab!)
//     return 0xFF;
// }


uint8_t read_bit(ps2_device_t *dev){
    // check if the input is high enough
    gpio_id_t clock = dev->clock;
    gpio_id_t data = dev->data;

    while(gpio_read(clock) == 0){
    }
    while(gpio_read(clock) == 1){
    }
    return  gpio_read(data);
}
// Read a single PS2 scan code.
uint8_t ps2_read(ps2_device_t *dev) {
    /**** TODO: your code goes here *****/
    // gpio_id_t clock = dev->clock;
    // gpio_id_t data = dev->data;
    int ones = 0;
    // int valid = 0;
   
   
    uint8_t code = 0; 
    // unsigned long restart = timer_get_ticks();
    unsigned long time = timer_get_ticks();
    unsigned long lasttime = timer_get_ticks();
    while(1){
        int start = read_bit(dev);
        if(start != 0){
            continue ;
        }
        lasttime = time;
        time =  timer_get_ticks();
        // if(restart - time > 500 * 24){
        //     // printf("restart: %ld, time: %ld", restart, time);
        //     continue;
        // }
        // timer_delay_us(30);

        for(int i = 0; i < 8; i++){
            int bit = read_bit(dev);
            lasttime = time;
            time = timer_get_ticks(); 
            if(time - lasttime > 500 * 24){
                // printf("restart: %ld, time: %ld", restart, time);
                continue;
            }
            code |= (bit << i); 
            if(bit == 1){
                ones += 1;
            }
        }
        int parity = read_bit(dev);
        lasttime = time;
        time = timer_get_ticks(); 
        if(time - lasttime > 500 * 24){
            // printf("restart: %ld, time: %ld", restart, time);
            continue;
        }
        // printf("ones: %d", ones);
        if((ones + parity) % 2 == 0){
            ones = 0;
            continue;  // bad parity, try again
        }
        else{
            ones = 0;
        }

        // // 4. stop bit must be 1
        int stop = read_bit(dev);
        lasttime = time;
        time = timer_get_ticks(); 
        if(time - lasttime > 500 * 24){
            // printf("restart: %ld, time: %ld", restart, time);
            continue;
        }
        if(stop != 1){
            continue;
        }   
        // if(ones%2 == 1){
        //     int temp = (read_bit(dev) << 1) & 1;
        //     if(temp == 0){
        //         code |= (read_bit(dev) << 1);
        //         valid  = 1;
            
        //     }
        //     else{
        //         valid = 0;
        //         code = read_bit(dev);
        //         time = timer_get_ticks();
        //         continue;
        //     }
        // }

        // if(valid == 1){
        //     int temp = (read_bit(dev) << 1) & 1;
        //     if(temp == 1){
        //         return code; 
                
                
        //     }
        //     else{
        //         code = read_bit(dev);
        //         continue;
                
        //     }
            return code;
        }
        // printf("start: %x", start);
        // printf("code : %x", code);
        // printf("parity: %x", parity);
        

    }
    
// }

