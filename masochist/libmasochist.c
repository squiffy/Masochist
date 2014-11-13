//
//  libmasochist.c
//  masochist
//
//  Created by Squiffy on 11/11/14.
//  Copyright (c) 2014 squiffypwn. All rights reserved.
//

#include "libmasochist.h"

struct mach_header_64 *kernel_header = (struct mach_header_64 *)KERNEL_BASE;

kern_return_t
libmasochist_init() {
    
    /* No panic is better */
    if(!kernel_header)
        return KERN_FAILURE;
    
    return KERN_SUCCESS;
    
}
