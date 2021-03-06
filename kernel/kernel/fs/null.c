/*
* Copyright (c) 2016, 2017 Pedro Falcato
* This file is part of Onyx, and is released under the terms of the MIT License
* check LICENSE at the root directory for more information
*/
#include <stdlib.h>
#include <string.h>

#include <onyx/dev.h>
#include <onyx/compiler.h>
#include <onyx/panic.h>

size_t null_write(size_t offset, size_t count, void *buf, struct inode *n)
{
	/* While writing to /dev/null, everything gets discarded. It's basically a no-op. */
	UNUSED(offset);
	UNUSED(count);
	UNUSED(buf);
	UNUSED(n);
	return count;
}
void null_init()
{
	struct inode *n = creat_vfs(slashdev, "null", 0666);
	if(!n)
		panic("Could not create /dev/null!\n");
	n->type = VFS_TYPE_BLOCK_DEVICE;
	
	struct minor_device *min = dev_register(1, 0);
	if(!min)
		panic("Could not create a device ID for /dev/null!\n");
	
	min->fops = malloc(sizeof(struct file_ops));
	if(!min->fops)
		panic("Could not create a file operation table for /dev/null!\n");
	memset(min->fops, 0, sizeof(struct file_ops));

	min->fops->write = null_write;
	n->dev = min->majorminor;
	memcpy(&n->fops, min->fops, sizeof(struct file_ops));
}
