//
//  libmasochist.h
//  masochist
//
//  Created by Squiffy on 11/11/14.
//  Copyright (c) 2014 squiffypwn. All rights reserved.
//

#ifndef __masochist__libmasochist__
#define __masochist__libmasochist__

#include <mach/mach_types.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <mach/mach_types.h>
#include <sys/systm.h>
#include <stdbool.h>

#define SLIDE 0x0
#define KERNEL_BASE (0xffffff8000200000 + SLIDE) /* use slide.c */

extern struct mach_header_64 *kernel_header;

kern_return_t libmasochist_init();

#endif /* defined(__masochist__libmasochist__) */
