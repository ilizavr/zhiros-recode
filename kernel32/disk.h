/*
 * Copyright (c) 2026 ilizavr
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "../lib/zhirtypes.h"

extern API bool diskadd(struct disk* dsk);
extern API struct disk* getdisk(int idx);
extern void ramdisk_init(i_ptr modulestart, i_ptr moduleend);
