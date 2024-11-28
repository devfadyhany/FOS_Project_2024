#include <inc/lib.h>

typedef struct{
	uint32 virtual_address;
	int num_of_marked_pages;
	int shared_object_id;
}MarkedElement;
MarkedElement marked_page[131072];
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
		return alloc_block_FF(size);
	} else {
		int num_of_required_pages = ROUNDUP(size, PAGE_SIZE) / PAGE_SIZE;
		int continious_page_counter = 0;
		uint32 start_page = 0;

		for (uint32 i = (uint32) myEnv->Hard_limit + PAGE_SIZE; i <= USER_HEAP_MAX; i += PAGE_SIZE) {
			if (continious_page_counter == num_of_required_pages) {
				break;
			}

			int page_num = (i - USER_HEAP_START) / PAGE_SIZE;

			MarkedElement current_page = marked_page[page_num];

			if (current_page.virtual_address != 0) {
				continious_page_counter = 0;
				start_page = 0;
				uint32 marked_size = (current_page.num_of_marked_pages) * (PAGE_SIZE);
				i += marked_size - PAGE_SIZE;
				continue;
			}

			if (start_page == 0) {
				start_page = i;
			}
			continious_page_counter++;
		}

		if (continious_page_counter != num_of_required_pages || size > (USER_HEAP_MAX - start_page)) {
			return NULL;
		}

		sys_allocate_user_mem(start_page, size);

		for (uint32 i = start_page; i < start_page + size; i += PAGE_SIZE) {
			int page_num = (i - USER_HEAP_START) / PAGE_SIZE;

			marked_page[page_num].virtual_address = i;
		}

		marked_page[(start_page - USER_HEAP_START) / PAGE_SIZE].num_of_marked_pages = num_of_required_pages;

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
	// panic("free() is not implemented yet...!!");
	uint32 va = (uint32) virtual_address;

	if (va >= USER_HEAP_START && va <= (uint32) (myEnv->Break)) {
		free_block(virtual_address);
		return;
	} else if (va >= ((uint32) (myEnv->Hard_limit) + PAGE_SIZE)&& va <= USER_HEAP_MAX) {
		int start_page_index = (va - USER_HEAP_START) / PAGE_SIZE;
		int num_of_pages = marked_page[start_page_index].num_of_marked_pages;

		for (uint32 i = va; i < va + (num_of_pages * PAGE_SIZE); i += PAGE_SIZE) {
			int page_num = (i - USER_HEAP_START) / PAGE_SIZE;

			marked_page[page_num].virtual_address = 0;
			marked_page[page_num].num_of_marked_pages = 0;
			marked_page[page_num].shared_object_id = 0;
		}

		sys_free_user_mem(va, num_of_pages * PAGE_SIZE);
	} else {
		panic("invalid address");
	}

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

	for (uint32 i = (uint32) myEnv->Hard_limit + PAGE_SIZE; i < USER_HEAP_MAX; i += PAGE_SIZE) {
		if (continious_page_counter == num_of_required_pages) {
			break;
		}

		int page_num = (i - USER_HEAP_START) / PAGE_SIZE;

		if (marked_page[page_num].virtual_address != 0) {
			continious_page_counter = 0;
			start_page = 0;
			uint32 marked_size = (marked_page[page_num].num_of_marked_pages) * (PAGE_SIZE);
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

	int res = sys_createSharedObject(sharedVarName, size, isWritable,(void*) start_page);

	if (res == E_SHARED_MEM_EXISTS || res == E_NO_SHARE) {
		return NULL;
	}

	for (uint32 i = start_page; i < start_page + size; i += PAGE_SIZE) {
		int page_num = (i - USER_HEAP_START) / PAGE_SIZE;

		marked_page[page_num].virtual_address = i;
	}

	marked_page[(start_page - USER_HEAP_START) / PAGE_SIZE].num_of_marked_pages = num_of_required_pages;
	marked_page[(start_page - USER_HEAP_START) / PAGE_SIZE].shared_object_id = res;

	return (void*) start_page;

}

//========================================
// [5] SHARE ON ALLOCATED SHARED VARIABLE:
//========================================
void* sget(int32 ownerEnvID, char *sharedVarName) {
	//TODO: [PROJECT'24.MS2 - #20] [4] SHARED MEMORY [USER SIDE] - sget()
	// Write your code here, remove the panic and write your code
	int size = sys_getSizeOfSharedObject(ownerEnvID, sharedVarName);

	if (size == E_SHARED_MEM_NOT_EXISTS) {
		return NULL;
	}
	// [3] search ff for space

	int num_of_required_pages = ROUNDUP(size, PAGE_SIZE) / PAGE_SIZE;
	int continious_page_counter = 0;
	uint32 start_page = 0;
	for (uint32 i = (uint32) myEnv->Hard_limit + PAGE_SIZE; i < USER_HEAP_MAX; i += PAGE_SIZE) {
		if (continious_page_counter == num_of_required_pages) {
			break;
		}

		int page_num = (i - USER_HEAP_START) / PAGE_SIZE;

		if (marked_page[page_num].virtual_address != 0) {
			continious_page_counter = 0;
			start_page = 0;
			uint32 marked_size = (marked_page[page_num].num_of_marked_pages) * (PAGE_SIZE);
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

	int id = sys_getSharedObject(ownerEnvID, sharedVarName,(uint32 *) start_page);
	if (id == E_SHARED_MEM_NOT_EXISTS) {
		return NULL;
	}

	for (uint32 i = start_page; i < start_page + size; i += PAGE_SIZE) {
		int page_num = (i - USER_HEAP_START) / PAGE_SIZE;

		marked_page[page_num].virtual_address = i;
	}

	marked_page[(start_page - USER_HEAP_START) / PAGE_SIZE].num_of_marked_pages = num_of_required_pages;

	return (uint32 *) start_page;
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
	// panic("sfree() is not implemented yet...!!");
	uint32 va = (uint32)virtual_address;
	int page_num = (va - USER_HEAP_START)/PAGE_SIZE;

	int sharedObjectId = marked_page[page_num].shared_object_id;

	sys_freeSharedObject(sharedObjectId, virtual_address);

	for (int i = page_num; i < marked_page[page_num].num_of_marked_pages; i++){
		marked_page[page_num].virtual_address = 0;
		marked_page[page_num].num_of_marked_pages = 0;
		marked_page[page_num].shared_object_id = 0;
	}
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
