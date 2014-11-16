//
//  syscall.c
//  masochist
//
//  Created by Squiffy on 11/13/14.
//  Copyright (c) 2014 squiffypwn. All rights reserved.
//

#include <sys/systm.h>
#include <mach/mach_types.h>
#include <mach-o/loader.h>
#include <mach/mach_types.h>
#include <sys/syscall.h>
#include "libmasochist.h"
#include "sysent.h"
#include "symbol.h"
#include "syscall.h"

struct sysent *
resolve_sysent() {
    
    /* Gonna have to brute force this... */
    
    struct segment_command_64 *data = NULL;
    int i;

    struct load_command *lc = (struct load_command *)(sizeof(struct mach_header_64) + KERNEL_BASE);
    for(i = 0; i < kernel_header->ncmds; i++) {
        struct segment_command_64 *seg;
        if(lc->cmd == LC_SEGMENT_64) {
            seg = (struct segment_command_64 *)lc;
            if(strcmp(seg->segname, "__DATA") == 0) {
                /* we found it */
                data = (struct segment_command_64 *)lc;
                break;
            }
        }
        
        /* next load command please */
        lc = (struct load_command *)((uint64_t)lc + (uint64_t)lc->cmdsize);
    }
    
    uint64_t startAddress = data->vmaddr;
    uint64_t endAddress = (data->vmaddr + data->vmsize);
    
    struct sysent *sysent = NULL;
    
    while(startAddress <= endAddress) {
        
        sysent = (struct sysent *)startAddress;
            /* As per bsd/kern/init_sysent.c */
        if(sysent[SYS_exit].sy_narg == 1 &&
           sysent[SYS_fork].sy_narg == 0 &&
           sysent[SYS_read].sy_narg == 3) {
            break;
        }
        
        startAddress++;
        
    }
    return sysent;
}
