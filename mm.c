#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 * BEFORE GETTING STARTED:
 *
 * Familiarize yourself with the functions and constants/variables
 * in the following included files.
 * This will make the project a LOT easier as you go!!
 *
 * The diagram in Section 4.1 (Specification) of the handout will help you
 * understand the constants in mm.h
 * Section 4.2 (Support Routines) of the handout has information about
 * the functions in mminline.h and memlib.h
 */
#include "./memlib.h"
#include "./mm.h"
#include "./mminline.h"

block_t *prologue;
block_t *epilogue;

// rounds up to the nearest multiple of WORD_SIZE
static inline long align(long size) {
    return (((size) + (WORD_SIZE - 1)) & ~(WORD_SIZE - 1));
}

void coalescing(block_t *block) {
    
    block_t *next = block_next(block);
    block_t *prev = block_prev(block);
    long new_size;
    
    int prev_block_free = 0;
    pull_free_block(block);

    if (!block_allocated(prev)) {
        
        pull_free_block(prev);
        new_size = block_size(prev) + block_size(block);
        block_set_size_and_allocated(prev, new_size, 0);
        prev_block_free = 1;
    }
    if (!block_allocated(next)) {
        
        if (prev_block_free == 1) {
            pull_free_block(next);
            new_size = block_size(next) + block_size(prev);
            
            block_set_size_and_allocated(prev, new_size, 0);
            insert_free_block(prev);
            return;

        } else {
            pull_free_block(next);
            new_size = block_size(next) + block_size(block);
            block_set_size_and_allocated(block, new_size, 0);
            insert_free_block(block);
            return;
        }
    }
    
    if (prev_block_free == 1) {
        insert_free_block(prev);
    } else {
        insert_free_block(block);
    }
}


/*
 *                             _       _ _
 *     _ __ ___  _ __ ___     (_)_ __ (_) |_
 *    | '_ ` _ \| '_ ` _ \    | | '_ \| | __|
 *    | | | | | | | | | | |   | | | | | | |_
 *    |_| |_| |_|_| |_| |_|___|_|_| |_|_|\__|
 *                       |_____|
 *
 * initializes the dynamic storage allocator (allocate initial heap space)
 * arguments: none
 * returns: 0, if successful
 *         -1, if an error occurs
 */
int mm_init(void) {
    
    if ((prologue = mem_sbrk(TAGS_SIZE)) == (void *) -1){
        perror("prologue error");
        return -1;
    }
    if ((epilogue = mem_sbrk(TAGS_SIZE)) == (void *) -1){
        perror("epilogue error");
        return -1;
    }

    block_set_size_and_allocated(prologue, TAGS_SIZE, 1);
    block_set_size_and_allocated(epilogue, TAGS_SIZE, 1);

    flist_first = NULL;
    return 0;
}

/*     _ __ ___  _ __ ___      _ __ ___   __ _| | | ___   ___
 *    | '_ ` _ \| '_ ` _ \    | '_ ` _ \ / _` | | |/ _ \ / __|
 *    | | | | | | | | | | |   | | | | | | (_| | | | (_) | (__
 *    |_| |_| |_|_| |_| |_|___|_| |_| |_|\__,_|_|_|\___/ \___|
 *                       |_____|
 *
 * allocates a block of memory and returns a pointer to that block's payload
 * arguments: size: the desired payload size for the block
 * returns: a pointer to the newly-allocated block's payload (whose size
 *          is a multiple of ALIGNMENT), or NULL if an error occurred
 */
void *mm_malloc(long size) {
    // TODO
    long b_size = align(size) + TAGS_SIZE;
    if (size == 0 || b_size < MINBLOCKSIZE){
        return NULL;
    }
    block_t *curr_block = flist_first;

    while (curr_block != NULL){
        if (block_size(curr_block) >= b_size){
            pull_free_block(curr_block);

            long free_size = block_size(curr_block) - b_size;

            if (free_size >= MINBLOCKSIZE) {
                block_set_size_and_allocated(curr_block, b_size, 1);
                block_set_size_and_allocated(block_next(curr_block), free_size, 0);

                insert_free_block(block_next(curr_block));
            } else {
                block_set_allocated(curr_block, 1);
            }
            return curr_block->payload;   

        }else{
            curr_block = block_next(curr_block);
            if (curr_block = flist_first) {
                break;
            }
        }
    }
    block_t *new_block = mem_sbrk(b_size);
    if (new_block == (void *)-1) {
        perror("mem_sbrk error");
        return NULL;
    }
    new_block = epilogue;
    block_set_size_and_allocated(new_block, b_size, 1);
    epilogue = block_next(new_block);
    block_set_size_and_allocated(epilogue, TAGS_SIZE, 1);
    return new_block->payload;
}

/*                              __
 *     _ __ ___  _ __ ___      / _|_ __ ___  ___
 *    | '_ ` _ \| '_ ` _ \    | |_| '__/ _ \/ _ \
 *    | | | | | | | | | | |   |  _| | |  __/  __/
 *    |_| |_| |_|_| |_| |_|___|_| |_|  \___|\___|
 *                       |_____|
 *
 * frees a block of memory, enabling it to be reused later
 * arguments: ptr: pointer to the block's payload
 * returns: nothing
 */
void mm_free(void *ptr) {
    // TODO
    if (ptr == NULL) {
        return;
    }
    block_t *block = payload_to_block(ptr);
    block_set_allocated(block, 0);
    insert_free_block(block);
    coalescing(block);
    return;
}

/*
 *                                            _ _
 *     _ __ ___  _ __ ___      _ __ ___  __ _| | | ___   ___
 *    | '_ ` _ \| '_ ` _ \    | '__/ _ \/ _` | | |/ _ \ / __|
 *    | | | | | | | | | | |   | | |  __/ (_| | | | (_) | (__
 *    |_| |_| |_|_| |_| |_|___|_|  \___|\__,_|_|_|\___/ \___|
 *                       |_____|
 *
 * reallocates a memory block to update it with a new given size
 * arguments: ptr: a pointer to the memory block's payload
 *            size: the desired new payload size
 * returns: a pointer to the new memory block's payload
 */
void *mm_realloc(void *ptr, long size) {
    // TODO
    if (ptr == NULL){
        mm_malloc(size);
    }else if (size == 0){
        mm_free(ptr);
        return NULL;
    }else{
        block_t *original = payload_to_block(ptr);
        long b_size = align(size) + TAGS_SIZE;
        long old_size = block_size(original);
        if (old_size >= b_size){
            block_set_size(original, b_size);
            return ptr;
        }else{
            // block *curr_block = original;
            // while (block_next(curr_block) != NULL && block_next_allocated(curr_block) != 0){
            //     old_size += block_next_size(curr_block);
            // }
            // if (old_size >= b_size){

            // }else{

            // }
            void *newptr = mm_malloc(size);
            if (newptr){
                memcpy(newptr, ptr, old_size);
                mm_free(ptr);
            }
            return newptr;
        }
    }

    return NULL;

}
