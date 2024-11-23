#include <inc/lib.h>

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

//=============================================
// [1] CHANGE THE BREAK LIMIT OF THE USER HEAP:
//=============================================
/*2023*/
void* sbrk(int increment) {
	return (void*) sys_sbrk(increment);
}

//=================================
// [2] ALLOCATE SPACE IN USER HEAP:
//=================================
void* malloc(uint32 size) {
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	if (size == 0)
		return NULL;
	//==============================================================
	//TODO: [PROJECT'24.MS2 - #12] [3] USER HEAP [USER SIDE] - malloc()
	// Write your code here, remove the panic and write your code
	//panic("malloc() is not implemented yet...!!");
	if (size <= DYN_ALLOC_MAX_BLOCK_SIZE) {
		cprintf("size: %d\n", size);
		return alloc_block_FF(size);
	} else {
		int num_of_required_pages = ROUNDUP(size, PAGE_SIZE) / PAGE_SIZE;
		int continious_page_counter = 0;
		uint32 start_page = 0;

//		cprintf("size: %d, num_of_pages: %d\n", size, num_of_required_pages);

		for (uint32 i = (uint32) myEnv->Hard_limit + PAGE_SIZE;
				i <= USER_HEAP_MAX; i += PAGE_SIZE) {
			if (continious_page_counter == num_of_required_pages) {
				break;
			}

			int page_num = (i - USER_HEAP_START) / PAGE_SIZE;

			uint32 current_page = myEnv->marked_page[page_num];

			if (current_page != 0) {
//				cprintf("start: %x\n", i);
//				cprintf("page marked\n");
				continious_page_counter = 0;
				start_page = 0;
//				cprintf("marked pages: %d\n", myEnv->marked_page[page_num]);
				uint32 marked_size = (current_page) * (PAGE_SIZE);
				i += marked_size - PAGE_SIZE;
//				cprintf("end: %x\n", i);
				continue;
			}

			if (start_page == 0) {
				start_page = i;
			}
			continious_page_counter++;
		}

//		cprintf("counter: %d\n", continious_page_counter);

		if (continious_page_counter != num_of_required_pages) {
			return NULL;
		}

		sys_allocate_user_mem(start_page, size);

		return (void*) start_page;
	}
	return NULL;
//Use sys_isUHeapPlacementStrategyFIRSTFIT() and	sys_isUHeapPlacementStrategyBESTFIT()
//to check the current strategy

}

//=================================
// [3] FREE SPACE FROM USER HEAP:
//=================================
void free(void* virtual_address) {
	//TODO: [PROJECT'24.MS2 - #14] [3] USER HEAP [USER SIDE] - free()
	// Write your code here, remove the panic and write your code
	panic("free() is not implemented yet...!!");
}

//=================================
// [4] ALLOCATE SHARED VARIABLE:
//=================================
void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable) {
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	if (size == 0)
		return NULL;
	//==============================================================
	//TODO: [PROJECT'24.MS2 - #18] [4] SHARED MEMORY [USER SIDE] - smalloc()
	// Write your code here, remove the panic and write your code
//	panic("smalloc() is not implemented yet...!!");

	int num_of_required_pages = ROUNDUP(size, PAGE_SIZE) / PAGE_SIZE;
	int continious_page_counter = 0;
	uint32 start_page = 0;

	for (uint32 i = (uint32) myEnv->Hard_limit + PAGE_SIZE; i < USER_HEAP_MAX;
			i += PAGE_SIZE) {
		if (continious_page_counter == num_of_required_pages) {
			break;
		}

		int page_num = (i - USER_HEAP_START) / PAGE_SIZE;

		if (myEnv->marked_page[page_num] != 0) {
			continious_page_counter = 0;
			start_page = 0;
			uint32 marked_size = (myEnv->marked_page[page_num]) * (PAGE_SIZE);
			i += marked_size - PAGE_SIZE;
			continue;
		}

		if (start_page == 0) {
			start_page = i;
		}
		continious_page_counter++;

	}

	if (continious_page_counter != num_of_required_pages) {
		return NULL;
	}

	sys_createSharedObject(sharedVarName, size, isWritable, (void*) start_page);

	return (void*) start_page;
}

//========================================
// [5] SHARE ON ALLOCATED SHARED VARIABLE:
//========================================
void* sget(int32 ownerEnvID, char *sharedVarName) {
	//TODO: [PROJECT'24.MS2 - #20] [4] SHARED MEMORY [USER SIDE] - sget()
	// Write your code here, remove the panic and write your code
	panic("sget() is not implemented yet...!!");
	return NULL;
}

//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//

//=================================
// FREE SHARED VARIABLE:
//=================================
//	This function frees the shared variable at the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from main memory then switch back to the user again.
//
//	use sys_freeSharedObject(...); which switches to the kernel mode,
//	calls freeSharedObject(...) in "shared_memory_manager.c", then switch back to the user mode here
//	the freeSharedObject() function is empty, make sure to implement it.

void sfree(void* virtual_address) {
	//TODO: [PROJECT'24.MS2 - BONUS#4] [4] SHARED MEMORY [USER SIDE] - sfree()
	// Write your code here, remove the panic and write your code
	panic("sfree() is not implemented yet...!!");
}

//=================================
// REALLOC USER SPACE:
//=================================
//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_move_user_mem(...)
//		which switches to the kernel mode, calls move_user_mem(...)
//		in "kern/mem/chunk_operations.c", then switch back to the user mode here
//	the move_user_mem() function is empty, make sure to implement it.
void *realloc(void *virtual_address, uint32 new_size) {
	//[PROJECT]
	// Write your code here, remove the panic and write your code
	panic("realloc() is not implemented yet...!!");
	return NULL;

}

//==================================================================================//
//========================== MODIFICATION FUNCTIONS ================================//
//==================================================================================//

void expand(uint32 newSize) {
	panic("Not Implemented");

}
void shrink(uint32 newSize) {
	panic("Not Implemented");

}
void freeHeap(void* virtual_address) {
	panic("Not Implemented");

}
