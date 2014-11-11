//
//  libmasochist.c
//  masochist
//
//  Created by Squiffy on 11/11/14.
//  Copyright (c) 2014 squiffypwn. All rights reserved.
//

#include "libmasochist.h"

kern_return_t
libmasochist_init() {
    
    /* Get the mach header of the kernel */
    kernel_header = (struct mach_header_64 *)KERNEL_BASE;
    
    /* No panic is better */
    if(!kernel_header)
        return KERN_FAILURE;
    
    return KERN_SUCCESS;
    
}
