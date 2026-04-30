/* File: malloc.c
 * --------------
 * ***** TODO: add your file header comment here *****
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

void *malloc(size_t nbytes) {
    /***** TODO: Replace with your code *****/
    count_allocs++;
    total_bytes_requested += nbytes;
    nbytes = roundup(nbytes, 8);
    return sbrk(nbytes);
}

void free(void *ptr) {
    count_frees++;
    /***** TODO: Your code goes here *****/
}

void heap_dump(const char *label) {
    void *cur_heap_end = sbrk(0);
    printf("\n---------- HEAP DUMP (%s) ----------\n", label);
    printf("Heap segment at %p - %p\n", HEAP_START, cur_heap_end);

    /***** TODO: Your code goes here *****/

    printf("----------  END DUMP (%s) ----------\n", label);
    printf("Stats: %d in-use (%d allocs, %d frees), %d total payload bytes requested\n\n",
        count_allocs - count_frees, count_allocs, count_frees, total_bytes_requested);
}
