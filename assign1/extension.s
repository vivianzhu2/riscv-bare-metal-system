 /* -------------- * 
Extension File for Larson Scanner: Added functions: config, forward, backward, and delay. Used named constants: NUM_LEDS, and FREQ_IN_HEX. Used a nested function where I explored the stack frame with the sp and their ra address.
 */
.globl _start       # mark _start symbol where execution begins

.equ NUM_LEDS,4
.equ FREQ, 0x2f5a
.equ FREQ_IN_S, 1
.equ FREQ_PER_5LEDS, (FREQ * FREQ_IN_S)
.equ FREQ_PER_LED, (FREQ_PER_5LEDS / 5)
.equ FREQ_IN_HEX, (FREQ_PER_LED * NUM_LEDS)  #final number hex used in the delay loop

_start:
    call config
loop:
    call forward
    call backward
    j loop


# Base Address of GPIO is 0x02000000
# PB GPIO Configure Register 0 (PB_CFG0) is at offset 0x30
# Configure PB_CFG0 to make PB0 to PB7 as outputs
config:
    lui     a0, 0x2000       # a0 holds base addr PB group = 0x2000000
    li      t1, 0x11111
    lui     a1, 0x11        # split a1 into two so it can hold more than 12 bits
    addi    a1, a1, 0x111   # a1 holds constant 1
    sw      a1, 0x30(a0)    # config PB0 as output

# Initialize the scan pattern values,counters and limits
# ------------------------------------------------------
    addi    a1, zero, 1     # a1 is the walking 1 pattern, initialized to 0x00000001
    li      a3, NUM_LEDS-1  # a3 is the counter for scan in one direction, initialized to NUM_LEDS-1
    li      a4, NUM_LEDS-1  # a4 is the limit for scan to right direction, initialized to NUM_LEDS-1
    li      a5, 0           # a5 is the limit for scan to left direction, initialized to 0
    ret


# forward scan with walking one
# -----------------------------
# PB GPIO DATA Register 0 (PB_DAT) is at offset 0x40
# driving PB0/1/2/3/4 with walking one pattern in a1 shifting left
# counting the LED position in a3
# light up for 1 second in one scan with delay using a2 counter
# revert the direction when counter a3 reaching 0
forward:
    sw      a1, 0x40(a0)    # set PB_DATA to light up the current LED
    slli    a1, a1, 0x1     # shift the walking one pattern 1 step to the left
    addi    a3, a3, -1      # decrement the counter
    lui     a2, FREQ_IN_HEX      # reload a2 with the countdown value to 1 round of scan of 5 LEDs with 1 sec frequency
    addi sp,sp, -16
    sd    ra, 8(sp)   
    call delay                      # delay loop
    ld    ra, 8(sp)
    addi  sp,sp, 16
    beq     a3, a5,jumpbackf # after NUM_LEDS-1 LEDs, switch direction to backward
    j       forward         # continue scan to the next LED forward
jumpbackf:       
    ret
# backward scan with walking one
# -----------------------------
# PB GPIO DATA Register 0 (PB_DAT) is at offset 0x40
# driving PB0/1/2/3/4 with walking one pattern in a1 shifting right
# counting the LED position in a3
# light up for 1 second in one scan with delay using a2 counter
# revert the direction when counter a3 reaching NUM_LEDS-1
backward:
   sw      a1, 0x40(a0)     # set PB_DATA to light up the current LED
   srli    a1, a1, 0x1      # shift the walking one pattern 1 step to the right
   addi    a3, a3, 1        # increment the counter

   lui     a2, FREQ_IN_HEX  # reload a2 with the countdown value to 1 round of scan of 5 LEDs with 1 sec frequency
    addi sp,sp, -16
    sd    ra, 8(sp)  
    call delay                      # delay loop
    ld    ra, 8(sp)
    addi  sp,sp, 16
   beq     a3, a4, jumpbackb  # after 4 iterations, switch dir to go back to forward
   j       backward         # continue scan to the next LED backward
jumpbackb:
   ret   
     
# delay function that counts down to zero
delay:
   addi a2, a2, -1
   bne  a2, zero, delay
   ret                      
