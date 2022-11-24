#ifndef MMINLINE_H_
#define MMINLINE_H_
#include "mm.h"
#include <assert.h>

static block_t *flist_first;  // head of circular, doubly linked free list
extern block_t *prologue;
extern block_t *epilogue;

/**
 * In order to pass the tests sequentially, you must implement the inline
 * functions in the order they appear here. Comments between the functions
 * list what tests you should be passing once you've successfully implemented
 * tests up to that point. 
*/

/*
 *
 * |_ | _  _ ;_/     _.|| _  _  _.-+- _  _|
 * [_)|(_)(_ | \____(_]||(_)(_ (_] | (/,(_]
 * 
 * returns 1 if block is allocated, 0 otherwise
 * In other words, returns 1 if the right-most bit in block's size is set, 0
 * otherwise
 */
static inline int block_allocated(block_t *b) { 
    // TODO: Implement this function!
    return b->size & 1;
}

/*
 * |_ | _  _ ;_/     _ ._  _|    -+- _  _
 * [_)|(_)(_ | \____(/,[ )(_]____ | (_](_]
 *                                     ._|
 * returns a pointer to the block's end tag (You probably won't need to use this
 * directly)
 */
static inline long *block_end_tag(block_t *b) {
    assert(b->size >= (WORD_SIZE * 2));

    // TODO: Implement this function!
    return &b->payload[b->size/8 - 2];
}

/*
 * |_ | _  _.;_/     _ ._  _|     _.|| _  _. _.-+- _  _|
 * [_)|(_)(_.| \____(/,[ )(_]____(_]||(_)(_.(_] | (/,(_]
 * 
 * same as the above, but checks the end tag of the block
 * NOTE: since b->size is divided by WORD_SIZE, the 3 right-most bits are 
 * truncated (including the 'is-allocated' bit)
 */
static inline int block_end_allocated(block_t *b) {
    // TODO: Implement this function!
    return block_end_tag(b) & -2 & 1;
}

/*
 * |_ | _  _.;_/     __ _ -+-     __*__. _
 * [_)|(_)(_.| \_____) (/, | _____) | /_(/,
 *
 * Sets the entire size of the block at both the beginning and the end tags
 * Preserves the alloc bit (if b is marked allocated or free, it will remain
 * so).
 * NOTE: size must be a multiple of ALIGNMENT, which means that in binary, its
 * right-most 3 bits must be 0. Thus, we can check if size is a multiple of
 * ALIGNMENT by &-ing it with ALIGNMENT - 1, which is 00..00111 in binary if
 * ALIGNMENT is 8. This is where the assert statement comes from!
 * NOTE: you may want to think about using the |= operator
 */
static inline void block_set_size(block_t *b, long size) {
    assert((size & (ALIGNMENT - 1)) == 0);
    // TODO: Implement this function
    size |= block_allocated(b);
    b->size = size;
    *block_end_tag(b) = size;
}

/*
 * |_ | _  _.;_/    __*__. _
 * [_)|(_)(_.| \____) | /_(/,
 *
 * returns the size of the entire block
 * NOTE: Think about how to use & to remove the is-allocated bit
 * from the size
 */
static inline long block_size(block_t *b) { 
    // TODO: Implement this function!
    return b->size & -2;
}

/**
 * If all functions previous to this line are complete, block_size test will
 * pass
*/

/*
 * |_ | _  _.;_/     __ _ -+-     _.|| _  _. _.-+- _  _|
 * [_)|(_)(_.| \_____) (/, | ____(_]||(_)(_.(_] | (/,(_]
 * 
 * Sets the allocated flags of the block, at both the beginning and the end
 * tags. NOTE: -2 is 111...1110 in binary
 */
static inline void block_set_allocated(block_t *b, int allocated) {
    assert((allocated == 0) || (allocated == 1));
    // TODO: Implement this function
    if (allocated){
        b->size |= 1;
        *block_end_tag(b) |= 1;
    }else{
        b->size &= -2;
        *block_end_tag(b) &= -2;
    }
}

/**
 * If all functions previous to this line are complete, block_allocated test will
 * pass
*/

/*
 * |_ | _  _.;_/   __ _ -+-   __*__. _    _.._  _|   _.|| _  _. _.-+- _  _|
 * [_)|(_)(_.| \___) (/, | ___) | /_(/,__(_][ )(_]__(_]||(_)(_.(_] | (/,(_]
 *
 * Sets the entire size of the block and sets the allocated flags of the block,
 * at both the beginning and the end
 */
static inline void block_set_size_and_allocated(block_t *b, long size,
                                                int allocated) {
    // TODO: Implement this function
    
}

/**
 * If all functions previous to this line are complete, block_size_and_allocated
 * test will pass
*/

/*
 * |_ | _  _.;_/     _ ._  _|     __*__. _
 * [_)|(_)(_.| \____(/,[ )(_]_____) | /_(/,
 * 
 * same as the above, but uses the end tag of the block
 */
static inline long block_end_size(block_t *b) {
    // TODO: Implement this function!
}

/**
 * If all functions previous to this line are complete, end_tag
 * test will pass
*/

/* 
 * |_ | _  _.;_/    _ ._. _ .  ,   __*__. _
 * [_)|(_)(_.| \___[_)[  (/, \/ ___) | /_(/,
 *                  |
 * 
 * returns the size of the previous block. NOTE: -2 is 111...1110 in binary
 */
static inline long block_prev_size(block_t *b) {
    // TODO: Implement this function
}

/*
 * |_ | _  _.;_/    ._ ._. _ .  ,
 * [_)|(_)(_.| \____[_)[  (/, \/
 *                  |
 *  
 * returns a pointer to the previous block
 */
static inline block_t *block_prev(block_t *b) {
    // TODO: Implement this function
}

/**
 * If all functions previous to this line are complete, block_prev
 * test will pass
*/

/*
 * |_ | _  _.;_/    ._  _ \./-+-
 * [_)|(_)(_.| \____[ )(/,/'\ |
 *  
 * returns a pointer to the next block
 */
static inline block_t *block_next(block_t *b) {
    // TODO: Implement this function
}

/**
 * If all functions previous to this line are complete, block_next
 * test will pass
*/

/*
 * ._  _.  .| _  _. _|    -+- _     |_ | _  _.;_/
 * [_)(_]\_||(_)(_](_]____ | (_)____[_)|(_)(_.| \
 * |     ._|
 * 
 * given a pointer to the payload, returns a pointer to the block
 * NOTE: think about how casting might be useful
 */
static inline block_t *payload_to_block(void *payload) {
    // TODO: Implement this function
}

/**
 * If all functions previous to this line are complete, payload_to_block
 * test will pass
*/

/*
 * |_ | _  _.;_/    ._  _ \./-+-     _.|| _  _. _.-+- _  _|
 * [_)|(_)(_.| \____[ )(/,/'\ | ____(_]||(_)(_.(_] | (/,(_]
 * 
 * returns 1 if the next block is allocated; 0 if not
 */
static inline int block_next_allocated(block_t *b) {
    // TODO: Implement this function
}

/*
 * |_ | _  _.;_/    ._  _ \./-+-     __*__. _
 * [_)|(_)(_.| \____[ )(/,/'\ | _____) | /_(/,
 * 
 * returns the size of the next block
 */
static inline long block_next_size(block_t *b) {
    // TODO: Implement this function
}

/**
 * If all functions previous to this line are complete, next_size_and_allocated
 * test will pass
*/

/* 
 * |_ | _  _.;_/    ._ ._. _ .  ,     _.|| _  _. _.-+- _  _|
 * [_)|(_)(_.| \____[_)[  (/, \/ ____(_]||(_)(_.(_] | (/,(_]
 *                  |
 *
 * returns 1 if the previous block is allocated, 0 otherwise
 */
static inline int block_prev_allocated(block_t *b) {
    // TODO: Implement this function
}

/**
 * If all functions previous to this line are complete, prev_size_and_allocated
 * test will pass
*/

/*
 * |_ | _  _.;_/    |_ |*._ ;_/
 * [_)|(_)(_.| \____[_)||[ )| \
 *
 * given the input block 'b', returns b's blink, which points to the
 * previous block in the free list. NOTE: if 'b' is free, b->payload[1]
 * contains b's blink
 */
static inline block_t *block_blink(block_t *b) {
    assert(!block_allocated(b));
    // TODO: Implement this function
}

/*
 * |_ | _  _.;_/     __ _ -+-    |_ |*._ ;_/
 * [_)|(_)(_.| \_____) (/, | ____[_)||[ )| \
 *
 * given the inputs 'b' and 'new_blink', sets b's blink to now point
 * to new_blink, which should be the previous block in the free list
 */
static inline void block_set_blink(block_t *b, block_t *new_blink) {
    assert(!block_allocated(b) && !block_allocated(new_blink));
    // TODO: Implement this function
}

/**
 * If all functions previous to this line are complete, set_blink
 * test will pass
*/

/*
 * |_ | _  _.;_/    |,|*._ ;_/
 * [_)|(_)(_.| \____| ||[ )| \
 * 
 * given the input block 'b', returns b's flink, which points to the next block
 * in the free list.NOTE: if 'b' is free, b->payload[0] contains b's flink
 */
static inline block_t *block_flink(block_t *b) {
    assert(!block_allocated(b));
    // TODO: Implement this function
}

/*
 * |_ | _  _.;_/    __ _ -+-    |,|*._ ;_/
 * [_)|(_)(_.| \____) (/, | ____| ||[ )| \
 * 
 * given the inputs 'b' and 'new_flink', sets b's flink to now point
 * to new_flink, which should be the next block in the free list
 */

static inline void block_set_flink(block_t *b, block_t *new_flink) {
    assert(!block_allocated(b) && !block_allocated(new_flink));
    // TODO: Implement this function
}

/**
 * If all functions previous to this line are complete, set_flink
 * test will pass
*/

/*
 *
 * . _  __ _ ._.-+-    |,._. _  _     |_ | _  _.;_/
 * |[ )_) (/,[   | ____| [  (/,(/,____[_)|(_)(_.| \
 * 
 * insert block into the (circularly doubly linked) free list
 * If the list is not empty, block should be inserted between
 * flist_first and the last block in the list. flist_first 
 * should always be set equal to the new block.
 */
static inline void insert_free_block(block_t *fb) {
    assert(!block_allocated(fb));
    // TODO: Implement this function
}

/**
 * If all functions previous to this line are complete, insert_free_block
 * test will pass
*/

/*
 * ._ . .||    |,._. _  _     |_ | _  _.;_/
 * [_)(_|||____| [  (/,(/,____[_)|(_)(_.| \
 * |
 * 
 * pull a block from the (circularly doubly linked) free list
 */
static inline void pull_free_block(block_t *fb) {
    assert(!block_allocated(fb));
    // TODO: Implement this function
}

/**
 * If all functions previous to this line are complete, ALL TESTS will pass
*/

#endif  // MMINLINE_H_
