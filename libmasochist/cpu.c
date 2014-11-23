//
//  cpu.c
//  masochist
//
//  Created by Will Estes on 11/16/14.
//  Copyright (c) 2014 squiffypwn. All rights reserved.
//

#include <mach/mach_types.h>
#include <i386/proc_reg.h>
#include "cpu.h"

kern_return_t
disable_write_protection() {
    
    uintptr_t cr0 = get_cr0();
    cr0 &= ~CR0_WP;
    set_cr0(cr0);
    
    return KERN_SUCCESS;
    
}

kern_return_t
enable_write_protection() {
    
    uintptr_t cr0 = get_cr0();
    cr0 |= CR0_WP;
    set_cr0(cr0);
    
    return KERN_SUCCESS;
}
