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
#include <sys/malloc.h>
#include <IOKit/IOLib.h>
#include "sysproto.h"
#include "libmasochist.h"
#include "symbol.h"
#include "syscall.h"
#include "cpu.h"

struct sysent *sysent = NULL;
struct sysent *sysent_copy = NULL;

typedef	int32_t	sy_call_t(struct proc *, void *, int *);
typedef	void	sy_munge_t(void *);

struct sysent {		/* system call table */
    sy_call_t	*sy_call;	/* implementing function */
    sy_munge_t	*sy_arg_munge32; /* system call arguments munger for 32-bit process */
    int32_t		sy_return_type; /* system call return types */
    int16_t		sy_narg;	/* number of args */
    uint16_t	sy_arg_bytes;	/* Total size of arguments in bytes for
                                 * 32-bit system calls
                                 */
};

struct sysent *
resolve_sysent() {
    
    /* Gonna have to brute force this... */
    
    struct segment_command_64 *data = NULL;
    int i;

    struct load_command *lc = (struct load_command *)(sizeof(struct mach_header_64) + kernel_base);
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

kern_return_t
init_sysent() {
    
    
    uint64_t nsysent = *(uint64_t *)find_symbol("_nsysent");
    sysent = resolve_sysent();
    size_t size = nsysent*sizeof(struct sysent);
    sysent_copy = IOMalloc(size);
    
    if(sysent_copy == NULL)
        return KERN_FAILURE;
    
    memcpy(sysent_copy, sysent, size);

    return KERN_SUCCESS;
    
}

kern_return_t
hook_system_call(void *function_ptr, unsigned int syscall) {
    
    uint64_t nsysent = *(uint64_t *)find_symbol("_nsysent");
    
    if(syscall > (nsysent - 1))
        return KERN_FAILURE;
    
    disable_write_protection();
    sysent[syscall].sy_call = (sy_call_t *)function_ptr;
    enable_write_protection();
    
    return KERN_SUCCESS;
    
}

int32_t
orig_system_call(unsigned int syscall, struct proc *proc, void *a1, int *a2) {
    
    uint64_t nsysent = *(uint64_t *)find_symbol("_nsysent");
    
    if(syscall > (nsysent -1))
        return KERN_FAILURE;
    
    return sysent_copy[syscall].sy_call(proc, a1, a2);
    
    
}
