#include "kheap.h"

#include <inc/memlayout.h>
#include <inc/dynamic_allocator.h>
#include "memory_manager.h"

//Initialize the dynamic allocator of kernel heap with the given start address, size & limit
//All pages in the given range should be allocated
//Remember: call the initialize_dynamic_allocator(..) to complete the initialization
//Return:
//	On success: 0
//	Otherwise (if no memory OR initial size exceed the given limit): PANIC
int initialize_kheap_dynamic_allocator(uint32 daStart, uint32 initSizeToAllocate, uint32 daLimit)
{
	//TODO: [PROJECT'24.MS2 - #01] [1] KERNEL HEAP - initialize_kheap_dynamic_allocator
	// Write your code here, remove the panic and write your code
	//panic("initialize_kheap_dynamic_allocator() is not implemented yet...!!");
	if((daStart + initSizeToAllocate) > daLimit){
		panic("there is not enough memory space");
	}

	Start = (uint32*) daStart;
	Break = (uint32*) (daStart + initSizeToAllocate);
	Hard_limit = (uint32*) daLimit;
	for(uint32 i = daStart; i < (uint32)Break; i += PAGE_SIZE){
		struct FrameInfo * frame_info = NULL;
		allocate_frame(&frame_info);
		map_frame(ptr_page_directory, frame_info, i, PERM_WRITEABLE | PERM_USER);
	}

	initialize_dynamic_allocator( daStart , initSizeToAllocate);
	return 0;

}

void* sbrk(int numOfPages)
{
	/* numOfPages > 0: move the segment break of the kernel to increase the size of its heap by the given numOfPages,
	 * 				you should allocate pages and map them into the kernel virtual address space,
	 * 				and returns the address of the previous break (i.e. the beginning of newly mapped memory).
	 * numOfPages = 0: just return the current position of the segment break
	 *
	 * NOTES:
	 * 	1) Allocating additional pages for a kernel dynamic allocator will fail if the free frames are exhausted
	 * 		or the break exceed the limit of the dynamic allocator. If sbrk fails, return -1
	 */

	//MS2: COMMENT THIS LINE BEFORE START CODING==========
	return (void*)-1 ;
	//====================================================

	//TODO: [PROJECT'24.MS2 - #02] [1] KERNEL HEAP - sbrk
	// Write your code here, remove the panic and write your code
	panic("sbrk() is not implemented yet...!!");
}

//TODO: [PROJECT'24.MS2 - BONUS#2] [1] KERNEL HEAP - Fast Page Allocator

void* kmalloc(unsigned int size)
{
	//TODO: [PROJECT'24.MS2 - #03] [1] KERNEL HEAP - kmalloc
	// Write your code here, remove the panic and write your code
	if (size <= DYN_ALLOC_MAX_BLOCK_SIZE){
		return alloc_block_FF(size);
	}else {
		if (isKHeapPlacementStrategyFIRSTFIT() == 1){
			// allocate on page boundaries using first fit strategy
			int num_of_required_pages = (size + PAGE_SIZE - 1)/PAGE_SIZE;
			int continious_page_counter = 0;
			uint32 start_page = 0;

			if (size > KERNEL_HEAP_MAX - (uint32)Hard_limit - PAGE_SIZE){
				return NULL;
			}

			for (uint32 i = (uint32)Hard_limit + PAGE_SIZE; i < KERNEL_HEAP_MAX; i += PAGE_SIZE){
				if (continious_page_counter == num_of_required_pages){
					break;
				}

				uint32* ptr_page_table = NULL;
				struct FrameInfo* assigned_frame = get_frame_info(ptr_page_directory, i, &ptr_page_table);

				if (assigned_frame == NULL){
					if (start_page == 0){
						start_page = i;
					}
					continious_page_counter++;
				}else {
					continious_page_counter = 0;
					start_page = 0;
				}
			}

			int counter = 0;
			struct FrameInfo * iterator;
			LIST_FOREACH(iterator, &MemFrameLists.free_frame_list){
				if (counter == num_of_required_pages){
					break;
				}

				allocate_frame(&iterator);
				uint32 current_page_address = start_page + (counter * PAGE_SIZE);
				map_frame(ptr_page_directory, iterator, current_page_address, PERM_WRITEABLE | PERM_USER);

				counter++;
			}

			return (void*)start_page;

		}else if (isKHeapPlacementStrategyBESTFIT() == 1){
			// allocate on page boundaries using best fit strategy
		}
	}
	return NULL;
}

void kfree(void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #04] [1] KERNEL HEAP - kfree
	// Write your code here, remove the panic and write your code
	panic("kfree() is not implemented yet...!!");

	//you need to get the size of the given allocation using its address
	//refer to the project presentation and documentation for details

}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #05] [1] KERNEL HEAP - kheap_physical_address
	// Write your code here, remove the panic and write your code
	//panic("kheap_physical_address() is not implemented yet...!!");
	uint32* ptr_page_table = NULL;
	get_page_table(ptr_page_directory, virtual_address, &ptr_page_table);

	if (ptr_page_table == NULL){
		return 0;
	}

	uint32 page_entry = ptr_page_table[PTX(virtual_address)];
	uint32 offset = (virtual_address << 20);
	uint32 physical_address = (page_entry << 12) | offset;

	return physical_address;
	//return the physical address corresponding to given virtual_address
	//refer to the project presentation and documentation for details

	//EFFICIENT IMPLEMENTATION ~O(1) IS REQUIRED ==================
}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//TODO: [PROJECT'24.MS2 - #06] [1] KERNEL HEAP - kheap_virtual_address
	// Write your code here, remove the panic and write your code
	//panic("kheap_virtual_address() is not implemented yet...!!");
	uint32 offset = physical_address % PAGE_SIZE;
	int frame_number = physical_address/PAGE_SIZE;

	for (int i = 0; i < 1024; i++){
		if (ptr_page_directory[i] & PERM_PRESENT){
			uint32* ptr_page_table = (uint32*)ptr_page_directory[i];

			for (int j = 0; j < 1024; j++){
				if ((ptr_page_table[j] >> 12) == frame_number * PAGE_SIZE){
					uint32 virtual_address = (i << 22) | (j << 12) | offset;
					return virtual_address;
				}
			}
		}
	}

	return 0;
	//return the virtual address corresponding to given physical_address
	//refer to the project presentation and documentation for details

	//EFFICIENT IMPLEMENTATION ~O(1) IS REQUIRED ==================
}
//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//
// krealloc():

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, if moved to another loc: the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().

void *krealloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT'24.MS2 - BONUS#1] [1] KERNEL HEAP - krealloc
	// Write your code here, remove the panic and write your code
	return NULL;
	panic("krealloc() is not implemented yet...!!");
}
