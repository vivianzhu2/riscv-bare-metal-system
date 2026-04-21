
# File: timer_asm.s
# ------------------
#  implement the function timer_get_ticks in timer_asm.s 
#  to access the current tick count. 
#  This function uses CSRR to access timer CSR. 


.globl timer_get_ticks
timer_get_ticks:
   #access current tick count
   csrr a0, 0xc01;    # access time csr; follows the function call ABI and properly shares register a0
   ret 	
