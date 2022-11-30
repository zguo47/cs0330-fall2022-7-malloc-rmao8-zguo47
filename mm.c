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

/*
 * coalescing: helper function that helps to coalesce free blocks by merging   *
 * adjacent smaller free blocks. If the previous or next block of the          *
 * designated is also free, it will be merged with the block.
 *
 * arguments: block_t *block
 * returns: N/A
 */
void coalescing(block_t *block) {
    block_t *next = block_next(block);
    block_t *prev = block_prev(block);
    long new_size;

    pull_free_block(block);

    // if the previous block is free
    if (!block_allocated(prev)) {
        pull_free_block(prev);
        new_size = block_size(prev) + block_size(block);
        block_set_size_and_allocated(prev, new_size, 0);
    }

    // if the next block is free
    if (!block_allocated(next)) {
        // if both are free
        if (!block_allocated(prev)) {
            pull_free_block(next);
            new_size = block_size(next) + block_size(prev);
            block_set_size_and_allocated(prev, new_size, 0);
            insert_free_block(prev);
            return;
        } else {
            // if only next block is free
            pull_free_block(next);
            new_size = block_size(next) + block_size(block);
            block_set_size_and_allocated(block, new_size, 0);
            insert_free_block(block);
            return;
        }
    }

    if (!block_allocated(prev)) {
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
    flist_first = NULL;

    // initiallize and allocate prologue and epilogue
    if ((prologue = mem_sbrk(TAGS_SIZE)) == (void *)-1) {
        perror("prologue error");
        return -1;
    }
    if ((epilogue = mem_sbrk(TAGS_SIZE)) == (void *)-1) {
        perror("epilogue error");
        return -1;
    }

    block_set_size_and_allocated(prologue, TAGS_SIZE, 1);
    block_set_size_and_allocated(epilogue, TAGS_SIZE, 1);
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
    // calculate the size required
    long b_size = align(size) + TAGS_SIZE;

    // if the size is 0, return NULL
    if (size == 0) {
        return NULL;
    }
    // if b_size < MINBLOCKSIZE, make it MINBLOCKSIZE. We could also make it
    // return NULL, but doing so produces lower utility compared to the former
    if (b_size < MINBLOCKSIZE) {
        b_size = MINBLOCKSIZE;
    }

    block_t *curr_block = flist_first;

    // search through the free list to get the first free block with sufficient
    // size
    while (curr_block != NULL) {
        // if we find free block with sufficient space
        if (block_size(curr_block) >= b_size) {
            pull_free_block(curr_block);

            // calculate the extra size
            long free_size = block_size(curr_block) - b_size;

            // if the extra size is at least MINBLOCKSIZE, we could split the
            // rest of the free space into a new free block and insert it into
            // the free list.
            if (free_size >= MINBLOCKSIZE) {
                block_set_size_and_allocated(curr_block, b_size, 1);
                block_set_size_and_allocated(block_next(curr_block), free_size,
                                             0);
                insert_free_block(block_next(curr_block));
            } else {
                // Otherwise, we could ignore the extra size because it is too
                // small
                block_set_allocated(curr_block, 1);
            }
            return curr_block->payload;
        } else {
            // continue to search through the free list
            curr_block = block_flink(curr_block);
            // if we have already traversed the list without finding a suitable
            // free block, we could exit the while loop
            if (curr_block == flist_first) {
                break;
            }
        }
    }
    // if there is no free block with sufficient size, we need to extend the
    // heap to ask for extra free space using mem_sbrk
    block_t *new_block = mem_sbrk(b_size);
    if (new_block == (void *)-1) {
        perror("mem_sbrk error");
        return NULL;
    }
    // update epilogue after extending the heap
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
    // if ptr is NULL, return directly
    if (ptr == NULL) {
        return;
    }
    block_t *block = payload_to_block(ptr);
    // if block is already freed
    if (!block_allocated(block)) {
        return;
    }
    block_set_allocated(block, 0);
    insert_free_block(block);
    // use coalescing helper to increase utility
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
    // if ptr is NULL, call malloc directly
    if (ptr == NULL) {
        mm_malloc(size);
    } else if (size == 0) {
        // if size is 0, free ptr
        mm_free(ptr);
        return NULL;
    } else {
        block_t *original = payload_to_block(ptr);
        long b_size = align(size) + TAGS_SIZE;
        long old_size = block_size(original);
        // if the new size is smaller than the original, return ptr directly. We
        // could implement shortening, however, doing so somehow lower our
        // utility, so we simply return ptr.
        if (old_size >= b_size) {
            // long extra_size = old_size - b_size;
            // if (extra_size >= MINBLOCKSIZE){
            //     block_set_size(original, b_size);
            //     block_set_size_and_allocated(block_next(original), extra_size, 0);
            //     insert_free_block(block_next(original));
            //     coalescing(block_next(original));
            // }
            return ptr;
        } else {
            // if the new size is larger
            block_t *next = block_next(original);
            block_t *prev = block_prev(original);
            // if the previous block is free, and the size of the two blocks
            // together is sufficient, we could combine the two blocks
            if ((!block_allocated(prev)) &&
                (old_size + block_size(prev)) >= b_size) {
                pull_free_block(prev);
                block_set_size_and_allocated(prev, old_size + block_size(prev),
                                             1);
                original = prev;
                memmove(original->payload, ptr, old_size);
                return original->payload;
            }
            // if the next block is free, and the size of the two blocks
            // together is sufficient, we could coombine the two block
            else if ((!block_allocated(next)) &&
                     (old_size + block_size(next)) >= b_size) {
                block_t *next = block_next(original);
                pull_free_block(next);
                block_set_size_and_allocated(original,
                                             old_size + block_size(next), 1);
                return original->payload;
            }
            // if the size is really large that we need to combine both previous
            // and next block (given they are free), we could do so
            else if ((!block_allocated(prev)) && (!block_allocated(next)) &&
                     (old_size + block_size(prev) + block_size(next)) >=
                         b_size) {
                pull_free_block(next);
                pull_free_block(prev);
                block_set_size_and_allocated(
                    prev, old_size + block_size(next) + block_size(prev), 1);

                original = prev;
                memmove(original->payload, ptr, old_size);
                return original->payload;
            }
            // if none of the above are sufficient, we could call malloc, which
            // first search through the free list and then call memsbrk if there
            // is no free block large enough in the list
            else {
                void *newptr = mm_malloc(b_size);
                if (newptr) {
                    memmove(newptr, ptr, old_size);
                    mm_free(ptr);
                }
                return newptr;
            }
        }
    }

    return NULL;
}
