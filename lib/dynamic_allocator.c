/*
 * dynamic_allocator.c
 *
 *  Created on: Sep 21, 2023
 *      Author: HP
 */

#include <inc/assert.h>
#include <inc/string.h>
#include "../inc/dynamic_allocator.h"

//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//

//=====================================================
// 1) GET BLOCK SIZE (including size of its meta data):
//=====================================================
__inline__ uint32 get_block_size(void* va) {
	uint32 *curBlkMetaData = ((uint32 *) va - 1);
	return (*curBlkMetaData) & ~(0x1);
}

//===========================
// 2) GET BLOCK STATUS:
//===========================
__inline__ int8 is_free_block(void* va) {
	uint32 *curBlkMetaData = ((uint32 *) va - 1);
	return (~(*curBlkMetaData) & 0x1);
}

//===========================
// 3) ALLOCATE BLOCK:
//===========================

void *alloc_block(uint32 size, int ALLOC_STRATEGY) {
	void *va = NULL;
	switch (ALLOC_STRATEGY) {
	case DA_FF:
		va = alloc_block_FF(size);
		break;
	case DA_NF:
		va = alloc_block_NF(size);
		break;
	case DA_BF:
		va = alloc_block_BF(size);
		break;
	case DA_WF:
		va = alloc_block_WF(size);
		break;
	default:
		cprintf("Invalid allocation strategy\n");
		break;
	}
	return va;
}

//===========================
// 4) PRINT BLOCKS LIST:
//===========================

void print_blocks_list(struct MemBlock_LIST list) {
	cprintf("=========================================\n");
	struct BlockElement* blk;
	cprintf("\nDynAlloc Blocks List:\n");
	LIST_FOREACH(blk, &list)
	{
		cprintf("(size: %d, isFree: %d)\n", get_block_size(blk),
				is_free_block(blk));
	}
	cprintf("=========================================\n");

}
//
////********************************************************************************//
////********************************************************************************//

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

bool is_initialized = 0;
//==================================
// [1] INITIALIZE DYNAMIC ALLOCATOR:
//==================================
/*uint32*  end_block;*/
void initialize_dynamic_allocator(uint32 daStart,
		uint32 initSizeOfAllocatedSpace) {
	//==================================================================================
	//DON'T CHANGE THESE LINES==========================================================
	//==================================================================================
	{
		if (initSizeOfAllocatedSpace % 2 != 0)
			initSizeOfAllocatedSpace++; //ensure it's multiple of 2
		if (initSizeOfAllocatedSpace == 0)
			return;
		is_initialized = 1;
	}
	//==================================================================================
	//==================================================================================

	//TODO: [PROJECT'24.MS1 - #04] [3] DYNAMIC ALLOCATOR - initialize_dynamic_allocator
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("initialize_dynamic_allocator is not implemented yet");
	//Your Code is Here...
	uint32* begin_block = (uint32*) daStart;
	*begin_block = 1;

	uint32* end_block = (uint32*) (daStart + initSizeOfAllocatedSpace
			- sizeof(int));
	*end_block = 1;

	uint32* first_block_header = (uint32*) (daStart + sizeof(int));
	*first_block_header = initSizeOfAllocatedSpace - (2 * sizeof(int));

	uint32* first_block_footer = (uint32*) (daStart + initSizeOfAllocatedSpace
			- (2 * sizeof(int)));
	*first_block_footer = initSizeOfAllocatedSpace - (2 * sizeof(int));

	LIST_INIT(&freeBlocksList);
	struct BlockElement* first_free_block = (struct BlockElement*) (daStart
			+ (2 * sizeof(int)));
	LIST_INSERT_HEAD(&freeBlocksList, first_free_block);
}
//==================================
// [2] SET BLOCK HEADER & FOOTER:
//==================================
void set_block_data(void* va, uint32 totalSize, bool isAllocated) {
	//TODO: [PROJECT'24.MS1 - #05] [3] DYNAMIC ALLOCATOR - set_block_data
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("set_block_data is not implemented yet");
	//Your Code is Here...
	uint32* block_header = va - sizeof(int);
	*block_header = totalSize | isAllocated;

	uint32* block_footer = (uint32*) (va + totalSize - 2 * sizeof(int));
	*block_footer = totalSize | isAllocated;

	if (isAllocated == 0) {
		uint32* free_block = (uint32*) va;
		bool assigned_free_block = 0;

		struct BlockElement* iterator;
		LIST_FOREACH(iterator, &freeBlocksList)
		{
			if ((int) free_block < (int) iterator) {
				LIST_INSERT_BEFORE(&freeBlocksList, iterator,
						(struct BlockElement* )free_block);
				assigned_free_block = 1;
				return;
			}

			if ((int) free_block > (int) iterator
					&& (int) free_block
							< (int) (iterator->prev_next_info.le_next)) {
				LIST_INSERT_AFTER(&freeBlocksList, iterator,
						(struct BlockElement* )free_block);
				assigned_free_block = 1;
				return;
			}
		}

		LIST_INSERT_TAIL(&freeBlocksList, (struct BlockElement* )free_block);
	} else {
		struct BlockElement* iterator;
		LIST_FOREACH(iterator, &freeBlocksList)
		{
			if ((int) iterator == (int) va) {
				LIST_REMOVE(&freeBlocksList,
						(struct BlockElement* )((uint32 )va));
				break;
			}
		}
	}
}

//=========================================
// [3] ALLOCATE BLOCK BY FIRST FIT:
//=========================================
void *alloc_block_FF(uint32 size) {
	//==================================================================================
	//DON'T CHANGE THESE LINES==========================================================
	//==================================================================================
	{
		if (size % 2 != 0)
			size++;	//ensure that the size is even (to use LSB as allocation flag)
		if (size < DYN_ALLOC_MIN_BLOCK_SIZE)
			size = DYN_ALLOC_MIN_BLOCK_SIZE;
		if (!is_initialized) {
			uint32 required_size = size + 2 * sizeof(int) /*header & footer*/
			+ 2 * sizeof(int) /*da begin & end*/;
			uint32 da_start = (uint32) sbrk(
			ROUNDUP(required_size, PAGE_SIZE) / PAGE_SIZE);
			uint32 da_break = (uint32) sbrk(0);
			initialize_dynamic_allocator(da_start, da_break - da_start);
		}
	}
	//==================================================================================
	//==================================================================================

	//TODO: [PROJECT'24.MS1 - #06] [3] DYNAMIC ALLOCATOR - alloc_block_FF
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("alloc_block_FF is not implemented yet");
	//Your Code is Here...
	if (size == 0 || size < 8) {
		return NULL;
	}

	uint32 allocated_block_size = size + 2 * sizeof(int) /*header & footer*/;
	struct BlockElement *element;

	LIST_FOREACH(element, &freeBlocksList)
	{
		uint32 block_size = get_block_size(element);

		if (block_size
				>= allocated_block_size + (2 * DYN_ALLOC_MIN_BLOCK_SIZE)) {
			// SPLIT THE BLOCK INTO TWO
			set_block_data(element, allocated_block_size, 1);
			set_block_data((void*) ((int) element + allocated_block_size),
					block_size - allocated_block_size, 0);

			return element;
		}

		if (allocated_block_size <= block_size) {
			// ALLOCATE BLOCK
			set_block_data(element, block_size, 1);
			return element;
		}
	}

		// NO FREE BLOCK FOUND FOR THE PROVIDED SIZE
	int numofpagesNeeded=ROUNDUP(allocated_block_size, PAGE_SIZE) / PAGE_SIZE;
	uint32* new_mem = sbrk(numofpagesNeeded )-sizeof(int);
	if(new_mem==(void *)-1)
	{
		return NULL;
	}
	 /*end_block = (uint32 *)((char *)(end_block) + numofpagesNeeded*PAGE_SIZE);
    	*end_block = 1;*/
	new_mem = (uint32 *)((char *)(new_mem) + numofpagesNeeded*PAGE_SIZE);
	    *new_mem = 1;
     uint32* last_block_footer = (uint32 *)((uint32)new_mem - numofpagesNeeded * PAGE_SIZE-  sizeof(int));
     uint32 size_of_block=*((uint32*)last_block_footer ) & ~1;
     struct BlockElement* old_last_block=(struct BlockElement*)((uint32)new_mem- (numofpagesNeeded * PAGE_SIZE)-(size_of_block)+ sizeof(int));
     if (is_free_block(old_last_block)) {
    	     uint32 old_size = get_block_size(old_last_block);
    	        uint32 new_size =old_size + numofpagesNeeded * PAGE_SIZE ;
    	        LIST_REMOVE(&freeBlocksList, old_last_block);
    	        set_block_data(old_last_block, new_size, 0);
    	        return alloc_block_FF(size);
    	    }
     else {

    	        uint32* new_block = (uint32*)((char*)new_mem - numofpagesNeeded * PAGE_SIZE+sizeof(int));
    	        set_block_data(new_block, numofpagesNeeded * PAGE_SIZE  , 0);
    	        return alloc_block_FF(size);
    	    }

}

//=========================================
// [4] ALLOCATE BLOCK BY BEST FIT:
//=========================================
void *alloc_block_BF(uint32 size) {
	//TODO: [PROJECT'24.MS1 - BONUS] [3] DYNAMIC ALLOCATOR - alloc_block_BF
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("alloc_block_BF is not implemented yet");
	//Your Code is Here...
	{
		if (size % 2 != 0)
			size++;	//ensure that the size is even (to use LSB as allocation flag)
		if (size < DYN_ALLOC_MIN_BLOCK_SIZE)
			size = DYN_ALLOC_MIN_BLOCK_SIZE;
		if (!is_initialized) {
			uint32 required_size = size + 2 * sizeof(int) /*header & footer*/
			+ 2 * sizeof(int) /*da begin & end*/;
			uint32 da_start = (uint32) sbrk(
			ROUNDUP(required_size, PAGE_SIZE) / PAGE_SIZE);
			uint32 da_break = (uint32) sbrk(0);
			initialize_dynamic_allocator(da_start, da_break - da_start);
		}
	}

	if (size == 0 || size < 8) {
		return NULL;
	}

	uint32 allocated_block_size = size + 2 * sizeof(int) /*header & footer*/;

	struct BlockElement *best_fit;
	int best_size = 2147483647 /* INT_MAX */;

	struct BlockElement *element;
	LIST_FOREACH(element, &freeBlocksList)
	{
		uint32 block_size = get_block_size(element);

		if (block_size >= allocated_block_size) {
			if (block_size < best_size) {
				best_size = block_size;
				best_fit = element;
			}
		}
	}

	if (best_size != 2147483647) {
		if (best_size
				>= allocated_block_size + (2 * DYN_ALLOC_MIN_BLOCK_SIZE)) {
			// SPLIT THE BLOCK INTO TWO
			void* result = (void*) best_fit;

			set_block_data(best_fit, allocated_block_size, 1);
			set_block_data((void*) ((int) best_fit + allocated_block_size),
					best_size - allocated_block_size, 0);

			return result;
		}

		if (best_fit != NULL) {
			if (allocated_block_size <= best_size) {
				// ALLOCATE BLOCK
				set_block_data(best_fit, best_size, 1);
				return (void*) best_fit;
			}
		}
	}

	// NO FREE BLOCK FOUND FOR THE PROVIDED SIZE
	int numofpagesNeeded=ROUNDUP(allocated_block_size, PAGE_SIZE) / PAGE_SIZE;
		uint32* new_mem = sbrk(numofpagesNeeded )-sizeof(int);
		if(new_mem==(void *)-1)
		{
			return NULL;
		}
		/* end_block = (uint32 *)((char *)(end_block) + numofpagesNeeded*PAGE_SIZE);
	    	*end_block = 1;*/
		new_mem = (uint32 *)((char *)(new_mem) + numofpagesNeeded*PAGE_SIZE);
			    *new_mem = 1;
	     uint32* last_block_footer = (uint32 *)((uint32)new_mem - numofpagesNeeded * PAGE_SIZE-  sizeof(int));
	     uint32 size_of_block=*((uint32*)last_block_footer ) & ~1;
	     struct BlockElement* old_last_block=(struct BlockElement*)((uint32)new_mem- (numofpagesNeeded * PAGE_SIZE)-(size_of_block)+ sizeof(int));
	     if (is_free_block(old_last_block)) {
	    	     uint32 old_size = get_block_size(old_last_block);
	    	        uint32 new_size =old_size + numofpagesNeeded * PAGE_SIZE ;
	    	        LIST_REMOVE(&freeBlocksList, old_last_block);
	    	        set_block_data(old_last_block, new_size, 0);
	    	        return alloc_block_BF(size);
	    	    }
	     else {

	    	        uint32* new_block = (uint32*)((char*)new_mem - numofpagesNeeded * PAGE_SIZE+sizeof(int));
	    	        set_block_data(new_block, numofpagesNeeded * PAGE_SIZE  , 0);
	    	        return alloc_block_BF(size);
	    	    }


	/*sbrk(0);
	return NULL;*/
}

//===================================================
// [5] FREE BLOCK WITH COALESCING:
//===================================================
void free_block(void *va) {
	//TODO: [PROJECT'24.MS1 - #07] [3] DYNAMIC ALLOCATOR - free_block
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("free_block is not implemented yet");
	//Your Code is Here...
	if (va == NULL) {
		return;
	}

	// FREE THE DESIRED BLOCK
	uint32 free_block_size = get_block_size(va);
	uint32 total_merged_size = free_block_size;

	struct BlockElement *merged_block_va = NULL;

	struct BlockElement *element;
	LIST_FOREACH(element, &freeBlocksList)
	{
		uint32 current_block_size = get_block_size(element);

		// BLOCK BEFORE THE DESIRED ONE IS FREE
		if ((void*) ((int) element + current_block_size) == va) {
			total_merged_size += current_block_size;

			LIST_REMOVE(&freeBlocksList, (struct BlockElement* ) element);

			merged_block_va = element;
		}

		// BLOCK AFTER THE DESIRED ONE IS FREE
		if ((void*) ((int) element - free_block_size) == va) {
			total_merged_size += current_block_size;

			LIST_REMOVE(&freeBlocksList, (struct BlockElement* ) element);

			if (merged_block_va == NULL) {
				merged_block_va = va;
			}
		}
	}

	if (total_merged_size == free_block_size) {
		merged_block_va = va;
	}

	set_block_data(merged_block_va, total_merged_size, 0);
}

//=========================================
// [6] REALLOCATE BLOCK BY FIRST FIT:
//=========================================
void *realloc_block_FF(void* va, uint32 new_size) {
	//TODO: [PROJECT'24.MS1 - #08] [3] DYNAMIC ALLOCATOR - realloc_block_FF
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//	panic("realloc_block_FF is not implemented yet");
	//Your Code is Here...
	if (va == NULL) {
		if (new_size == 0) {
			return NULL;
		} else {
			return alloc_block_FF(new_size);
		}
	}

	if (new_size == 0) {
		free_block(va);
		return NULL;
	}

	// if there is va and size not equal zero
	if (new_size % 2 != 0)
		new_size++;
	if (new_size < DYN_ALLOC_MIN_BLOCK_SIZE)
		new_size = DYN_ALLOC_MIN_BLOCK_SIZE;

	new_size += (2 * sizeof(int));
	uint32 old_size = get_block_size(va);

	if (new_size == old_size)
		return va;

	// expand
	if (new_size > old_size) {
		struct BlockElement *next_element = va + old_size;
		uint32 next_element_size = get_block_size(next_element);
		uint32 diff_size = new_size - old_size;

		if (is_free_block(next_element) && next_element_size >= diff_size) {
			LIST_REMOVE(&freeBlocksList, next_element);
			set_block_data(va, new_size, 1);
			// SPLIT
			if ((old_size + next_element_size) - new_size != 0) {
				set_block_data(va + new_size, (old_size + next_element_size) - new_size, 0);
			}
			return va;
		} else {
			free_block(va);
			va = alloc_block_FF(new_size - (2*sizeof(int)));
			return va;
		}
	}

	// shrink
	else {
		struct BlockElement *next_element = va + old_size;

		set_block_data(va, new_size, 1);

		if (is_free_block(next_element)) {
			uint32 next_element_size = get_block_size(next_element);

			LIST_REMOVE(&freeBlocksList, next_element);
			set_block_data(va + new_size, (old_size - new_size) + next_element_size, 0);
		}else {
			if ((old_size - new_size) >= DYN_ALLOC_MIN_BLOCK_SIZE){
				set_block_data(va + new_size, (old_size - new_size), 0);
			}else {
				set_block_data(va, new_size + (old_size - new_size), 1);
			}
		}

		return va;
	}

	sbrk(0);
}

/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
//=========================================
// [7] ALLOCATE BLOCK BY WORST FIT:
//=========================================
void *alloc_block_WF(uint32 size) {
	panic("alloc_block_WF is not implemented yet");
	return NULL;
}

//=========================================
// [8] ALLOCATE BLOCK BY NEXT FIT:
//=========================================
void *alloc_block_NF(uint32 size) {
	panic("alloc_block_NF is not implemented yet");
	return NULL;
}

