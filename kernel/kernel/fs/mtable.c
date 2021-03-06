/*
* Copyright (c) 2017 Pedro Falcato
* This file is part of Onyx, and is released under the terms of the MIT License
* check LICENSE at the root directory for more information
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include <onyx/vfs.h>
#include <onyx/mtable.h>
#include <onyx/mutex.h>

static mountpoint_t *mtable = NULL;
static size_t nr_mtable_entries = 0;
static mutex_t mtable_lock = MUTEX_INITIALIZER;
struct inode *mtable_lookup(struct inode *mountpoint)
{
	if(!mtable)
		return errno = ENOENT, NULL;
	mutex_lock(&mtable_lock);
	for(size_t i = 0; i < nr_mtable_entries; i++)
	{
		/* Found a mountpoint, return its target */
		if(mtable[i].ino == mountpoint->inode && mtable[i].dev == mountpoint->dev)
		{
			mutex_unlock(&mtable_lock);
			return mtable[i].rootfs;
		}
	}
	mutex_unlock(&mtable_lock);
	return errno = ENOENT, NULL;
}
int mtable_mount(struct inode *mountpoint, struct inode *rootfs)
{
	assert(mountpoint);
	assert(rootfs);
	mutex_lock(&mtable_lock);
	nr_mtable_entries++;
	mountpoint_t *new_mtable = malloc(nr_mtable_entries * sizeof(mountpoint_t));
	if(!new_mtable)
	{
		nr_mtable_entries--;
		mutex_unlock(&mtable_lock);
		return errno = ENOMEM, -1;
	}
	if(mtable)
		memcpy(new_mtable, mtable, (nr_mtable_entries-1) * sizeof(mountpoint_t));
	new_mtable[nr_mtable_entries - 1].ino = mountpoint->inode;
	new_mtable[nr_mtable_entries - 1].dev = mountpoint->dev;
	new_mtable[nr_mtable_entries - 1].rootfs = rootfs;
	rootfs->refcount++;
	mountpoint_t *old = mtable;
	mtable = new_mtable;

	free(old);
	mutex_unlock(&mtable_lock);
	
	return 0;
}
