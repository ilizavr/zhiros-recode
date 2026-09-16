/*
 * Copyright (c) 2026 ilizavr
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "../lib/zhirtypes.h"

extern void pic_remap();
extern void set_idt_gate(u8 num,void* base);
extern void init_idt();
extern void pic_eoi();
