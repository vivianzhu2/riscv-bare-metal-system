/* File: ps2_assign5.c
 * -------------------
 * The ps2 module encapsulates functionality for interacting with a PS/2 device.
 * The ps2.c module has only two functions: ps2_new and ps2_read.
 * All of the nitty-gritty low-level protocol details are encapsulated within the ps2 module. 
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


// Helper function to read one bit from the PS2 device, checking for valid timing
uint8_t read_bit(ps2_device_t *dev){
    // check if the input is high enough
    gpio_id_t clock = dev->clock;
    gpio_id_t data = dev->data;

    while(gpio_read(clock) == 0){ // wait for rising edge
    }
    while(gpio_read(clock) == 1){ // wait for falling edge
    }
    return  gpio_read(data);
}
// The ps2_read function reads one well-formed scancode.
// "Well-formed" means that the start, parity, and stop bit each have valid values.
// Read a single PS2 scancode. Always returns a correctly received scancode:
// if an error occurs (e.g., start bit not detected, parity is wrong), the
// function should read another scancode.
uint8_t ps2_read(ps2_device_t *dev) {

    int ones = 0;
    // int valid = 0;
   
   
    uint8_t code = 0; 
    // unsigned long restart = timer_get_ticks();
    unsigned long time = timer_get_ticks();
    unsigned long lasttime = timer_get_ticks();
    int resync=0;
    int start=0;
    while(1){
        code = 0; 
        ones = 0;
        if (resync == 0){
            start = read_bit(dev);
        }
        else{
            resync = 0;
        }
        
        //int start = read_bit(dev);
        if(start != 0){
            continue ;
        }
        lasttime = time;
        time =  timer_get_ticks();


//         Your driver should implement a timeout reset that detects when the gap between bits is too long for them to have been part of the same scancode. Given the PS/2 bit rate of 10-16Khz, the expected gap between bits should be in the range 60-100us. Call your timer_get_ticks function to get a timestamp for each clock edge. If you detect that this clock edge occurred more than 500us after the previous timestamp, reset the scancode state and assume the current clock edge is for the start bit of a new scancode. This small effort provides additional robustness in your driver to combat flaky connections and hardware blips.
//         After reconnect, your driver should immediately resynchronize (due to long gap) and correctly read the next new scancode.
//         discard malformed (wrong start/stop/parity, timeout) and resynchronize
// resycnhronize can be challenging to directly test, just try your best and we'll run under our test rig to give you results
        // if(restart - time > 500 * 24){
        //     // printf("restart: %ld, time: %ld", restart, time);
        //     continue;
        // }
        // timer_delay_us(30);
        //Call your timer_get_ticks function to get a timestamp for each clock edge. 
        //If you detect that this clock edge occurred more than 500us after the previous timestamp, 
        // reset the scancode state and assume the current clock edge is for the start bit of a new scancode. 
        for(int i = 0; i < 8; i++){ // read 8 data bits, LSB first
            int bit = read_bit(dev);
            lasttime = time;
            time = timer_get_ticks(); 
            if(time - lasttime > 500 * 24){
                // printf("restart: %ld, time: %ld", restart, time);
                    resync = 1;
                    start=bit;
                    break;  

            }
            code |= (bit << i); 
            if(bit == 1){
                ones += 1;
            }
        }
        if (resync == 1){
            continue;
        }
        int parity = read_bit(dev);
        lasttime = time;
        time = timer_get_ticks(); 
        if(time - lasttime > 500 * 24){
            // printf("restart: %ld, time: %ld", restart, time);
            resync = 1;
            start=parity;
            continue;
        }
        // printf("ones: %d", ones);
        if((ones + parity) % 2 == 0){
            ones = 0;
            resync = 1;
            start=parity;
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
            resync = 1;
            start=stop;
            continue;
        }
        if(stop != 1){
            resync = 1;
            start=stop;
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

