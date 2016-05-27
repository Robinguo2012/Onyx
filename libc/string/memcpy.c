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
#include <string.h>

void *memcpy(void *__restrict__ dstptr, const void *__restrict__ srcptr,
	     size_t size)
{
	unsigned char *dst = (unsigned char *) dstptr;
	const unsigned char *src = (const unsigned char *) srcptr;
	size_t i;
	for ( i = 0; i < size; i++)
		dst[i] = src[i];
	return dstptr;
}
