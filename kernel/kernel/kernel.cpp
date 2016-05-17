/* Copyright 2016 Pedro Falcato

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http ://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
/**************************************************************************
 *
 *
 * File: kernel.c
 *
 * Description: Main kernel file, contains the entry point and initialization
 *
 * Date: 30/1/2016
 *
 *
 **************************************************************************/
#include <stddef.h>
#include <stdint.h>
#include <multiboot2.h>
#include <stdio.h>
#include <kernel/vmm.h>
#include <kernel/Paging.h>
#include <kernel/pmm.h>
#include <kernel/idt.h>

/* Function: init_arch()
 * Purpose: Initialize architecture specific features, should be hooked by the architecture the kernel will run on
 */
#if defined (__i386__)
	#define KERNEL_VIRTUAL_BASE 0xC0000000
#elif defined (__x86_64__)
	#define KERNEL_VIRTUAL_BASE 0xFFFFFFFF80000000
#endif
void KernelLate();
void InitKeyboard();
extern uint64_t kernelEnd;
extern char __BUILD_NUMBER;
extern char __BUILD_DATE;
#define UNUSED_PARAMETER(x) (void)x
extern "C" void KernelEarly(uintptr_t addr, uint32_t magic)
{
	addr += KERNEL_VIRTUAL_BASE;
	if (magic != MULTIBOOT2_BOOTLOADER_MAGIC)
	{
		return;
	}
	IDT::Init();
	for (struct multiboot_tag *tag = (struct multiboot_tag *)(addr + 8);tag->type != MULTIBOOT_TAG_TYPE_END;
		tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag + ((tag->size + 7) & ~7))) {
		size_t totalMemory = 0;
		switch(tag->type) {
			case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
			{
				struct multiboot_tag_basic_meminfo *memInfo = (struct multiboot_tag_basic_meminfo *) tag;
				totalMemory = memInfo->mem_lower + memInfo->mem_upper;
			}
			case MULTIBOOT_TAG_TYPE_MMAP:
			{
				// Initialize the PMM stack KERNEL_VIRTUAL_BASE + 1MB. TODO: detect size of modules and calculate size from that
				PhysicalMemoryManager::Init(totalMemory, (uintptr_t)&kernelEnd + 0x100000);
				struct multiboot_tag_mmap *mmaptag = (struct multiboot_tag_mmap *) tag;
				size_t entries = mmaptag->size / mmaptag->entry_size;
				struct multiboot_mmap_entry *mmap = (struct multiboot_mmap_entry*)mmaptag->entries;
				for(size_t i = 0; i <= entries; i++) {
					if(mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
						PhysicalMemoryManager::Push(mmap->addr, mmap->len, 0x200000);
					}
					mmap++;
				}
			}
			case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
			{
				struct multiboot_tag_framebuffer *tagfb = (struct multiboot_tag_framebuffer *) tag;
				void *fb = (void *) (unsigned long) tagfb->common.framebuffer_addr;
				(void)fb;
				VirtualMemoryManager::Init();
				// Use Paging:: directly, as we have no heap yet
			}
		}
	}
	char* mem = (char *)Paging::MapPhysToVirt(0x0,(uintptr_t)0x0, 0);
	*mem = 0xFF;
	while(1);
}
extern "C" void KernelMain()
{

	printf("Spartix kernel %s branch %s build %d\n", KERNEL_VERSION,
	       KERNEL_BRANCH, &__BUILD_NUMBER);
	printf("Built on %d\n", &__BUILD_DATE);
	
	for (;;) {
		__asm__ __volatile__ ("hlt");
	}
}
void KernelLate()
{
	for (;;) {
		__asm__ __volatile__ ("hlt");
	}
}