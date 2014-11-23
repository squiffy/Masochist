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
#include "sysproto.h"
#include "libmasochist.h"
#include "symbol.h"
#include "syscall.h"
#include "cpu.h"

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

int mkdir_hook(struct proc *process, struct mkdir_args *args, int *unused) {
    
    printf("Directory: %s\n", (char *)args->path);
    
    return 0;
    
}

kern_return_t
test_hook() {
    
    struct sysent *sysent = resolve_sysent();
    
    disable_write_protection();
    sysent[SYS_mkdir].sy_call = (sy_call_t *)mkdir_hook;
    enable_write_protection();
    
    return KERN_SUCCESS;
    
}