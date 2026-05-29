/* File: malloc.c
 * --------------
 * implementedation of malloc and free functions for dynamic memory allocation.
 * Re-work malloc() to add a block header to each block.
 * Implement free() to update status in block header.
 * Implement heap_dump() to output debugging information.
 * Upgrade malloc() to search freed blocks and reuse/split.
 * Upgrade free() to coalesce adjacent free blocks.
 * 
 */


 /*
 * The code given below is simple "bump" allocator from lecture.
 * An allocation request is serviced by using sbrk to extend
 * the heap segment.
 * It does not recycle memory (free is a no-op) so when all the
 * space set aside for the heap is consumed, it will not be able
 * to service any further requests.
 *
 * This code is given here just to show the very simplest of
 * approaches to dynamic allocation. You will replace this code
 * with your own heap allocator implementation.
 */

#include "malloc.h"
#include "memmap.h"
#include "printf.h"
#include <stddef.h> // for NULL
#include "strings.h"

/*
 * Data variables private to this module used to track
 * statistics for debugging/validate heap:
 *    count_allocs, count_frees, total_bytes_requested
 */
static int count_allocs, count_frees, total_bytes_requested;

/*
 * The segment of memory available for the heap runs from HEAP_START
 * to HEAP_MAX. Markers placed in memmap.ld establish these boundaries
 * and the pointer constants are declared in memmap.h
 */

/*
 * The sbrk function is used to extend the size of the in-use heap.
 * It internally tracks the current end of the heap using the pointer
 * cur_heap_end. The pointer is initialized to HEAP_START and is
 * moved upward as the in-use portion of heap segment enlarges.
 * Because cur_head_end is qualified as static, it is not stored
 * as local variable in stack frame, instead it is placed in global data.
 * The one variable is shared and retains its value between calls.
 *
 * The call sbrk(n) bumps up the cur_heap_end pointer, extending the in-use heap
 * by n bytes and returns a pointer to that new space.
 * Note that sbrk(0) is a means to access current end of heap without
 * making changes to it.
 */
void *sbrk(size_t nbytes) {
    static void *cur_heap_end = HEAP_START;      //  IMPORTANT: declared static

    void *new_heap_end = (char *)cur_heap_end + nbytes; // why is typecase required here?
    
    // printf("sbrk current heap end: %p\n", cur_heap_end);
    if (new_heap_end > HEAP_MAX)    // if request would extend beyond heap max
        return NULL;                // reject 
    void *prev_heap_end = cur_heap_end;
    cur_heap_end = new_heap_end;  
    
    return prev_heap_end;
}

// round up size to multiple of n (for malloc, will use to round up n = 8)
// the expression below uses a clever efficient bitwise approach that relies on
// n being a power of two. Trace through the expression to learn how it works!
static inline size_t roundup(size_t val, size_t n) {
    return (val + n-1) & ~(n-1);
}
struct header {
    int payload_size;
    int status;       // 0 if free, 1 if in use
};

void *malloc(size_t nbytes) { // return pointer to newly allocated block of at least nbytes of payload space, or NULL if cannot service request
// * Re-work malloc() to add a block header to each block.
// * Upgrade malloc() to search freed blocks and reuse/split.

    if(nbytes <= 0){
        return NULL;
    }
    void* cur_heap_end = sbrk(0);
    nbytes = roundup(nbytes, 8);

    struct header *tempheader = HEAP_START; //add a block header to each block.

    int leftover = 0;
    int insert = 0;
    // walk the heap by traversing the implicit list of headers
    for(int i = 0; i< count_allocs + count_frees; i++){
        if(tempheader->status == 0){ // if space is free, check if it has a large enough gap
            if(tempheader->payload_size >= nbytes){ // if it has a large enough gap, split remainder into its own free block
                if((void*)((char *)tempheader + nbytes+8) > HEAP_MAX){ // if request would extend beyond heap max, reject
                    return NULL;
                }
                insert = 1;

                struct header *prevheader = tempheader;
                leftover = tempheader->payload_size - nbytes - 8;
                tempheader->payload_size = nbytes;
                tempheader->status = 1;
                // printf("malloc: %p\n payload size: %d\n", cur_heap_end, tempheader->payload_size);
                tempheader =(struct header*)((char*)tempheader + nbytes + 8);
                if(leftover >= 0){  // if there is leftover space, split into its own free block
                    tempheader->payload_size = leftover;
                    tempheader->status = 0;
                }
               
                else{ // if perfect fit, don't do anything
                    count_frees --;
                }
                count_allocs++;
                total_bytes_requested += nbytes;
                
                return prevheader;


            }
            else{
                // if not enough space, skip to next block
                
            }
        }
        else{
            // if space is used, skip to next block
            
        }
        tempheader = (struct header*)((char*)tempheader + tempheader->payload_size + 8);

    }
    if(insert == 0){ // check whether hit the heap end
        tempheader = cur_heap_end;
        if((void*)((char *)tempheader + nbytes+8) > HEAP_MAX){ // if request would extend beyond heap max, reject
            return NULL;
        }
        // if(tempheader + nbytes + 8 < HEAP_MAX){
        count_allocs++;
        total_bytes_requested += nbytes;
        tempheader->payload_size = nbytes;
       // printf("malloc: %p\n payload size: %d\n", cur_heap_end, tempheader->payload_size);
        tempheader->status = 1;
        nbytes = nbytes + 8;
        // cur_heap_end = (char *)cur_heap_end + nbytes;
        // return tempheader;
        return sbrk(nbytes);
    }    
    else{ // if we have already inserted into a free block, but we have not returned yet, then we must have hit the heap end, so we can reject the request
        return NULL;
    }
 

}


// * Implement free() to update status in block header.
// * Upgrade free() to coalesce adjacent free blocks.

void free(void *ptr) {

    if(ptr == NULL){ // legal request, should do nothing
        return;
    }


    volatile struct header *ogblock = ptr;
    struct header *headerptr = ptr;

    
    if (headerptr->status == 1) {
        count_allocs--;
        count_frees++;
    }
    total_bytes_requested -= headerptr->payload_size;
    headerptr->status = 0;
    ogblock->status = 0;
    void* cur_heap_end = sbrk(0);
    //printf("free: %p\n headptr: %p, headerptr->payload_size: %d,  total_bytes_requested: %d\n", cur_heap_end, headerptr, headerptr->payload_size, total_bytes_requested);

    headerptr =(struct header*)((char*)headerptr + headerptr->payload_size + 8);
    while((void*)headerptr < HEAP_MAX && (count_frees > 1) && (void*) headerptr < cur_heap_end){ // walk the heap by traversing the implicit list of headers, coalescing adjacent free blocks
        if(headerptr->status == 0){ // if next block is free, coalesce with current block by adding payload size of next block and header size to current block's payload size, and update count_frees
            ogblock->payload_size = ogblock->payload_size + headerptr->payload_size + 8;
            count_frees--;

            headerptr = (struct header*)((char*)headerptr + headerptr->payload_size + 8);
        }
        else{  // if next block is not free, stop coalescing
            break;
        }
    }
    
}

void heap_dump(const char *label) { //Implement heap_dump() to output debugging information.
    void* cur_heap_end = sbrk(0);
    printf("\n---------- HEAP DUMP (%s) ----------\n", label);
    printf("Heap segment at %p - %p\n", HEAP_START, cur_heap_end);
  
    
    struct header *p = HEAP_START; // walk the heap by traversing the implicit list of headers, printing out information about each block and statistics about the heap
    for(int i = 0; i < count_allocs + count_frees; i++){ // walk the heap by traversing the implicit list of headers, printing out information about each block and statistics about the heap
        printf("Heap segment at %p - %p\n", p, (char*)p + p->payload_size +8);
        printf("address: %p \n payload: %16d \n status:%d\n", p, p->payload_size, p->status);
        p = (struct header*)((char*)p + p->payload_size + 8);
    }
    printf("----------  END DUMP (%s) ----------\n", label);
    printf("Stats: %d in-use (%d allocs, %d frees), %d total payload bytes requested\n\n",
        count_allocs - count_frees, count_allocs, count_frees, total_bytes_requested);
}
