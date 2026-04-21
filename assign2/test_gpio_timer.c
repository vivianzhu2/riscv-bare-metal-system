/* File: test_gpio_timer.c
 * -----------------------
 * ***** TODO: add your file header comment here *****
 */
#include "gpio.h"
#include "timer.h"

// You call assert on an expression that you expect to be true. If expr
// instead evaluates to false, then assert calls abort, which stops
// your program and flashes onboard led.
#define assert(expr) if(!(expr)) abort()

// infinite loop that flashes onboard blue LED (GPIO PD18)
void abort(void) {
    volatile unsigned int *GPIO_CFG2 = (unsigned int *)0x02000098;
    volatile unsigned int *GPIO_DATA = (unsigned int *)0x020000a0;

    // Configure GPIO PD18 function to be output.
    *GPIO_CFG2 = (*GPIO_CFG2 & ~(0xf00)) | 0x100;
    while (1) { // infinite loop
        *GPIO_DATA ^= (1 << 18); // invert value
        for (volatile int delay = 0x100000; delay > 0; delay--) ; // wait
    }
}

static void test_gpio_set_get_function(void) {
    // Test get pin function (pin defaults to disabled)
    assert( gpio_get_function(GPIO_PC0) == GPIO_FN_DISABLED);

    // Set pin to output, confirm get returns what was set
    gpio_set_output(GPIO_PC0);
    assert( gpio_get_function(GPIO_PC0) == GPIO_FN_OUTPUT );

    // Set pin to input, confirm get returns what was set
    gpio_set_input(GPIO_PC0);
    assert( gpio_get_function(GPIO_PC0) == GPIO_FN_INPUT );

    /***** TODO: add more tests *****/
    // test all the pins in the groups by setting them to input and output and confirming the get function returns the correct value for PB 
    for(int i = GPIO_PB0; i < GPIO_PB_LAST_INDEX; i++){
       gpio_set_input(i);
       assert(gpio_get_function(i) == GPIO_FN_INPUT);
       gpio_set_output(i);
       assert(gpio_get_function(i) == GPIO_FN_OUTPUT);
     }
    // extensive test for all pins in PC0-PC7  
    for(int i = GPIO_PC0; i < GPIO_PC_LAST_INDEX; i++){
       gpio_set_input(i);
       assert(gpio_get_function(i) == GPIO_FN_INPUT);
       gpio_set_output(i);
       assert(gpio_get_function(i) == GPIO_FN_OUTPUT);
     } 
    // extensive test for all pins in PD0-PD21 except PD18 
    for(int i = GPIO_PD0; i < GPIO_PD_LAST_INDEX; i++){
       if(i == GPIO_PD18){
           continue;
       }
       gpio_set_input(i);
       assert(gpio_get_function(i) == GPIO_FN_INPUT);
       gpio_set_output(i);
       assert(gpio_get_function(i) == GPIO_FN_OUTPUT);
     }
     //extensive test for all pins in PE0-PE5 
    for(int i = GPIO_PE0; i < GPIO_PE_LAST_INDEX; i++){
       gpio_set_input(i);
       assert(gpio_get_function(i) == GPIO_FN_INPUT);
       gpio_set_output(i);
       assert(gpio_get_function(i) == GPIO_FN_OUTPUT);
     }
     //extensive test for all pins in PF0-PF5
    for(int i = GPIO_PF0; i < GPIO_PF_LAST_INDEX; i++){
       gpio_set_input(i);
       assert(gpio_get_function(i) == GPIO_FN_INPUT);
       gpio_set_output(i);
       assert(gpio_get_function(i) == GPIO_FN_OUTPUT);
     }  

// add one about the reconfiguration property of the pins, i.e. setting a pin to output then input then disabled and confirming the get function returns the correct value for each step
    gpio_set_input(GPIO_PB1);
    gpio_set_function(GPIO_PB1, GPIO_FN_DISABLED);
    assert(gpio_get_function(GPIO_PB1) == GPIO_FN_DISABLED);
//  other valid functions like alternate functions and disabled function
    gpio_set_output(GPIO_PF2);
    gpio_set_input(GPIO_PF2);
    gpio_set_function(GPIO_PF2, GPIO_FN_ALT7);
    assert(gpio_get_function(GPIO_PF2) == GPIO_FN_ALT7);

// independent pin configuration: setting one pin's function should not affect the function of another pin
   gpio_set_function(GPIO_PE5, GPIO_FN_OUTPUT);
   gpio_set_function(GPIO_PE4, GPIO_FN_ALT3);
   assert(gpio_get_function(GPIO_PE5) == GPIO_FN_OUTPUT);  // PE5 survived PE4's write
   assert(gpio_get_function(GPIO_PE4) == GPIO_FN_ALT3); 
// BE CAREFUL FOR SPECIAL PINS PD18
}

static void test_gpio_read_write(void) {
    // Test read write

    // set pin to output before gpio_write
    gpio_set_output(GPIO_PB4);

    // gpio_write low, confirm gpio_read reads what was written
    gpio_write(GPIO_PB4, 0);
    assert( gpio_read(GPIO_PB4) ==  0 );

   // gpio_write high, confirm gpio_read reads what was written
    gpio_write(GPIO_PB4, 1);
    assert( gpio_read(GPIO_PB4) ==  1 );

    // gpio_write low, confirm gpio_read reads what was written
    gpio_write(GPIO_PB4, 0);
    assert( gpio_read(GPIO_PB4) ==  0 );

    // Test that gpio_write does not affect gpio_read for pins configured as input
     for(int i = GPIO_PB0; i < GPIO_PB_LAST_INDEX; i++){
       gpio_set_output(i);
       gpio_write(i, 0);
       assert(gpio_read(i) ==  0);
     }
     for(int i = GPIO_PC0; i < GPIO_PC_LAST_INDEX; i++){
       gpio_set_output(i);
       gpio_write(i, 0);
       assert(gpio_read(i) ==  0);
     }
     for(int i = GPIO_PD0; i < GPIO_PD_LAST_INDEX; i++){
       gpio_set_output(i);
       gpio_write(i, 0);
       assert(gpio_read(i) ==  0);
     }
     for(int i = GPIO_PD0; i < GPIO_PD_LAST_INDEX; i++){
       gpio_set_output(i);
       gpio_write(i, 0);
       assert(gpio_read(i) ==  0);
     }
     for(int i = GPIO_PE0; i < GPIO_PE_LAST_INDEX; i++){
       gpio_set_output(i);
       gpio_write(i, 0);
       assert(gpio_read(i) ==  0);
     }
     for(int i = GPIO_PF0; i < GPIO_PF_LAST_INDEX; i++){
       gpio_set_output(i);
       gpio_write(i, 0);
       assert(gpio_read(i) ==  0);
     }
    // Edge case: Test gpio_write and gpio_read on a pin configured as input
    gpio_set_input(GPIO_PB5);
    gpio_write(GPIO_PB5, 1); // Writing to an input pin should not affect its state
    assert(gpio_read(GPIO_PB5) == 0); // gpio_read should still return 0 for an input pin

    // Edge case: Test gpio_write with invalid values
    gpio_set_output(GPIO_PB6);
    gpio_write(GPIO_PB6, 2); // Invalid value, should not affect the pin state
    assert(gpio_read(GPIO_PB6) == 0); // gpio_read should still return 0
     
}


static void test_leds(void) {
    // Test that gpio turns on
    // have gpio connected to LED circuit, confirm gpio_write turns LED on
    // selected gpios from different groups available on Pi's header
    gpio_id_t array[3] = {GPIO_PG12, GPIO_PB7, GPIO_PD21};
    for (int i = 0; i < 3; i++) {
        gpio_id_t pin = array[i];
        gpio_set_output(pin);
        gpio_write(pin, 1);
    }
}

static void test_timer(void) {
    // Test timer tick count incrementing
    unsigned long start = timer_get_ticks();
    for( int i=0; i<10; i++ ) { /* Spin */ }
    unsigned long finish = timer_get_ticks();
    assert( finish > start );

    // Test timer delay
    int usecs = 100;
    start = timer_get_ticks();
    timer_delay_us(usecs);
    finish = timer_get_ticks();
    assert( finish >= start + usecs*TICKS_PER_USEC );
}

static void test_breadboard_connections(void) {
    const int N_SEG = 7, N_DIG = 4;
    gpio_id_t segment[] = {GPIO_PD17, GPIO_PB6, GPIO_PB12, GPIO_PB11, GPIO_PB10, GPIO_PD11, GPIO_PD13};
    gpio_id_t digit[] = {GPIO_PB4, GPIO_PB3, GPIO_PB2, GPIO_PC0};
    gpio_id_t button = GPIO_PD12;

    for (int i = 0; i < N_SEG; i++) {   // configure segments & digits as output
        gpio_set_output(segment[i]);
    }
    for (int i = 0; i < N_DIG; i++) {
        gpio_set_output(digit[i]);
    }
    gpio_set_input(button);         // configure button

    while (1) { // loop forever
        for (int i = 0; i < N_DIG; i++) {   // iterate over digits
            gpio_write(digit[i], 1);        // turn on digit
            for (int j = 0; j < N_SEG; j++) {   // iterate over segments
                gpio_write(segment[j], 1);      // turn on segment
                timer_delay_ms(200);
                while (gpio_read(button) == 0)
                    ;                       // pause while button pressed
                gpio_write(segment[j], 0);  // turn off segment
            }
            gpio_write(digit[i], 0);    // turn off digit
        }
    }
}

void main(void) {
    gpio_init();
    timer_init();
   

 // Uncomment the call to each test function below when you have implemented
    // the functions and are ready to test them
    test_gpio_set_get_function();
//    test_gpio_read_write();
    // test_leds();
//    test_timer();
//    test_breadboard_connections();

    /***** TODO: Add more and better tests!  *****/
}
