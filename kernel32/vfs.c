/*
 * Copyright (c) 2026 ilizavr
 * SPDX-License-Identifier: GPL-3.0-or-later
 */


#include "vfs.h"

char current_letter_disk = 'A';

struct mount mounts[26];

bool mount(struct disk* dsk, void * open_fnc)
{
    u32 eflags = save_irq();
    if(current_letter_disk>'Z') {
        restore_irq(eflags);
        return false;
    }

    mounts[current_letter_disk-'A'].dsk = dsk;
    mounts[current_letter_disk-'A'].open = open_fnc;
    current_letter_disk++;

    restore_irq(eflags);
    return true;
}

struct file* open(char disk_letter,char *path)
{
    if(disk_letter<'A'||disk_letter>=current_letter_disk) return 0;

    return mounts[disk_letter-'A'].open(mounts[disk_letter-'A'].dsk,path);
}
