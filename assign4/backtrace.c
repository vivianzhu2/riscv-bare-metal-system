/* File: backtrace.c
 * -----------------
 * Implementation of backtrace functions to gather and print stack frames. 
 * The backtrace_gather_frames function fills an array of frame_t, one for each function on the stack. 
 * The first entry in the array is filled with the frame corresponding for the topmost function, 
 * the subsequent entry is the caller of that function and so on working backwards through the call stack. 
 * 
 * Set the __stack_chk_guard canary to the value of your choice, and finish the code for __stack_chk_fail 
 * to determine which function was responsible for the stack buffer overflow and report with a message.
 */
#include "backtrace.h"
#include "strings.h"
#include "mango.h"
#include "printf.h"
#include "strings.h"
#include "symtab.h"



int backtrace_gather_frames(frame_t f[], int max_frames) { // gather up to max_frames frames into f array, return count of frames gathered
    void *fp_reg;
    __asm__("mv %0, fp":"=r" (fp_reg));

    memset(f, 0, max_frames * sizeof(*f)); // do not remove this line
    // Get current fp (this is why they mention inline assembly)

    uintptr_t ra;
    int count = 0;
    // Treat it like a pointer to a structure in memory

    while(count < max_frames && fp_reg != NULL){ // stop when we have gathered max_frames or we have reached the end of the stack (NULL frame pointer)
        ra = (uintptr_t)*((uintptr_t*)fp_reg -1);
        f[count].resume_addr = ra;
        count += 1;
        printf("ra:%lx fp:%p\n", ra,fp_reg);
        fp_reg = (void *)(uintptr_t)(*((uintptr_t*)fp_reg - 2));
        printf("AFTER: ra:%lx fp:%p\n", ra,fp_reg);
    }

    return count;
}

void backtrace_print_frames(frame_t f[], int n) {
    char labelbuf[128];

    for (int i = 0; i < n; i++) {
        symtab_label_for_addr(labelbuf, sizeof(labelbuf), f[i].resume_addr);
        printf("#%d 0x%8lx at %s\n", i, f[i].resume_addr, labelbuf);
    }
}

void backtrace_print(void) {
    int max = 50;
    frame_t arr[max];

    int n = backtrace_gather_frames(arr, max);
    backtrace_print_frames(arr+1, n-1);   // print frames starting at this function's caller
}


/***** canary value and assign below.
       If you leave unassigned, the variable will be placed in BSS and will be initialized to zero.
       Why would a zero a bad choice for canary value?
*****/
long __stack_chk_guard = 107107107107; //canary value

void __stack_chk_fail(void)  { // called when stack smashing detected
 
    void *fp_reg;
    __asm__("mv %0, fp":"=r" (fp_reg)); // get current frame pointer

    uintptr_t ra = (uintptr_t)*((uintptr_t*)fp_reg -1); // get return address from current frame
    //printf("\n *** FP %p *** \n", (void*)fp_reg);
    //printf("\n *** RA %p *** \n", (void*)ra);   
    // uintptr_t ra_bg=ra+72-204; // adjust return address to point to the badguy function that was smashed
    //printf("\n *** RA instruction %lx *** \n", *(( uintptr_t *)ra_bg));   
    //void *ra_reg;
    //__asm__("mv %0, ra":"=r" (ra_reg));

    //printf("\n *** RA_REG %p *** \n", (void*)ra_reg);

    symbol_t output;
    //char outputc[20];
    const char *fn_name="TODO: add code to determine name of failing function";
    //char fn_name;   
    if(symtab_symbol_for_addr(((uintptr_t)ra - 4), &output)){ // if we can find the symbol for the adjusted return address, then we can get the function name
        fn_name = output.name;
    }
 
    printf("\n *** Stack smashing detected at end of function %s() *** \n", fn_name);
    mango_abort();

}
