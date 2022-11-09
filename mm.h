#ifndef MM_H_
#define MM_H_

#include <stdio.h>

int mm_init(void);
void *mm_malloc(long size);
void mm_free(void *ptr);
void *mm_realloc(void *ptr, long size);

// Defines alignment to 8 bytes.
#define ALIGNMENT 8
// Size of a memory address, which in this case is 8 bytes
// in a 64-bit system.
#define WORD_SIZE (long)(sizeof(long))  //returns an unsigned, which is why we cast
// Sum of the sizes of the beginning and end tags of a block.
// (Each tag's size is WORD_SIZE)
#define TAGS_SIZE (long)(2 * WORD_SIZE)
// Minimum size of a block. Your implementation should make
// sure no allocated or free block has a size of less than
// this constant.
#define MINBLOCKSIZE (long)(4 * WORD_SIZE)

typedef struct block {
    long size;
    // size is assumed to be a multiple of 8. The least-significant bit is
    // overloaded:
    //     if 0 the block is free
    //     if 1 the block is allocated
    long payload[];
    // the actual size of payload is given in the size field
    // for free blocks:
    //     payload[0] is the block's flink (the next block in the free list)
    //     payload[1] is the block's blink (the previous block in the free list)
    // there is a copy of the size field at the end of the block
} block_t;

#endif  // MM_H_
