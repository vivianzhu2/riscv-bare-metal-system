# File: button.s
# ---------------
# LED connected to PB0, button connected to PC0
# Button configured with hardware pull-up resistor
# default state is high, when pressed, goes low
#
_start:
    lui     a0,0x2000       # a0 holds gpio base addr = 0x2000000
    addi    a1,zero,0x1     # a1 = 1
    sw      a1,0x30(a0)     # config PB0 as output
    sw      zero,0x60(a0)   # config PC0 as input

loop:
    lw      a1,0x70(a0)
    andi    a1,a1,1
    sw      a1,0x40(a0)
    j       loop

