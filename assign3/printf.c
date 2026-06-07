/* File: printf.c 
 * --------------
 * ***** TODO: add your file header comment here *****
 */
#include "printf.h"
#include <stdarg.h>
#include <stdint.h>
#include "strings.h"
#include "uart.h"

/* Prototypes for internal helpers.
 * Typically we would qualify these functions as static (private to module)
 * but in order to call them from the test program, must declare externally
 */
void num_to_string(unsigned long num, int base, char *outstr);
const char *hex_string(unsigned long val);
const char *decimal_string(long val);

// max number of digits in long + space for negative sign and null-terminator
#define MAX_DIGITS 25


/* Convenience functions `hex_string` and `decimal_string` are provided
 * to you.  You should use the functions as-is, do not change the code!
 *
 * A key implementation detail to note is these functions declare
 * a buffer to hold the output string and return the address of buffer
 * to the caller. If that buffer memory were located on stack, it would be
 * incorrect to use pointer after function exit because local variables
 * are deallocated. To ensure the buffer memory is accessible after
 * the function exists, the declaration is qualified `static`. Memory
 * for a static variable is not stored on stack, but instead in the global data
 * section, which exists outside of any function call. Additionally static
 * makes it so there is a single copy of the variable, which is shared by all
 * calls to the function. Each time you call the function, it overwrites/reuses
 * the same variable/memory.
 *
 * Adding static qualifier to a variable declared inside a function is a
 * highly atypical practice and appropriate only in very specific situations.
 * You will likely never need to do this yourself.
 * Come talk to us if you want to know more!
 */

const char *hex_string(unsigned long val) {
    // static buffer to allow use after function returns (see note above)
    static char buf[MAX_DIGITS];
    num_to_string(val, 16, buf); // num_to_string does the hard work
    return buf;
}

const char *decimal_string(long val) {
    // static buffer to allow use after function returns (see note above)
    static char buf[MAX_DIGITS];
    if (val < 0) {
        buf[0] = '-';   // add negative sign in front first
        num_to_string(-val, 10, buf + 1); // pass positive val as arg, start writing at buf + 1
    } else {
        num_to_string(val, 10, buf);
    }
    return buf;
}
/**
 * Converts an unsigned long integer to its string representation in the specified base.
 *
 * @param num    The unsigned long integer to convert.
 * @param base   The numerical base for conversion (e.g., 2 for binary, 10 for decimal, 16 for hex).
 * @param outstr Output buffer where the resulting null-terminated string will be stored.
 *               Caller is responsible for ensuring the buffer is large enough to hold the result.
 */
void num_to_string(unsigned long num, int base, char *outstr) {
    char* reversed = outstr;
    if (base == 10){
        while(num / 10  > 0){
            unsigned long digit = num % base;
            *reversed = (char)(digit + '0');
            reversed++;
            num/=10;
        }
        *reversed = (char)(num + '0');
        reversed++;
    }
    if(base == 16){
        while(num / 16 > 0){
            unsigned long int digit = num % base;
            if(digit < 10){
                *reversed = (char)(digit + '0');
            }
            else{
                *reversed = (char)(digit - 10 + 'a');
            }
            reversed++;
            num /= 16;
        }
        
        if(num < 10){
            *reversed = (char)(num + '0');
        }
        else{
            *reversed = (char)(num - 10 + 'a');
        }
        reversed++;
    }
    *reversed = '\0';
    // reversed[strlen(reversed)] = '\0';  
    char temp;

    reversed = outstr; 
    int slen= strlen(reversed);
    for(int i = 0; i < slen/2; i++){
        // temp = reversed[strlen(reversed) - 1 - i];
        temp = outstr[i];
        outstr[i] = outstr[slen- 1 - i];
        outstr[slen -1 - i] = temp;

    }

    //append null terminator at the end of the string
    outstr[strlen(reversed)] = '\0';
    return;

    //reverse the string now

}
/* vsnprintf: the main workhorse behind printf/snprintf.
 * Walks through the format string one character at a time — plain characters
 * get copied straight to buf, and % specifiers get replaced with formatted args.
 * Won't write past bufsize, but still returns how many chars the full output
 * would have been (so callers can detect truncation).
 *
 * Handles: %c, %s, %d, %x, %p, %ld, %lx, %%
 * Also handles field widths like %8d or %04x.
 */
int vsnprintf(char *buf, size_t bufsize, const char *format, va_list args) {
    if(bufsize != 0){
        buf[0] = '\0'; // if buffer size is 0, return length of full output without writing anything
    }
     // start with empty string in buffer
    int exp_length = 0; // return value from strlcat (expected final length)
    char temp[2] = "";
    char *new = temp;
    int fw = 0; 
    int k = 0;
    int has_width=0;
    char tempfw[32767];
    int overflow = 0;
    int totallen= 0;
    int negwidth = 0;
    int star = 0;
    // va_start(args, format); // init va_list, read arguments following argument named n
    for(int i = 0; i<strlen(format); i++){
        if((format[i] == '%') && (i < strlen(format) - 1)){// found a % specifier, and it's not the last character in the string
            k= i + 1;
            negwidth = 0; // marker to see whether to pad on the left or the right
            star = 0; // marker to see whether to look for the next argument as field width
            if(format[k] == '*'){
                int arg = va_arg(args, int); //look for fieldwidth in separate arg
                fw = arg;
                if(arg<0){
                    fw = -arg; 
                    negwidth = 1; //preserve negativity padding
                }
                star = 1;
                i++;
            }
            if(star==0){
                if(format[k] == '-'){
                    negwidth = 1; 
                    k++;                
                }
                tempfw[0] = '\0';
                while(format[k] >= '0' && format[k] <= '9' && star == 0){
                    tempfw[k - i - 1 -negwidth] = format[k];// collect width digits into temp buffer
                    k++;
                    has_width=1; // mark that we found at least one width digit
                    
                }
    
                
                if (has_width==1) {
                    tempfw[k - i - 1] = '\0';
                    fw = strtonum(tempfw, NULL); // convert collected digits to integer width
                }
                else {
                    fw=0;
                }
                i = k-1;
            }


            if(format[i + 1] == '%'){// case where %% becomes %
                // *new = '%';
                // new ++;
                // *new = '\0';
                // new--;
                char new2[2];
                new2[0] = format[i+1];
                new2[1] = '\0';
                if(bufsize <= (strlen(buf) + strlen(new2))){ //buffer check
                    exp_length = strlcat(buf,new2, bufsize);
                    if(overflow == 1){
                        totallen += strlen(new2); // already overflowed: accumulate manually
                    }
                    else{
                        totallen = exp_length;// first overflow: use strlcat return value
                    }
                    overflow = 1;
                }
                else{ // no overflow yet: use strlcat return value
                    exp_length = strlcat(buf,new2, bufsize);
                    totallen = exp_length;
                }
                i++;
            }
            else if(format[i + 1] == 'c'){ // %c becomes single character
                char arg = (char)va_arg(args, int); // char is promoted to int in varargs

                // *new = arg;
                // new ++;
                // *new = '\0';
                // new--;

                char new2[2];
                new2[0] = arg;
                new2[1] = '\0';

                char result[fw+1];  // buffer for padded
                // result[0] = '\0';       
                int npadding = 0;
     
                if( fw > strlen(new2)){ // field width wider than character: pad with spaces
                    npadding = fw - strlen(new2);
                    if(negwidth == 0){ // if positive pad to the left
                        for( int j = 0; j<npadding +strlen(new2); j++){
                            if(j < npadding){
                                result[j] = ' '; // leading space padding
                            }
                            else{
                                result[j] = new2[j - npadding];   //copy the remaining sources
                            }
                        }          
                    }
                    else{ // if negative pad to the right 
                        for( int j = 0; j<npadding +strlen(new2); j++){  
                            if(j < strlen(new2)){
                                result[j] = new2[j]; // first append string
                            }
                            else{
                               result[j] = ' '; // then append
                            }
                        } 
                    }
                    result[npadding + strlen(new2)] = '\0';
                    // new2 = result;
                    if(bufsize <= (strlen(buf) + strlen(result))){// buffer check for padded string
                        exp_length = strlcat(buf,result, bufsize);
                        if(overflow == 1){ // already overflowed: accumulate manually
                            totallen += strlen(result);
                        }
                        else{
                            totallen = exp_length;
                        }
                        overflow = 1;
                    }
                    else{ // no overflow yet: use strlcat return value
                        exp_length = strlcat(buf,result, bufsize); // add the padded string to the original
                        totallen = exp_length;
                    }
                }
                else{ // no padding needed
                    // exp_length = strlcat(buf,strarg, bufsize);
                    if(bufsize <= (strlen(buf) + strlen(new2))){ //buffer check
                        exp_length = strlcat(buf,new2, bufsize); 
                        if(overflow == 1){ 
                            totallen += strlen(new2);
                        }
                        else{ 
                            totallen = exp_length;
                        }
                        overflow = 1;
                    }
                    else{ 
                        exp_length = strlcat(buf,new2, bufsize);  
                        totallen = exp_length;
                    }
                }

                i++;

            }
            else if(format[i + 1] == 's'){
                char* arg = va_arg(args, char*);
                //new = arg;
                char result[fw + 1];         
                int npadding = 0;
                if( fw > strlen(arg)){
                    npadding = fw - strlen(arg);
                    // for( int j = 0; j<npadding +strlen(arg); j++){
                    //     if(j < npadding){
                    //         result[j] = ' ';
                    //     }
                    //     else{
                    //         result[j] = arg[j - npadding];
                    //     }
                    // } 
                    if(negwidth == 0){
                        for( int j = 0; j<npadding +strlen(arg); j++){
                            if(j < npadding){
                                result[j] = ' ';
                            }
                            else{
                                result[j] = arg[j - npadding];
                            }
                        }
                    }
                    else{
                        for( int j = 0; j<npadding +strlen(arg); j++){
                            if(j < strlen(arg)){
                                result[j] = arg[j];
                            }
                            else{
                               result[j] = ' ';
                            }
                        } 
                    }
                    result[npadding + strlen(arg)] = '\0'; 
                    new = result;
                    if(bufsize <= (strlen(buf) + strlen(new))){
                        exp_length = strlcat(buf,new, bufsize);
                        if(overflow == 1){
                            totallen += strlen(new);
                        }
                        else{
                            totallen = exp_length;
                            // totallen = strlen(buf) + strlen(new);
                        }
                        overflow = 1;
                    }
                    else{
                        exp_length = strlcat(buf,new, bufsize);
                        totallen = exp_length;
                    }
                }
                else{
                    // exp_length = strlcat(buf,strarg, bufsize);
                    new = arg;
                    if(bufsize <= (strlen(buf) + strlen(new))){
                        exp_length = strlcat(buf,new, bufsize);
                        if(overflow == 1){
                            totallen += strlen(new);
                        }
                        else{
                            totallen = exp_length;
                            // totallen = strlen(buf) + strlen(new);
                        }
                        overflow = 1;
                    }
                    else{
                        exp_length = strlcat(buf,new, bufsize);
                        totallen = exp_length;
                    }
                }
                i++;
            }
            else if(format[i + 1] == 'd'){
                int arg = va_arg(args, int);
                const char* strarg = decimal_string(arg);
                char result[fw + 1];         
                int npadding = 0;
                if( fw > strlen(strarg)){
                    npadding = fw - strlen(strarg);

                    if(negwidth == 0){
                        for( int j = 0; j<npadding +strlen(strarg); j++){
                            if(j < npadding){
                                result[j] = ' ';
                            }
                            else{
                                result[j] = strarg[j - npadding];
                            }
                        } 
                    }
                    else{
                        for( int j = 0; j<npadding +strlen(strarg); j++){
                            if(j < strlen(strarg)){
                                result[j] = strarg[j];
                            }
                            else{
                               result[j] = ' ';
                            }
                        } 
                    }

                    // for( int j = 0; j<npadding +strlen(strarg); j++){
                    //     if(j < npadding){
                    //         result[j] = ' ';
                    //     }
                    //     else{
                    //         result[j] = strarg[j - npadding];
                    //     }
                    // } 
                    result[npadding + strlen(strarg)] = '\0'; 
                    new = result;
                    if(bufsize <= (strlen(buf) + strlen(new))){
                        exp_length = strlcat(buf,new, bufsize);
                        if(overflow == 1){
                            totallen += strlen(new);
                        }
                        else{
                            totallen = exp_length;
                        }
                        overflow = 1;
                    }
                    else{
                        exp_length = strlcat(buf,new, bufsize);
                        totallen = exp_length;
                    }
                }
                else{
                    // exp_length = strlcat(buf,strarg, bufsize);
                    //new = strarg;
                    if(bufsize <= (strlen(buf) + strlen(strarg))){
                        exp_length = strlcat(buf,strarg, bufsize);
                        if(overflow == 1){
                            totallen += strlen(strarg);
                        }
                        else{
                            totallen = exp_length;
                        }
                        overflow = 1;
                    }
                    else{
                        exp_length = strlcat(buf,strarg, bufsize);
                        totallen = exp_length;
                    }
                }
                i++;
            }
            else if(format[i + 1] == 'x'){
                unsigned int arg = va_arg(args, unsigned int);

                const char* strarg = hex_string(arg);
                char result[fw + 1];         
                int npadding = 0;
                if( fw > strlen(strarg)){
                    npadding = fw - strlen(strarg);
                    // for( int j = 0; j<npadding +strlen(strarg); j++){
                    //     if(j < npadding){
                    //         result[j] = '0';
                    //     }
                    //     else{
                    //         result[j] = strarg[j - npadding];
                    //     }
                    // }  
                    if(negwidth == 0){
                        for( int j = 0; j<npadding +strlen(strarg); j++){
                            if(j < npadding){
                                result[j] = '0';
                            }
                            else{
                                result[j] = strarg[j - npadding];
                            }
                        } 
                    }
                    else{
                        for( int j = 0; j<npadding +strlen(strarg); j++){
                            if(j < strlen(strarg)){
                                result[j] = strarg[j];
                            }
                            else{
                               result[j] = '0';
                            }
                        } 
                    }

                    result[npadding + strlen(strarg)] = '\0';
                    new = result;
                    if(bufsize <= (strlen(buf) + strlen(new))){
                        exp_length = strlcat(buf,new, bufsize);
                        if(overflow == 1){
                            totallen += strlen(new);
                        }
                        else{
                            totallen = exp_length;
                        }
                        overflow = 1;
                    }
                    else{
                        exp_length = strlcat(buf,new, bufsize);
                        totallen = exp_length;
                    }
                }
                else{
                    // exp_length = strlcat(buf,strarg, bufsize);
                    //new = strarg;
                    if(bufsize <= (strlen(buf) + strlen(strarg))){
                        exp_length = strlcat(buf,strarg, bufsize);
                        if(overflow == 1){
                            totallen += strlen(strarg);
                        }
                        else{
                            totallen = exp_length;
                        }
                        overflow = 1;
                    }
                    else{
                        exp_length = strlcat(buf,strarg, bufsize);
                        totallen = exp_length;
                    }
                }
                i++;
            }
            else if(format[i + 1] == 'p'){
                unsigned long arg = va_arg(args, unsigned long);

                const char* strarg = hex_string(arg);
    
                if (fw == 0){
                    fw = 8;
                }     
                char result[100];
                int npadding = 0;
                result[0] = '0';
                result[1] = 'x';

                if( fw > strlen(strarg)){
                    npadding = fw - strlen(strarg);
                    // for( int j = 2; j<npadding +strlen(strarg) + 2; j++){
                    //     if(j < npadding + 2){
                    //         result[j] = '0';
                    //     }
                    //     else{
                    //         result[j] = strarg[j - npadding - 2];
                    //     }
                    // }  
                    if(negwidth == 0){
                        for( int j = 2; j<npadding +strlen(strarg) + 2; j++){
                            if(j < npadding + 2){
                                result[j] = '0';
                            }
                            else{
                                result[j] = strarg[j - npadding - 2];
                            }
                        }   
                    }
                    else{
                        for( int j = 2; j<npadding +strlen(strarg) + 2; j++){
                            if(j < strlen(strarg) + 2){
                                result[j] = strarg[j - 2];
                            }
                            else{
                               result[j] = '0';
                            }
                        } 
                    }

                    result[npadding + strlen(strarg) + 2] = '\0';
                    new = result;


                    if(bufsize <= (strlen(buf) + strlen(new))){
                        exp_length = strlcat(buf,new, bufsize);
                        if(overflow == 1){
                            totallen += strlen(new);
                        }
                        else{
                            totallen = exp_length;
                        }
                        overflow = 1;
                    }
                    else{

                        exp_length = strlcat(buf,new, bufsize);
                        totallen = exp_length;
                    }
                }
                else{
                    // exp_length = strlcat(buf,strarg, bufsize);
                    //new = strarg;
                    npadding = 0;
                    for( int j = 2; j<npadding +strlen(strarg) + 2; j++){
                        if(j < strlen(strarg) + 2){
                            result[j] = strarg[j - 2];
                        }
                        else{
                           result[j] = '0';
                        }
                    } 
                    result[ strlen(strarg) + 2] = '\0';
                    new = result;
                    if(bufsize <= (strlen(buf) + strlen(new))){
                        exp_length = strlcat(buf,new, bufsize);
                        if(overflow == 1){
                            totallen += strlen(new);
                        }
                        else{
                            totallen = exp_length;
                        }
                        overflow = 1;
                    }
                    else{
                        exp_length = strlcat(buf,new, bufsize);
                        totallen = exp_length;
                    }
                }
                // if(exp_length > strlen(buf)){
                //     return exp_length;
                // }
                i++;
            }
            else if(i <strlen(format) - 2){
                if(format[i + 1] == 'l' && format[i+2] == 'd'){
                    long arg = va_arg(args, long);
                    const char* strarg = decimal_string(arg);
                    char result[fw + 1];         
                    int npadding = 0;
                    if( fw > strlen(strarg)){
                        npadding = fw - strlen(strarg);
                        // for( int j = 0; j<npadding +strlen(strarg); j++){
                        //     if(j < npadding){
                        //         result[j] = ' ';
                        //     }
                        //     else{
                        //         result[j] = strarg[j - npadding];
                        //     }
                        // }  
                        if(negwidth == 0){
                            for( int j = 0; j<npadding +strlen(strarg); j++){
                                if(j < npadding){
                                    result[j] = ' ';
                                }
                                else{
                                    result[j] = strarg[j - npadding];
                                }
                            }  
                        }
                        else{
                            for( int j = 0; j<npadding +strlen(strarg); j++){
                                if(j < strlen(strarg)){
                                    result[j] = strarg[j];
                                }
                                else{
                                   result[j] = ' ';
                                }
                            } 
                        }
                        result[npadding + strlen(strarg)] = '\0';
                        new = result;
                        if(bufsize <= (strlen(buf) + strlen(new))){
                            exp_length = strlcat(buf,new, bufsize);
                            if(overflow == 1){
                                totallen += strlen(new);
                            }
                            else{
                                totallen = exp_length;
                            }
                            overflow = 1;
                        }
                        else{
                            exp_length = strlcat(buf,new, bufsize);
                            totallen = exp_length;
                        }
                    }
                    else{
                        // exp_length = strlcat(buf,strarg, bufsize);
                        // new = strarg;
                        if(bufsize <= (strlen(buf) + strlen(strarg))){
                            exp_length = strlcat(buf,strarg, bufsize);
                            if(overflow == 1){
                                totallen += strlen(strarg);
                            }
                            else{
                                totallen = exp_length;
                            }
                            overflow = 1;
                        }
                        else{
                            exp_length = strlcat(buf,strarg, bufsize);
                            totallen = exp_length;
                        }
                    }
                    // if(exp_length > strlen(buf)){
                    //     return exp_length;
                    // }
                    
                    i+=2;
                }
                if(format[i + 1] == 'l' && format[i+2] == 'x'){
                    unsigned long arg = va_arg(args, unsigned long);
                    const char* strarg = hex_string(arg);
                    char result[fw + 1];         
                    int npadding = 0;
                    if( fw > strlen(strarg)){
                        npadding = fw - strlen(strarg);
                        // for( int j = 0; j<npadding +strlen(strarg); j++){
                        //     if(j < npadding){
                        //         result[j] = '0';
                        //     }
                        //     else{
                        //         result[j] = strarg[j - npadding];
                        //     }
                        // }  
                        if(negwidth == 0){
                            for( int j = 0; j<npadding +strlen(strarg); j++){
                                if(j < npadding){
                                    result[j] = '0';
                                }
                                else{
                                    result[j] = strarg[j - npadding];
                                }
                            }  
                        }
                        else{
                            for( int j = 0; j<npadding +strlen(strarg); j++){
                                if(j < strlen(strarg)){
                                    result[j] = strarg[j];
                                }
                                else{
                                   result[j] = '0';
                                }
                            } 
                        }
                        result[npadding + strlen(strarg)] = '\0';
                        // exp_length = strlcat(buf,result, bufsize); 
                        new = result;
                        if(bufsize <= (strlen(buf) + strlen(new))){
                            exp_length = strlcat(buf,new, bufsize);
                            if(overflow == 1){
                                totallen += strlen(new);
                            }
                            else{
                                totallen = exp_length;
                            }
                            overflow = 1;
                        }
                        else{
                            exp_length = strlcat(buf,new, bufsize);
                            totallen = exp_length;
                        }
                    }
                    else{
                        // exp_length = strlcat(buf,strarg, bufsize);
                        // new = strarg;
                        if(bufsize <= (strlen(buf) + strlen(strarg))){
                            exp_length = strlcat(buf,strarg, bufsize);
                            if(overflow == 1){
                                totallen += strlen(strarg);
                            }
                            else{
                                totallen = exp_length;
                            }
                            overflow = 1;
                        }
                        else{
                            exp_length = strlcat(buf,strarg, bufsize);
                            totallen = exp_length;
                        }
                    }
                    // totallen+= exp_length;
                    // if(exp_length > strlen(buf)){
                    //     return exp_length;
                    // }
                    i+=2;
                }
            }

            else{

                break;
            }

        }
        else{
            char new2[2];
            new2[0] = format[i];
            new2[1] = '\0';
            // new2++;
            // *new2 = '\0';
            // new2--;


            if(bufsize <= (strlen(buf) + strlen(new2))){
                exp_length = strlcat(buf,new2, bufsize);
                if(overflow == 1){
                    totallen += strlen(new2);
                }
                else{
                    totallen = exp_length;
                }
                overflow = 1;
            }
            else{
                exp_length = strlcat(buf,new2, bufsize);
                totallen = exp_length;
            }
            
            // if(totallen > strlen(buf)){
            //     return totallen;
            // }
        }

    }
    // if(bufsize == 0){
    //    buf = NULL; // if buffer size is 0, return length of full output without writing anything
    // }
    // va_end(args);
    return  totallen;


}


int snprintf(char *buf, size_t bufsize, const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    int result = vsnprintf(buf, bufsize, format, ap);
    va_end(ap);
    return result;
    // /***** TODO: Your code goes here *****/
    // buf[0] = '\0'; // start with empty string in buffer
    // // int length = 0;
    // int exp_length = 0;
    // va_list ap; // declare va_list
    // char temp[2] = "";
    // char *new = temp;
    // int fw = 0; 
    // int k = 0;
    // int has_width=0;
    // char tempfw[32767];
    // va_start(ap, format); // init va_list, read arguments following argument named n
    // for(int i = 0; i<strlen(format); i++){
    //     if((format[i] == '%') && (i < strlen(format) - 1)){
    //         // loop through format for all the decimal digits until dont have any more numbers
    //         // save that integer
    //         // function for padding
    //         // function for truncating
    //         k= i + 1;
    //         while(format[k] >= '0' && format[k] <= '9'){
    //             tempfw[k - i - 1] = format[k];
    //             k++;
    //             has_width=1;
                
    //         }
    //         i = k-1;
    //         if (has_width==1) {
    //             tempfw[k] = '\0';
    //             fw = strtonum(tempfw, NULL);
    //         }
    //         else {
    //             fw=0;
    //         }

    //         if(format[i + 1] == '%'){
    //             *new = '%';
    //             new ++;
    //             *new = '\0';
    //             new--;
    //             exp_length = strlcat(buf,new, bufsize);
    //             if(exp_length > strlen(buf)){
    //                 return exp_length;
    //             }
                
    //             i++;
    //         }
    //         else if(format[i + 1] == 'c'){
    //             char arg = (char)va_arg(ap, int);

    //             *new = arg;
    //             new ++;
    //             *new = '\0';
    //             new--;
    //             char result[fw+1];  
    //             // result[0] = '\0';       
    //             int npadding = 0;
    //             if( fw > strlen(new)){
    //                 npadding = fw - strlen(new);
    //                 for( int j = 0; j<npadding +strlen(new); j++){
    //                     if(j < npadding){
    //                         result[j] = ' ';
    //                     }
    //                     else{
    //                         result[j] = new[j - npadding];
    //                     }
    //                 }  
    //                 result[npadding + strlen(new)] = '\0';
    //                 exp_length = strlcat(buf,result, bufsize);
    //             }
    //             else{
    //                 exp_length = strlcat(buf,new, bufsize);
    //             }
    //             if(exp_length > strlen(buf)){
    //                 return exp_length;
    //             }
               
    //             i++;

    //         }
    //         else if(format[i + 1] == 's'){
    //             char* arg = va_arg(ap, char*);
    //             //new = arg;
    //             char result[fw + 1];         
    //             int npadding = 0;
    //             if( fw > strlen(arg)){
    //                 npadding = fw - strlen(arg);
    //                 for( int j = 0; j<npadding +strlen(arg); j++){
    //                     if(j < npadding){
    //                         result[j] = ' ';
    //                     }
    //                     else{
    //                         result[j] = arg[j - npadding];
    //                     }
    //                 } 
    //                 // result[npadding + strlen(arg)] = '\0'; 
    //                 exp_length = strlcat(buf,result, bufsize);
    //             }
    //             else{
    //                 exp_length = strlcat(buf,arg, bufsize);
    //             }
    //             if(exp_length > strlen(buf)){
    //                 return exp_length;
    //             }
    //             i++;
    //         }
    //         else if(format[i + 1] == 'd'){
    //             int arg = va_arg(ap, int);
    //             const char* strarg = decimal_string(arg);
    //             char result[fw + 1];         
    //             int npadding = 0;
    //             if( fw > strlen(strarg)){
    //                 npadding = fw - strlen(strarg);
    //                 for( int j = 0; j<npadding +strlen(strarg); j++){
    //                     if(j < npadding){
    //                         result[j] = '0';
    //                     }
    //                     else{
    //                         result[j] = strarg[j - npadding];
    //                     }
    //                 } 
    //                 result[npadding + strlen(strarg)] = '\0'; 
    //                 exp_length = strlcat(buf,result, bufsize);
    //             }
    //             else{
    //                 exp_length = strlcat(buf,strarg, bufsize);
    //             }
    //             if(exp_length > strlen(buf)){
    //                 return exp_length;
    //             }
    //             i++;
    //         }
    //         else if(format[i + 1] == 'x'){
    //             int arg = va_arg(ap, int);

    //             const char* strarg = hex_string(arg);
    //             char result[fw + 1];         
    //             int npadding = 0;
    //             if( fw > strlen(strarg)){
    //                 npadding = fw - strlen(strarg);
    //                 for( int j = 0; j<npadding +strlen(strarg); j++){
    //                     if(j < npadding){
    //                         result[j] = '0';
    //                     }
    //                     else{
    //                         result[j] = strarg[j - npadding];
    //                     }
    //                 }  
    //                 result[npadding + strlen(strarg)] = '\0';
    //                 exp_length = strlcat(buf,result, bufsize);
    //             }
    //             else{
    //                 exp_length = strlcat(buf,strarg, bufsize);
    //             }
    //             if(exp_length > strlen(buf)){
    //                 return exp_length;
    //             }
    //             i++;
    //         }
    //         else if(format[i + 1] == 'p'){
    //             unsigned long arg = va_arg(ap, unsigned long);

    //             const char* strarg = hex_string(arg);
    //             char result[9];         
    //             int npadding = 0;
    //             if( 8 > strlen(strarg)){
    //                 npadding = 8 - strlen(strarg);
    //                 for( int j = 0; j<npadding +strlen(strarg); j++){
    //                     if(j < npadding){
    //                         result[j] = '0';
    //                     }
    //                     else{
    //                         result[j] = strarg[j - npadding];
    //                     }
    //                 }  
    //                 result[npadding + strlen(strarg)] = '\0';
    //                 exp_length = strlcat(buf,result, bufsize);
    //             }
    //             else{
    //                 exp_length = strlcat(buf,strarg, bufsize);
    //             }
    //             if(exp_length > strlen(buf)){
    //                 return exp_length;
    //             }
    //             i++;
    //         }
    //         else if(i <strlen(format) - 2){
    //             if(format[i + 1] == 'l' && format[i+2] == 'd'){
    //                 long arg = va_arg(ap, long);
    //                 const char* strarg = decimal_string(arg);
    //                 char result[fw + 1];         
    //                 int npadding = 0;
    //                 if( fw > strlen(strarg)){
    //                     npadding = fw - strlen(strarg);
    //                     for( int j = 0; j<npadding +strlen(strarg); j++){
    //                         if(j < npadding){
    //                             result[j] = '0';
    //                         }
    //                         else{
    //                             result[j] = strarg[j - npadding];
    //                         }
    //                     }  
    //                     result[npadding + strlen(strarg)] = '\0';
    //                     exp_length = strlcat(buf,result, bufsize);
    //                 }
    //                 else{
    //                     result[npadding + strlen(strarg)] = '\0';
    //                     exp_length = strlcat(buf,strarg, bufsize);
    //                 }
    //                 if(exp_length > strlen(buf)){
    //                     return exp_length;
    //                 }
    //                 i+=2;
    //             }
    //             if(format[i + 1] == 'l' && format[i+2] == 'x'){
    //                 unsigned long arg = va_arg(ap, unsigned long);
    //                 const char* strarg = hex_string(arg);
    //                 char result[fw + 1];         
    //                 int npadding = 0;
    //                 if( fw > strlen(strarg)){
    //                     npadding = fw - strlen(strarg);
    //                     for( int j = 0; j<npadding +strlen(strarg); j++){
    //                         if(j < npadding){
    //                             result[j] = '0';
    //                         }
    //                         else{
    //                             result[j] = strarg[j - npadding];
    //                         }
    //                     }  
    //                     result[npadding + strlen(strarg)] = '\0';
    //                     exp_length = strlcat(buf,result, bufsize);
    //                 }
    //                 else{
    //                     exp_length = strlcat(buf,strarg, bufsize);
    //                 }

    //                 if(exp_length > strlen(buf)){
    //                     return exp_length;
    //                 }
    //                 i+=2;
    //             }
    //         }

    //         else{

    //             break;
    //         }

    //     }
    //     else{
    //         *new = format[i];
    //         new++;
    //         *new = '\0';
    //         new--;
    //         exp_length = strlcat(buf,new, bufsize);
    //         if(exp_length > strlen(buf)){
    //             return exp_length;
    //         }
    //     }

    // }
    
    // va_end(ap);
    // return  exp_length;


    // // va_list ap; // declare va_list

    // // va_start(ap, n); // init va_list, read arguments following argument named n
    // // for (int i = 0; i < n; i++) {
    // //    int arg = va_arg(ap, int); // access value of next argument, type is `int`
    // //    result += arg;
    // // }
    // // va_end(ap);   // clean up



    // // if(format[1] == '%' && ap == '%'){

    // // }
    
    // // return (int)strlcat(buf, format, bufsize);

   
}

int printf(const char *format, ...) {
    const int MAX_OUTPUT_LEN = 1024; // ok to assume printf output never longer than this
    /***** TODO: Your code goes here *****/
    char buf[MAX_OUTPUT_LEN];
    int bufsize = MAX_OUTPUT_LEN;

    va_list ap;
    va_start(ap, format);
    int result = vsnprintf(buf, bufsize, format, ap);
    va_end(ap);
    uart_putstring(buf);
    return result;
}
