/*
* Copyright (c) 2016, 2017 Pedro Falcato
* This file is part of Onyx, and is released under the terms of the MIT License
* check LICENSE at the root directory for more information
*/
#include <stdlib.h>
#include <string.h>

#include <onyx/dev.h>
#include <onyx/panic.h>
#include <onyx/compiler.h>

size_t zero_read(int flags, size_t offset, size_t count, void *buf, struct inode *n)
{
	/* While reading from /dev/zero, all you read is zeroes. Just memset the buf. */
	UNUSED(offset);
	UNUSED(n);
	UNUSED(flags);
	memset(buf, 0, count);
	return count;
}
void zero_init()
{
	struct inode *n = creat_vfs(slashdev, "zero", 0666);
	if(!n)
		panic("Could not create /dev/zero!\n");
	n->type = VFS_TYPE_BLOCK_DEVICE;
	
	struct minor_device *min = dev_register(1, 0);
	if(!min)
		panic("Could not create a device ID for /dev/zero!\n");
	
	min->fops = malloc(sizeof(struct file_ops));
	if(!min->fops)
		panic("Could not create a file operation table for /dev/zero!\n");
	memset(min->fops, 0, sizeof(struct file_ops));

	min->fops->read = zero_read;
	n->dev = min->majorminor;
	memcpy(&n->fops, min->fops, sizeof(struct file_ops));
}
