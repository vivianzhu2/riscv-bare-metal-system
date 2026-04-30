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
    if (base == 10){ // for decimal
        while(num / 10  > 0){
            unsigned long digit = num % base; // isolate digit
            *reversed = (char)(digit + '0'); // convert to char
            reversed++;
            num/=10;
        }
        *reversed = (char)(num + '0'); 
        reversed++;
    }
    if(base == 16){ // hex 
        while(num / 16 > 0){
            unsigned long int digit = num % base; //isolate digit
            if(digit < 10){
                *reversed = (char)(digit + '0'); // convert to char
            }
            else{
                *reversed = (char)(digit - 10 + 'a'); // address the characters greater than 10
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
    
    for(int i = 0; i < slen/2; i++){ //reverse, because num is lifo

        temp = outstr[i];
        outstr[i] = outstr[slen- 1 - i];
        outstr[slen -1 - i] = temp;

    }

    //append null terminator at the end of the string
    outstr[strlen(reversed)] = '\0';
    return;



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

    buf[0] = '\0'; // start with empty string in buffer
    int exp_length = 0; // return value from strlcat (expected final length)
    char temp[2] = "";
    char *new = temp;
    int fw = 0; 
    int k = 0;
    int has_width=0;
    char tempfw[32767];
    int overflow = 0;
    int totallen= 0;
    // va_start(args, format); // init va_list, read arguments following argument named n
    for(int i = 0; i<strlen(format); i++){
        if((format[i] == '%') && (i < strlen(format) - 1)){ // found a % specifier, and it's not the last character in the string
            //has_width=0;
            k= i + 1;
         // scan ahead past any field-width digits (ex the "8" in %8d)
            while(format[k] >= '0' && format[k] <= '9'){
                tempfw[k - i - 1] = format[k];  // collect width digits into temp buffer
                k++;
                has_width=1;  // mark that we found at least one width digit
                
            }
            i = k-1;
            if (has_width==1) {
                tempfw[k] = '\0';
                fw = strtonum(tempfw, NULL);   // convert collected digits to integer width
            }
            else {
                fw=0;
            }

            if(format[i + 1] == '%'){ // case where %% becomes %
                *new = '%';
                new ++;
                *new = '\0';
                new--;
                if(bufsize <= (strlen(buf) + strlen(new))){ //buffer check
                    exp_length = strlcat(buf,new, bufsize);
                    if(overflow == 1){
                        totallen += strlen(new);   // already overflowed: accumulate manually
                    }
                    else{
                        totallen = exp_length;// first overflow: use strlcat return value
                    }
                    overflow = 1;
                }
                else{  // no overflow yet: use strlcat return value
                    exp_length = strlcat(buf,new, bufsize);
                    totallen = exp_length;
                }
                i++;
            }
            else if(format[i + 1] == 'c'){  // %c becomes single character
                char arg = (char)va_arg(args, int); // char is promoted to int in varargs

                *new = arg;
                new ++;
                *new = '\0';
                new--;
                char result[fw+1];  // buffer for padded
                // result[0] = '\0';       
                int npadding = 0;
                if( fw > strlen(new)){  // field width wider than character: pad with spaces
                    npadding = fw - strlen(new);
                    for( int j = 0; j<npadding +strlen(new); j++){
                        if(j < npadding){
                            result[j] = ' ';  // leading space padding
                        }
                        else{
                            result[j] = new[j - npadding]; //copy the remaining sources
                        }
                    }  
                    result[npadding + strlen(new)] = '\0';
                    new = result;
                    if(bufsize <= (strlen(buf) + strlen(new))){  // buffer check for padded string
                        exp_length = strlcat(buf,new, bufsize);
                        if(overflow == 1){ // already overflowed: accumulate manually
                            totallen += strlen(new); 
                        }
                        else{ // first overflow: use strlcat return value
                            totallen = exp_length;
                        }
                        overflow = 1;
                    }
                    else{ // no overflow yet: use strlcat return value
                        exp_length = strlcat(buf,new, bufsize); // add the padded string to the original
                        totallen = exp_length;
                    }
                }
                else{  // no padding needed
                
                    if(bufsize <= (strlen(buf) + strlen(new))){  // buffer check for unpadded character
                        exp_length = strlcat(buf,new, bufsize);
                        if(overflow == 1){
                            totallen += strlen(new);
                        }
                        else{
                            totallen = exp_length;
                        }
                        overflow = 1;
                    }
                    else{  // no overflow yet: use strlcat return value
                        exp_length = strlcat(buf,new, bufsize);
                        totallen = exp_length;
                    }
                }

               
                i++;

            }
            else if(format[i + 1] == 's'){ // string functio
                char* arg = va_arg(args, char*);
                //new = arg;
                char result[fw + 1];         // buffer for padded result 
                int npadding = 0;
                if( fw > strlen(arg)){
                    npadding = fw - strlen(arg); // field width wider than string: left-pad with spaces
                    for( int j = 0; j<npadding +strlen(arg); j++){
                        if(j < npadding){
                            result[j] = ' '; // leading space padding
                        }
                        else{
                            result[j] = arg[j - npadding]; // copy the rest
                        }
                    } 
                   // result[npadding + strlen(arg)] = '\0'; 
                    new = result;
                    if(bufsize <= (strlen(buf) + strlen(new))){  // buffer check for padded string
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
                    else{ // no overflow yet: use strlcat return value
                        exp_length = strlcat(buf,new, bufsize);
                        totallen = exp_length;
                    }
                }
                else{ // no padding needed
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
            else if(format[i + 1] == 'd'){  // %d = signed decimal
                int arg = va_arg(args, int);
                const char* strarg = decimal_string(arg); // convert to decimal string
                char result[fw + 1];         
                int npadding = 0;
                if( fw > strlen(strarg)){   // field width wider than number: left-pad with spaces
                    npadding = fw - strlen(strarg);
                    for( int j = 0; j<npadding +strlen(strarg); j++){
                        if(j < npadding){
                            result[j] = ' ';
                        }
                        else{
                            result[j] = strarg[j - npadding];
                        }
                    } 
                    result[npadding + strlen(strarg)] = '\0'; 
                    new = result;
                    if(bufsize <= (strlen(buf) + strlen(new))){ // buffer check for padded number
                        exp_length = strlcat(buf,new, bufsize);
                        if(overflow == 1){
                            totallen += strlen(new);
                        }
                        else{
                            totallen = exp_length;
                        }
                        overflow = 1;
                    }
                    else{ // no overflow yet: use strlcat return value
                        exp_length = strlcat(buf,new, bufsize);
                        totallen = exp_length;
                    }
                }
                else{ // no padding needed
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
            else if(format[i + 1] == 'x'){ // %x becomes unsigned hex integer
                int arg = va_arg(args, int);

                const char* strarg = hex_string(arg); //converts to hex string
                char result[fw + 1];         // buffer for padding
                int npadding = 0;
                if( fw > strlen(strarg)){ //field width wider than hex: left-pad;
                    npadding = fw - strlen(strarg);
                    for( int j = 0; j<npadding +strlen(strarg); j++){
                        if(j < npadding){
                            result[j] = '0'; // leading zero padding
                        }
                        else{
                            result[j] = strarg[j - npadding];
                        }
                    }  
                    result[npadding + strlen(strarg)] = '\0';
                    new = result;
                    if(bufsize <= (strlen(buf) + strlen(new))){  // 
                        exp_length = strlcat(buf,new, bufsize);
                        if(overflow == 1){
                            totallen += strlen(new);
                        }
                        else{
                            totallen = exp_length;
                        }
                        overflow = 1;
                    }
                    else{  // no overflow yet: use strlcat return value  
                        exp_length = strlcat(buf,new, bufsize); // concatenate original string with padded input
                        totallen = exp_length; // keep track of the length
                    }
                }
                else{ // field width not wider than hex: no padding needed
                    // exp_length = strlcat(buf,strarg, bufsize);
                    //new = strarg;
                    if(bufsize <= (strlen(buf) + strlen(strarg))){ // buffer check for unpadded hex string
                        exp_length = strlcat(buf,strarg, bufsize);
                        if(overflow == 1){
                            totallen += strlen(strarg);
                        }
                        else{
                            totallen = exp_length;
                        }
                        overflow = 1;
                    }
                    else{ // no overflow yet: use strlcat return value
                        exp_length = strlcat(buf,strarg, bufsize);
                        totallen = exp_length;
                    }
                }
                i++;
            }
            else if(format[i + 1] == 'p'){// %p becomes pointer (printed as 0x + zero-padded hex)
                unsigned long arg = va_arg(args, unsigned long);

                const char* strarg = hex_string(arg); // convert to hex
    
                if (fw == 0){
                    fw = 8;  //default is 8 
                }     
                char result[100];
                int npadding = 0;
                result[0] = '0'; // prepend 0x prefix for pointer
                result[1] = 'x';

                if( fw > strlen(strarg)){ // pad hex portion with zeros.
                    npadding = fw - strlen(strarg);
                    for( int j = 2; j<npadding +strlen(strarg) + 2; j++){
                        if(j < npadding + 2){
                            result[j] = '0'; // leading zero padding after 0x
                        }
                        else{
                            result[j] = strarg[j - npadding - 2];
                        }
                    }  
                    result[npadding + strlen(strarg) + 2] = '\0';
                    new = result;
                    if(bufsize <= (strlen(buf) + strlen(new))){ // buffer check for padded pointer string
                        exp_length = strlcat(buf,new, bufsize);
                        if(overflow == 1){
                            totallen += strlen(new);
                        }
                        else{
                            totallen = exp_length;
                        }
                        overflow = 1;
                    }
                    else{ //    
                        exp_length = strlcat(buf,new, bufsize);
                        totallen = exp_length;
                    }
                }
                else{ // no padding needed
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
                // if(exp_length > strlen(buf)){
                //     return exp_length;
                // }
                i++;
            }
            else if(i <strlen(format) - 2){
                if(format[i + 1] == 'l' && format[i+2] == 'd'){ // %ld becomes signed long decimal
                    long arg = va_arg(args, long);
                    const char* strarg = decimal_string(arg);   // convert long to decimal string
                    char result[fw + 1];         
                    int npadding = 0;
                    if( fw > strlen(strarg)){  // field width wider than number: left-pad with spaces
                        npadding = fw - strlen(strarg); // left-pad with spaces to reach field width
                        for( int j = 0; j<npadding +strlen(strarg); j++){
                            if(j < npadding){
                                result[j] = ' '; // leading space padding
                            }
                            else{
                                result[j] = strarg[j - npadding];
                            }
                        }  
                        result[npadding + strlen(strarg)] = '\0';
                        new = result;
                        if(bufsize <= (strlen(buf) + strlen(new))){ // buffer check for padded long decimal string
                            exp_length = strlcat(buf,new, bufsize);
                            if(overflow == 1){
                                totallen += strlen(new);
                            }
                            else{
                                totallen = exp_length;
                            }
                            overflow = 1;
                        }
                        else{ // no overflow yet: use strlcat return value
                            exp_length = strlcat(buf,new, bufsize);
                            totallen = exp_length;
                        }
                    }
                    else{ // no padding needed
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
                        else{  // no overflow yet: use strlcat return value
                            exp_length = strlcat(buf,strarg, bufsize);
                            totallen = exp_length;
                        }
                    }
                    // if(exp_length > strlen(buf)){
                    //     return exp_length;
                    // }
                    
                    i+=2;    // skip past both 'l' and 'd'
                }
                if(format[i + 1] == 'l' && format[i+2] == 'x'){// %lx becomes unsigned long hex
                    unsigned long arg = va_arg(args, unsigned long);
                    const char* strarg = hex_string(arg);    // convert unsigned long to hex string
                    char result[fw + 1];         
                    int npadding = 0;
                    if( fw > strlen(strarg)){
                        npadding = fw - strlen(strarg);  // left-pad with zeros to reach field width
                        for( int j = 0; j<npadding +strlen(strarg); j++){
                            if(j < npadding){
                                result[j] = '0';
                            }
                            else{
                                result[j] = strarg[j - npadding];
                            }
                        }  
                        result[npadding + strlen(strarg)] = '\0';
                        // exp_length = strlcat(buf,result, bufsize); 
                        new = result;
                        if(bufsize <= (strlen(buf) + strlen(new))){ // buffer check for padded long hex string
                            exp_length = strlcat(buf,new, bufsize);
                            if(overflow == 1){
                                totallen += strlen(new);
                            }
                            else{
                                totallen = exp_length;
                            }
                            overflow = 1;
                        }
                        else{ // no overflow yet: use strlcat return value
                            exp_length = strlcat(buf,new, bufsize);
                            totallen = exp_length;
                        }
                    }
                    else{
                        // exp_length = strlcat(buf,strarg, bufsize);
                        // new = strarg;
                        if(bufsize <= (strlen(buf) + strlen(strarg))){ // buffer check for unpadded long hex string
                            exp_length = strlcat(buf,strarg, bufsize);
                            if(overflow == 1){
                                totallen += strlen(strarg);
                            }
                            else{
                                totallen = exp_length;
                            }
                            overflow = 1;
                        }
                        else{ //    no overflow yet: use strlcat return value
                            exp_length = strlcat(buf,strarg, bufsize);
                            totallen = exp_length;
                        }
                    }
                    // totallen+= exp_length;
                    // if(exp_length > strlen(buf)){
                    //     return exp_length;
                    // }
                    i+=2; // skip past both 'l' and 'x'
                }
            }

            else{

                break;   // unrecognized specifier at end of string; stop
            }

        }
        else{ // ordinary (non-%) character: copy literally
            *new = format[i];
            new++;
            *new = '\0';
            new--;


            if(bufsize <= (strlen(buf) + strlen(new))){ // buffer check for ordinary character
                exp_length = strlcat(buf,new, bufsize);
                if(overflow == 1){
                    totallen += strlen(new);
                }
                else{
                    totallen = exp_length;
                }
                overflow = 1;
            }
            else{ //    no overflow yet: use strlcat return value
                exp_length = strlcat(buf,new, bufsize);
                totallen = exp_length;
            }
            
            // if(totallen > strlen(buf)){
            //     return totallen;
            // }
        }

    }
    
    // va_end(args);
    return  totallen; // total chars that would have been written (may exceed bufsize if truncated)


}
/* snprintf: public interface for formatted printing into a sized buffer.
 * Wraps vsnprintf, handling va_list setup and teardown.
 * Returns the total number of characters that would have been written
 * (not counting null terminator), even if truncated by bufsize. */

int snprintf(char *buf, size_t bufsize, const char *format, ...) {
    va_list ap;
    va_start(ap, format);// initialize va_list to read variadic args
    int result = vsnprintf(buf, bufsize, format, ap); // call vsnprintf to implement snprintf, passing the va_list
    va_end(ap);    // clean up va_list
    return result;
   
}

/* printf: formatted print to UART output.
 * Uses a fixed internal buffer of MAX_OUTPUT_LEN bytes.
 * Returns the total number of characters that would have been written. */
int printf(const char *format, ...) {
    const int MAX_OUTPUT_LEN = 1024; // ok to assume printf output never longer than this
  
    char buf[MAX_OUTPUT_LEN];
    int bufsize = MAX_OUTPUT_LEN;

    va_list ap;
    va_start(ap, format);  // initialize va_list to read variadic args
    int result = vsnprintf(buf, bufsize, format, ap); // call vsnprintf to format into internal buffer, passing the va_list
    va_end(ap);
    //uart_putstring(buf); // output the formatted string to UART
    return result;             // clean up va_list

}