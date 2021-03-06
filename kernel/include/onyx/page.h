/*
* Copyright (c) 2016, 2017 Pedro Falcato
* This file is part of Onyx, and is released under the terms of the MIT License
* check LICENSE at the root directory for more information
*/
#ifndef _KERNEL_PAGE_H
#define _KERNEL_PAGE_H

#include <stddef.h>
#include <stdint.h>

#include <onyx/spinlock.h>
#include <onyx/compiler.h>
#include <onyx/list.h>

/* The default physical allocator is the buddy allocator */
#define CONFIG_BUDDY_ALLOCATOR		1
#if defined(__x86_64__)

#define PAGES_PER_AREA 512
#define MAX_ORDER 11
#define HUGE_PAGE_SIZE 0x200000

#define DMA_UPPER_LIMIT (void*) 0x1000000
#define HIGH_MEM_FLOOR  DMA_UPPER_LIMIT
#define HIGH_MEM_LIMIT  (void*) 0xFFFFFFFF
#define HIGH_MEM_64_FLOOR HIGH_MEM_LIMIT
#define HIGH_MEM_64_LIMIT (void*) -1

#else
#error "Define PAGES_PER_AREA and/or MAX_ORDER"
#endif

#define NR_ZONES 3
#define IS_HUGE_ALIGNED(x) (((unsigned long) x % HUGE_PAGE_SIZE) ? 0 : 1)
#define ALIGN_TO(x, y) (((unsigned long)x + (y - 1)) & -y)
#define IS_DMA_PTR(x) x < DMA_UPPER_LIMIT
#define IS_HIGHMEM_PTR(x) x > HIGH_MEM_FLOOR && x < HIGH_MEM_LIMIT
#define IS_HIGHMEM64_PTR(x) x > HIGH_MEM_64_FLOOR && x < HIGH_MEM_64_LIMIT

#define ilog2(X) ((unsigned) (8*sizeof (unsigned long long) - __builtin_clzll((X)) - 1))


/* Passed to __alloc_page() */
#define PAGE_AREA_DMA (1 << 0)
#define PAGE_AREA_HIGH_MEM 	(1 << 1)
#define PAGE_AREA_HIGH_MEM_64 	(1 << 2)
#define PAGE_NO_RETRY		(1 << 3)

/* struct page - Represents every usable page on the system 
 * Everything is native-word-aligned in order to allow atomic changes
 * Careful adding fields in - they may increase the memory use exponentially
*/
struct page
{
	void *paddr;
	unsigned long ref;
	struct page *next;
};
#define PAGE_HASHTABLE_ENTRIES 0x4000	
struct page_hashtable
{
	struct page *table[PAGE_HASHTABLE_ENTRIES];
};
#ifdef CONFIG_BUDDY_ALLOCATOR
/* A structure describing areas of size 2^order pages */
typedef struct free_area
{
	/* Each of them contains a list of free pages */
	struct list_head free_list;
	/* And a bitmap of buddies */
	unsigned long *map;
	/* Each "sub-zone" has a buddy, and to merge an area into a larger area, 
	 * we need both buddies to be free; because of that, we use a bitmap of buddies to represent them.
	 * We use a bit for each buddy, if it's set, the buddy is allocated, if not, it's set to 0.
	*/
} free_area_t;

#else

typedef struct page_zone
{
	/* We obviously need a lock to protect this page zone */
	spinlock_t lock __attribute__((aligned(16)));
	page_area_t *free_areas __attribute__((aligned(16)));
	/* The name of the page zone */
	char *name;
	/* The size of the page zone */
	size_t size;
	/* The allocated/free pages */
	size_t allocated_pages, free_pages;

} page_zone_t;

typedef struct page_area
{
	struct page_area *prev;
	struct page_area *next;
} page_area_t;

#endif /* CONFIG_BUDDY_ALLOCATOR */

struct memstat
{
	size_t free_mem;
	size_t allocated_mem;
};

#ifdef __cplusplus
extern "C" {
#endif

void *__alloc_page(int opt);
void *__alloc_pages(int order);
void __free_page(void *page);
void __free_pages(void *pages, int order);
void page_get_stats(struct memstat *memstat);
void page_init(void);
unsigned int page_hash(uintptr_t p);
bool pages_are_registered(void);
void page_register_pages(void);
struct page *phys_to_page(uintptr_t phys);
unsigned long page_increment_refcount(void *paddr);
unsigned long page_decrement_refcount(void *paddr);

#ifdef __cplusplus
}
#endif
#endif
