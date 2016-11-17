/*----------------------------------------------------------------------
 * Copyright (C) 2016 Pedro Falcato
 *
 * This file is part of Spartix, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * You can redistribute it and/or modify it under the terms of the GNU
 * General Public License version 2 as published by the Free Software
 * Foundation.
 *----------------------------------------------------------------------*/
#ifndef _APIC_H
#define _APIC_H

#include <stdio.h>

#include <kernel/vmm.h>
#define IOAPIC_BASE_PHYS 0xFEC00000
#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_MSR_BSP 0x100 // Processor is a BSP
#define IA32_APIC_BASE_MSR_ENABLE 0x800

void apic_timer_init();
void ioapic_init();
void set_pin_handlers();
uint32_t read_io_apic(uint32_t reg);
void write_io_apic(uint32_t reg, uint32_t value);
void lapic_init();
void wake_up_processor(uint8_t);

#define LAPIC_EOI	0xB0
#define LAPIC_TSKPRI	0x80
#define LAPIC_ICR	0x300
#define LAPIC_IPIID	0x310
#define LAPIC_LVT_TIMER	0x320
#define LAPIC_PERFCI	0x340
#define LAPIC_LI0	0x350
#define LAPIC_LI1	0x360
#define LAPIC_ERRINT	0x370
#define LAPIC_SPUINT	0xF0
#define LAPIC_TIMER_DIV	0x3E0
#define LAPIC_TIMER_INITCNT 0x380
#define LAPIC_TIMER_CURRCNT 0x390
#define LAPIC_TIMER_IVT_MASK 0x10000
#define LAPIC_LVT_TIMER_MODE_PERIODIC 0x20000
#define APIC_DEFAULT_SPURIOUS_IRQ 15
#endif