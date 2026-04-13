/* File: larson.s
 * --------------
 * ***** TODO: add your file header comment here *****
 */

/*
 * The code below is copy of the blink program from lecture/lab.
 */
 .globl _start       # mark _start symbol where execution begins
 _start:
    lui     a0,0x2000       # a0 holds base addr PB group = 0x2000000
    li      t1, 0x11111
    lui     a1, 0x11
    addi    a1,a1,0x111      # a1 holds constant 1
    sw      a1,0x30(a0)      # config PB0 as output
    addi    a1, zero, 1      #
    li      a3, 5 
    li      a4, 4
    li      a5, 0
    sw      a1, 0x40(a0)

loop:       
    sw      a1,0x40(a0)     # set data value of PB0 to a1 
    slli    a1, a1, 0x1
    addi    a3, a3, -1
    lui     a2,0x3f00/3       # a2 = init countdown value
delay:                      # busy loop to wait for a while
    addi    a2,a2,-1        # decrement a2
    bne     a2,zero,delay   # keep counting down until a2 is zero
    beq     a3, a5, reverse
    j       loop            # back to top of outer loop

reverse:
   sw      a1, 0x40(a0)
   srli    a1, a1, 0x1
   addi    a3, a3, 1
   lui     a2, 0x3f00/3
delayr:
   addi    a2, a2, -1
   bne     a2,zero, delayr
   beq     a3, a4, loop
   j       reverse
