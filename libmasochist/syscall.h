//
//  syscall.h
//  masochist
//
//  Created by Squiffy on 11/13/14.
//  Copyright (c) 2014 squiffypwn. All rights reserved.
//

#ifndef __masochist__syscall__
#define __masochist__syscall__

kern_return_t init_sysent();
struct sysent *resolve_sysent();
kern_return_t hook_system_call(void *function_ptr, unsigned int syscall);
kern_return_t orig_system_call(unsigned int syscall, struct proc *proc, void *a1, int *a2);

#endif /* defined(__masochist__syscall__) */
