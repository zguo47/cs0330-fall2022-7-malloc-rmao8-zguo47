# 7-malloc
Our strategy for maintaining compaction is to introduce the method coalescing(), 
which given a free block, check to see if the previous block and the next block
are free blocks; if yes, coalesce the free blocks to be one free block of 
combined size. In the method mm_free(), when we free a block, we call 
coalescing() so that if available, the block being freed is automatically 
combined with other free blocks, which saves the heap from having tiny free 
blocks. 

Our mm_realloc() considers several cases. If the given pointer is NULL, we 
directly call mm_malloc(size), if size is equal to 0, we call mm_free(ptr) and 
return NULL. Then, we compare the size of the original block with the given 
size, if the new size is smaller than the original size, we return ptr directly,
and we shorten the memory size to the new size using block_set_size(). Otherwise,
we check the previous block and the next block; if the previous block is not
allocated, and the combined size of the two blocks is larger than the requested
size, we combine the two blocks to a larger block and memmove the original
block to the new address; if the next block is not allocated, we do the same 
thing; and if both the previous and the next block aren't allocated, we also
check the combined size, coalesce and memmove ptr to the new address. In this 
way we can improve the performance of mm_realloc by utilizing the neighboring
free blocks if available, avoid calling mem_sbrk() as possible. If there is
still not enough space, we call mm_malloc with the requested size, memmove ptr
to the new address and then free ptr. 

Except for the implementation of coalescing() and the strategies in mm_realloc(),
we also check if the free block size if larger than the requested size, and if
the extra size is larger than min block size, we set it as a new free block and 
insert it back to the free list. Only when there is no available free block in
the free list, we call mem_sbrk() to request new space in heap. 

Our util score for coalesce is around 99, and realloc is around 80. 

Unresolved bugs: N/A
