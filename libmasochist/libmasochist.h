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

extern struct mach_header_64 *kernel_header;
extern uint64_t kernel_base;

kern_return_t libmasochist_init(uint64_t slide);

#endif /* defined(__masochist__libmasochist__) */
