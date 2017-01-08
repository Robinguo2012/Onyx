/*----------------------------------------------------------------------
 * Copyright (C) 2016, 2017 Pedro Falcato
 *
 * This file is part of Spartix, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * You can redistribute it and/or modify it under the terms of the GNU
 * General Public License version 2 as published by the Free Software
 * Foundation.
 *----------------------------------------------------------------------*/
#ifndef _KERNEL_INFO_H
#define _KERNEL_INFO_H

#undef stringify
#define stringify(str) #str
#define OS_NAME "Spartix"
#define OS_RELEASE "0.1-dev"
#define OS_VERSION "SMP "__DATE__" "__TIME__

#if defined(__x86_64__)
#define OS_MACHINE "x86_64 amd64"
#else
#error "Define a machine string for your architecture"
#endif






#endif