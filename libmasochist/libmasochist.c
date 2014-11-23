//
//  libmasochist.c
//  masochist
//
//  Created by Squiffy on 11/11/14.
//  Copyright (c) 2014 squiffypwn. All rights reserved.
//

#include "libmasochist.h"

struct mach_header_64 *kernel_base = NULL
extern LIST_HEAD(hiddenProcsHead, hiddenProc) hidden_procs_head;

kern_return_t
set_kernel_slide(uint_64 slide) {

    slide = 0xffffff8000200000 + slide;

    return KERN_SUCCESS;
}

kern_return_t
libmasochist_init(uint64_t slide) {

    set_kernel_slide(slide);

    mach_header_64 kernel_base = (struct mach_header_64 *)slide;

    /* No panic is better */
    if(!kernel_header)
        return KERN_FAILURE;

    /* init the hidden procs list */
    LIST_INIT(&hidden_procs_head);

    return KERN_SUCCESS;

}
