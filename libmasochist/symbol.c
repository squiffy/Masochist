//
//  symbol.c
//  masochist
//
//  Created by Squiffy on 11/11/14.
//  Copyright (c) 2014 squiffypwn. All rights reserved.
//

#include <mach/mach_types.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <mach/mach_types.h>
#include <sys/systm.h>
#include <stdbool.h>
#include "libmasochist.h"
#include "symbol.h"

void *
find_symbol(const char *symbol) {
    
    struct load_command *lc = NULL;
    struct segment_command_64 *seg = NULL;
    struct segment_command_64 *linkedit = NULL;
    
    /* Lets get the first load command */
    
    lc = (struct load_command *)(sizeof(struct mach_header_64) + kernel_base);
    
    /* iterate through all of the load commands until we find __LINKEDIT */
    int i;
    for(i = 0; i < kernel_header->ncmds; i++) {
        
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
    for(i = 0; i < kernel_header->ncmds; i++) {
        
        if(lc->cmd == LC_SYMTAB) {
            
            lc_symtab = (struct symtab_command *)lc;
            lc = (struct load_command *)(sizeof(struct mach_header_64) + kernel_base);
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
            return (void *)nlist->n_value;
            
        }
        
        /* next nlist please */
        nlist = (struct nlist_64 *)(symbolTable + (i * sizeof(struct nlist_64)));
    }
    
    /* FAILURE */
    return NULL;
    
}