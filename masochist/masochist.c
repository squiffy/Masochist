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

void *
find_symbol(const char *symbol, struct mach_header_64 *header) {
    
    struct load_command *lc = NULL;
    struct segment_command_64 *seg = NULL;
    struct segment_command_64 *linkedit = NULL;
    
    /* Lets get the first load command */
    
    lc = (struct load_command *)(sizeof(struct mach_header_64) + KERNEL_BASE);
    
    /* iterate through all of the load commands until we find __LINKEDIT */
    int i;
    for(i = 0; i < header->ncmds; i++) {
        
        if(lc->cmd == LC_SEGMENT_64) {
            
            seg = (struct segment_command_64 *)lc;
            
            if(strcmp(seg->segname, "__LINKEDIT") == 0) {
                
                /* we found it */
                linkedit = (struct segment_command_64 *)lc;
                break;
                
            }
        }
        
        /* next load command please */
        lc = (struct load_command *)((uint64_t)lc + (uint64_t)lc->cmdsize);
    }
    
    if(linkedit == NULL) {
        
        /* We didn't find it */
        printf("__LINKEDIT doesn't exit.\n");
        return 0;
        
    }
    
    /* iterate through load commands again and look for LC_SYMTAB */
    
    struct symtab_command *lc_symtab;
    for(i = 0; i < header->ncmds; i++) {
        
        if(lc->cmd == LC_SYMTAB) {
            
            lc_symtab = (struct symtab_command *)lc;
            lc = (struct load_command *)(sizeof(struct mach_header_64) + KERNEL_BASE);
            break;
            
        }
        
        lc = (struct load_command *)((uint64_t)lc + (uint64_t)lc->cmdsize);
        
    }
    
    /* Calculate the address to the symbol table and the string table */
    
    uint64_t symbolTable = (linkedit->vmaddr + (lc_symtab->symoff) - linkedit->fileoff);
    uint64_t stringTable = (linkedit->vmaddr + (lc_symtab->stroff - linkedit->fileoff));
    
    struct nlist_64 *nlist = (struct nlist_64 *)symbolTable;
    
    /* Iterate through the symbol table until we find what we're lookig for */
    
    for(i = 0; i < lc_symtab->nsyms; i++) {
        
        if(!strcmp(symbol, (char *)(nlist->n_un.n_strx + stringTable))) {
            
            /* Found it! Return a pointer to the symbol */
            return (struct proclist *)(nlist->n_value);
            
        }
        
        /* next nlist please */
        nlist = (struct nlist_64 *)(symbolTable + (i * sizeof(struct nlist_64)));
    }
    
    /* FAILURE */
    return 0;
    
}

kern_return_t
masochist_start(kmod_info_t * ki, void *d) {
    
    /* Get the mach header of the kernel */
    struct mach_header_64 *kernel_header = (struct mach_header_64 *)KERNEL_BASE;
    
    /* Lets find some symbols */
    struct proclist *allproc = find_symbol("_allproc", kernel_header);
    printf("_allproc = 0x%p\n", allproc);
    
    return KERN_SUCCESS;
}

kern_return_t
masochist_stop(kmod_info_t *ki, void *d)
{
    return KERN_SUCCESS;
}
