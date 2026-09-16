/*
 * Copyright (c) 2026 ilizavr
 * SPDX-License-Identifier: GPL-3.0-or-later
 */


#include "../lib/zhirtypes.h"
extern API bool mount(struct disk* dsk, void * open_fnc);
extern API struct file* open(char disk_letter,char *path);
