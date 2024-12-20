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

int num_of_processes_in_kernel = 0;
uint32 last_free_place = 0;
int first_time_allocating = 1;

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
//	kernel_lock = 0;
	for(uint32 i = daStart; i < (uint32)Break; i += PAGE_SIZE){
		struct FrameInfo * frame_info = NULL;
		allocate_frame(&frame_info);
		map_frame(ptr_page_directory, frame_info, i, PERM_WRITEABLE);

		frame_info->mapped_page = i;
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
	 * 		or the break exceed the limit of the dynamic allocator. If sbrk fails, kernel should panic(...)
	 */

	//MS2: COMMENT THIS LINE BEFORE START CODING====

	//====================================================

	//[PROJECT'24.MS2] Implement this function
	// Write your code here, remove the panic and write your code


	uint32 * check_max = (uint32 *)((char *)Break + ((int)(numOfPages)*(PAGE_SIZE )));
	uint32 * old_break=Break;
     if(check_max>Hard_limit || numOfPages<0)//Break
		{
		return (void*)-1 ;
		}
	else if(numOfPages==0)
		 {
			   return (void*)(Break);
		 }
		else
		{
		     if(numOfPages>0)
				{
		    	 Break=check_max;
                   for(uint32 i = (uint32)old_break; i < (uint32)Break; i += PAGE_SIZE){
                	   struct FrameInfo *frame_info = NULL;
                	   int is_allocate= allocate_frame(&frame_info);
                	   if(is_allocate!=E_NO_MEM)
                	   {
                	   		        map_frame(ptr_page_directory, frame_info, i, PERM_WRITEABLE);
                	   		        frame_info->mapped_page = i;
                	   }
                	   else
                	   {
                		   panic("NO Memory...!!");
                	   }
                   	}
					return (void*)(old_break);
				}
		}
		return (void*)-1 ;
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
			int num_of_required_pages = ROUNDUP(size, PAGE_SIZE) / PAGE_SIZE;
			int continious_page_counter = 0;
			uint32 start_page = 0;

			uint32* ptr_page_table = NULL;
			struct FrameInfo* assigned_frame = NULL;

			int remaining_process = num_of_processes_in_kernel;

			if (last_free_place == 0 || first_time_allocating == 0){
				for (uint32 i = (uint32)Hard_limit + PAGE_SIZE; i < KERNEL_HEAP_MAX; i += PAGE_SIZE){
					if (continious_page_counter == num_of_required_pages){
						break;
					}

					if (remaining_process == 0 && ((KERNEL_HEAP_MAX - i) / PAGE_SIZE) >= num_of_required_pages){
						continious_page_counter = num_of_required_pages;
						start_page = i;
						break;
					}

					assigned_frame = get_frame_info(ptr_page_directory, i, &ptr_page_table);

					if (assigned_frame == NULL){
						if (start_page == 0){
							start_page = i;
						}
						continious_page_counter++;
					}else {
						continious_page_counter = 0;
						start_page = 0;
						i += (assigned_frame->process_num_of_pages) * PAGE_SIZE - PAGE_SIZE;
						remaining_process--;
					}
				}
			}else {
				start_page = last_free_place;

				if (((KERNEL_HEAP_MAX - start_page) / PAGE_SIZE) >= num_of_required_pages){
					continious_page_counter = num_of_required_pages;
				}
			}

			if (continious_page_counter != num_of_required_pages){
				return NULL;
			}

			struct FrameInfo * iterator = NULL;
			uint32 current_page_address;

//			while(xchg(&(kernel_lock), 1) != 0);

			for (int i = 0; i < num_of_required_pages; i++){
				allocate_frame(&iterator);
				current_page_address = start_page + (i * PAGE_SIZE);
				map_frame(ptr_page_directory, iterator, current_page_address, PERM_WRITEABLE);

				uint32 va_permissions = pt_get_page_permissions(ptr_page_directory, current_page_address);

				iterator->mapped_page = current_page_address;

				if (current_page_address == start_page){
					iterator->process_start_page = start_page;
					iterator->process_num_of_pages = num_of_required_pages;
				}
			}

			num_of_processes_in_kernel++;
			last_free_place = start_page + (num_of_required_pages * PAGE_SIZE);

//			kernel_lock = 0;

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
	if (virtual_address == NULL) {
		panic("kfree() received NULL address!");
		return;
	}

	uint32* va = (uint32*)virtual_address;

    if (va >= (uint32*)KERNEL_HEAP_START && va <= Hard_limit){
    	 free_block(virtual_address);
    }
    else if (va >= (uint32*)((uint32)Hard_limit + PAGE_SIZE) && va <= (uint32*)KERNEL_HEAP_MAX) {

    	uint32 physical_address = kheap_physical_address((uint32)va);
    	struct FrameInfo* frame_to_get_pages = to_frame_info(physical_address);

    	int num_of_pages = frame_to_get_pages->process_num_of_pages;

        uint32 address = (uint32)virtual_address;
        uint32* ptr_page_table = NULL;
        struct FrameInfo* frame = NULL;

//		while(xchg(&(kernel_lock), 1) != 0);

        for (int i = 0; i < num_of_pages; i++) {

            frame = get_frame_info(ptr_page_directory, address, &ptr_page_table);

            if (frame != NULL) {
                unmap_frame(ptr_page_directory, address);

                frame->mapped_page = 0;
                frame->process_start_page = 0;
                frame->process_num_of_pages = 0;

                tlb_invalidate(ptr_page_directory, (void*) address); // Refresh the TLB
            }
            address += PAGE_SIZE;
        }
        num_of_processes_in_kernel--;
        last_free_place = 0;
        first_time_allocating = 0;

//        kernel_lock = 0;
    }
    else {
//         Invalid address, should panic
        panic("kfree() received an invalid address outside allocator ranges!");
    }
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

	if ((ptr_page_table[PTX(virtual_address)] & PERM_PRESENT) == PERM_PRESENT){
		uint32 page_entry = ptr_page_table[PTX(virtual_address)] & ~0xFFF;

		uint32 offset = (virtual_address & 0xFFF);
		uint32 physical_address = page_entry + offset;

		return physical_address;
	}

	return 0;
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
	struct FrameInfo* frame = to_frame_info(physical_address);

	if (frame->mapped_page != 0){
		uint32 virtual_address = frame->mapped_page | offset;
		return virtual_address;
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
	//return NULL;
	//panic("krealloc() is not implemented yet...!!");
	if (virtual_address == NULL){
		return kmalloc(new_size);
	}

	if (new_size == 0){
		kfree(virtual_address);
		return NULL;
	}

	int blockAllocator_address = ((uint32*)virtual_address >= Start && (uint32*)virtual_address < Break);
	int blockAllocator_size = (new_size <= DYN_ALLOC_MAX_BLOCK_SIZE);

	if (blockAllocator_address){
		if (blockAllocator_size){
			uint32* new_address = (uint32*)realloc_block_FF(virtual_address ,new_size);
			return new_address;
		}else {
			uint32* new_address = (uint32*)kmalloc(new_size);
			if (new_address != NULL){
				uint32 size = get_block_size(virtual_address);
				memcpy(new_address, virtual_address, size - (2 *sizeof(int)));
				kfree(virtual_address);
			}
			return new_address;
		}
	}else {
		if (blockAllocator_size){
			uint32* new_address = (uint32*)alloc_block_FF(new_size);

			if (new_address != NULL){
				kfree(virtual_address);
			}
			return new_address;
		}else {
			uint32* new_address = (uint32*)kmalloc(new_size);
			if (new_address != NULL){
				uint32* ptr_page_table = NULL;
				struct FrameInfo* frame = get_frame_info(ptr_page_directory, (uint32)virtual_address, &ptr_page_table);
				memcpy(new_address, virtual_address, frame->process_num_of_pages * PAGE_SIZE);
				kfree(virtual_address);
			}
			return new_address;
		}
	}
	return NULL;
}
