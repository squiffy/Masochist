//
//  syscall.h
//  masochist
//
//  Created by Squiffy on 11/13/14.
//  Copyright (c) 2014 squiffypwn. All rights reserved.
//

#ifndef __masochist__syscall__
#define __masochist__syscall__

struct sysent *resolve_sysent();
kern_return_t test_hook();
#endif /* defined(__masochist__syscall__) */
