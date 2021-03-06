/*
* Copyright (c) 2016, 2017 Pedro Falcato
* This file is part of Onyx, and is released under the terms of the MIT License
* check LICENSE at the root directory for more information
*/
/**************************************************************************
 *
 *
 * File: panic.c
 *
 * Description: Contains the implementation of the panic function
 *
 * Date: 1/2/2016
 *
 *
 **************************************************************************/
#include <stdio.h>
#pragma GCC target("sse2", "3dnow")
#include <x86intrin.h>
#include <inttypes.h>

#include <onyx/cpu.h>
#include <onyx/registers.h>
#include <onyx/compiler.h>
#include <onyx/paging.h>
#include <onyx/vmm.h>
#include <onyx/task_switching.h>
#include <onyx/process.h>
#include <onyx/panic.h>
#include <onyx/modules.h>
const char *skull = "            _,,,,,,,_\n\
          ,88888888888,\n\
        ,888\'       \\`888,\n\
        888\' 0     0 \\`888\n\
       888      0      888\n\
       888             888\n\
       888    ,000,    888\n\
        888, 0     0 ,888\n\
        \'888,       ,888\'\n\
          \'8JGS8888888\'\n\
            \\`\\`\\`\\`\\`\\`\\`\\`\n";
int panicing = 0;
extern void *stack_trace();
__attribute__ ((noreturn, cold, noinline))
void panic(const char *msg)
{
	/* First, disable interrupts */
	DISABLE_INTERRUPTS();
	char buffer[1000];
	panicing = 1;
	memset(buffer, 0, 1000);
	/* And dump the context to it */
#ifdef __x86_64__
	register uintptr_t rax __asm__("rax");
	register uintptr_t rbx __asm__("rbx");
	register uintptr_t rcx __asm__("rcx");
	register uintptr_t rdx __asm__("rdx");
	register uintptr_t rdi __asm__("rdi");
	register uintptr_t rsi __asm__("rsi");
	register uintptr_t rsp __asm__("rsp");
	register uintptr_t r8 __asm__("r8");
	register uintptr_t r9 __asm__("r9");
	register uintptr_t r10 __asm__("r10");
	register uintptr_t r11 __asm__("r11");
	register uintptr_t r12 __asm__("r12");
	register uintptr_t r13 __asm__("r13");
	register uintptr_t r14 __asm__("r14");
	register uintptr_t r15 __asm__("r15");
	uintptr_t rflags = __readeflags();
	uintptr_t cr0 = 0;
	uintptr_t cr2 = 0;
	uintptr_t cr4 = 0;
	uintptr_t cr3 = 0;
	uintptr_t rip = (uintptr_t) __builtin_return_address(0);
	uintptr_t rbp = (uintptr_t) __builtin_frame_address(0);
	uintptr_t fs = 0;

	/* Construct the buffer using sprintf */
	snprintf(buffer, 1000, "RAX: %016"PRIx64" RBX: %016"PRIx64" RCX: %016"PRIx64" RDX: %016"PRIx64"\nRDI: %016"PRIx64" RSI: %016"PRIx64" RBP: %016"PRIx64" RSP: %016"PRIx64"\n"
			"R8:  %016"PRIx64" R9:  %016"PRIx64" R10: %016"PRIx64" R11: %016"PRIx64"\nR12: %016"PRIx64" R13: %016"PRIx64" R14: %016"PRIx64" R15: %016"PRIx64"\n"
			"CR0: %016"PRIx64" CR2: %016"PRIx64" CR3: %016"PRIx64" CR4: %016"PRIx64"\n"
			"RIP: %016"PRIx64" RFLAGS: %08"PRIx64" GS:  %016"PRIx64" FS:  %016"PRIx64"\n",
		rax, rbx, rcx, rdx, rdi, rsi, rbp, rsp, r8, r9, r10, r11, r12, r13, r14, r15, cr0, cr2, cr3,
		cr4, rip, rflags, 0ul, fs);
#else
	#error "Implement thread context printing in your arch"
#endif
	printk("panic: %s\nThread Context:\n%s", msg, buffer);

	module_dump();
	printk("Stack dump: \n");

	stack_trace();
	printk("Killing cpus\n");
	cpu_kill_other_cpus();
	halt();
	__builtin_unreachable();
}
void __assert_fail(const char * assertion, const char * file, unsigned int line, const char * function)
{
	char buf[200] = {0};
	snprintf(buf, 200, "Assertion %s failed in %s:%u, in function %s\n", assertion, file, line, function);
	printk(buf);
	panic("Assertion failed!\n");
}
