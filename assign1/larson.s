# File: larson.s
# --------------
# Generate Larson scanner pattern with 5 LEDs

.globl _start       # mark _start symbol where execution begins

# Configure PB0/PB1/PB2/PB4 for output
# ------------------------------------
# Base Address of GPIO is 0x02000000
# PB GPIO Configure Register 0 (PB_CFG0) is at offset 0x30
# Configure PB_CFG0 to make PB0 to PB7 as outputs
_start:
    lui     a0, 0x2000       # a0 holds base addr PB group = 0x2000000
    li      t1, 0x11111
    lui     a1, 0x11        # split a1 into two so it can hold more than 12 bits
    addi    a1, a1, 0x111   # a1 holds constant 1
    sw      a1, 0x30(a0)    # config PB0 as output

# Initialize the scan pattern values,counters and limits
# ------------------------------------------------------
    addi    a1, zero, 1     # a1 is the walking 1 pattern, initialized to 0x00000001
    li      a3, 4           # a3 is the counter for scan in one direction, initialized to NUM_LEDS-1
    li      a4, 4           # a4 is the limit for scan to right direction, initialized to NUM_LEDS-1
    li      a5, 0           # a5 is the limit for scan to left direction, initialized to 0

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

    lui     a2, 0x2f5a      # reload a2 with the countdown value to 1 round of scan of 5 LEDs with 1 sec frequency
delay:                      # delay loop
    addi    a2, a2, -1      # decrement delay loop
    bne     a2, zero, delay # keep counting down until a2 is zero
    beq     a3, a5,backward # after NUM_LEDS-1 LEDs, switch direction to backward
    j       forward         # continue scan to the next LED forward

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

   lui     a2, 0x2f5a       # reload a2 with the countdown value to 1 round of scan of 5 LEDs with 1 sec frequency
delayr:                     # delay loop
   addi    a2, a2, -1       # decrement delay loop
   bne     a2,zero, delayr  # keep counting down until a2 is zero
   beq     a3, a4, forward  # after 4 iterations, switch dir to go back to forward
   j       backward         # continue scan to the next LED backward
