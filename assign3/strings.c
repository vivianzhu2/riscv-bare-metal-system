/* File: strings.c
 * ---------------
 * ***** TODO: add your file header comment here *****
 */
#include "strings.h"

void *memcpy(void *dst, const void *src, size_t n) {
    /* Copy contents from src to dst one byte at a time */
    char *d = dst;
    const char *s = src;
    while (n--) {
        *d++ = *s++;
    }
    return dst;
}

void *memset(void *dst, int val, size_t n) {
    /***** TODO: Your code goes here *****/
    char *d = dst;
    const char *v = (const char *) &val; // cast val to char pointer to get the byte value of val
    while(n--) {
        *d++ = *v;
    }
    return dst;
}

size_t strlen(const char *str) {
    /* Implementation a gift to you from lab3 */
    size_t n = 0;
    while (str[n] != '\0') {
        n++;
    }
    return n;
}

int strcmp(const char *s1, const char *s2) {
    int length;
    if(strlen(s1)>strlen(s2)){ // compare the length of two strings, and use the longer one as the length of the loop
        length = strlen(s1);
    } else {
        length = strlen(s2);
    }
    for(int i = 0; i < length ; i++){
        if(s1[i] == '\0' && s2[i] == '\0'){ // if both strings end at the same time, they are equal
            return 0;
        }
        else if(s1[i] == '\0'){ // if s1 ends but s2 does not, then s1 is smaller than s2
            return -1;
        }
        else if(s2[i] == '\0'){ // if s2 ends but s1 does not, then s1 is greater than s2
            return 1;
        }
        if(s1[i] > s2[i]){ // if the characters at the current position are greater in s1 than in s2, then s1 is greater than s2
            return 1;
        }
        else if(s1[i] < s2[i]){ // if the characters at the current position are smaller in s1 than in s2, then s1 is smaller than s2
            return -1;
        }
    }
    return 0;
}

size_t strlcat(char *dst, const char *src, size_t dstsize) {
    /***** TODO: Your code goes here *****/
    int strlen_dst = strlen(dst);
    int strlen_src = strlen(src);
    char *d = dst;

    const char *s = src;
    int ifnull = (*d == '\0');
    for(int i=0; i < dstsize; i++){
        if(*d == '\0'){ 
            ifnull = 1;
            break;
        }
        d++;
    }
    if(ifnull == 0){
        return dstsize + strlen(src); 
    }

    for(int i = strlen_dst; i< dstsize ; i++){
        if(i == dstsize - 1){
            *d = '\0';
            return (strlen_dst + strlen_src);
            //break;
        }
        *d = *s;
       // *d++ = *s++; // copy the character from src to dst
        if(*s == '\0'){ // if we reach the end of src, we can stop copying
            return (strlen_dst + strlen_src);
        }
        d++;
        s++;
    }
    
   // *d ++ = '\0'; 
    return strlen_dst + strlen_src; // return the total length of the string we tried to create


}

unsigned long strtonum(const char *str, const char **endptr) {
    /***** TODO: Your code goes here *****/
    const char *s = str;
    
    if(! (*s  >= '0' && *s <= '9')){
        if((s != NULL) && (endptr!=NULL)){
            *endptr = s; // set endptr to point to the first non-digit character
        }
        return 0;
    }
    unsigned long result = 0;
    if(*s == '0'){
        s++;
            if(*s == 'x' || *s == 'X'){ // if the string starts with "0x" or "0X", then it is a hexadecimal number and we need to convert it accordingly
                s++;
                while((*s!= '\0') && ((*s>='0' && *s <= '9') || (*s >= 'a' && *s <= 'f') || (*s >= 'A' && *s<= 'F'))){
                    if(*s>='0' && *s <= '9'){
                        result = (unsigned long)(*s- '0') + result * 16;
                    }
                    else if(*s >= 'a' && *s <= 'f'){
                        result = (unsigned long)(*s - 'a') + 10 + result * 16;
                    }
                    else if(*s >= 'A' && *s <= 'F'){
                        result = (unsigned long)(*s - 'A') + 10 + result * 16;
                    }
                    s++;
                }
                //*endptr = s;
            }  
            else if (*s!='\0' && *s >= '0' && *s<='9') {
                
            }
            else{
                if((s != NULL) && (endptr!=NULL)){
                    *endptr = s; // set endptr to point to the first non-digit character
                }
                return 0;
            }
    }
   
    while(*s!='\0' && *s >= '0' && *s<='9'){ // keep converting characters to numbers until we reach a non-digit character
    
        result = (unsigned long)(*s-'0') + result * 10; // convert the character to a number and add it to the result
        s++; 
    }
    
    if((s != NULL) && (endptr!=NULL)){
        *endptr = s; // set endptr to point to the first non-digit character
    }
    return result;


}
