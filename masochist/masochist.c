//
//  masochist.c
//  masochist
//
//  Created by Squiffy on 11/6/14.
//  Copyright (c) 2014 squiffypwn. All rights reserved.
//

#include <mach/mach_types.h>

kern_return_t masochist_start(kmod_info_t * ki, void *d);
kern_return_t masochist_stop(kmod_info_t *ki, void *d);

kern_return_t masochist_start(kmod_info_t * ki, void *d)
{
    return KERN_SUCCESS;
}

kern_return_t masochist_stop(kmod_info_t *ki, void *d)
{
    return KERN_SUCCESS;
}
