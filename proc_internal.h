/*
 * Copyright (c) 2000-2010 Apple Inc. All rights reserved.
 *
 * @APPLE_OSREFERENCE_LICENSE_HEADER_START@
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. The rights granted to you under the License
 * may not be used to create, or enable the creation or redistribution of,
 * unlawful or unlicensed copies of an Apple operating system, or to
 * circumvent, violate, or enable the circumvention or violation of, any
 * terms of an Apple operating system software license agreement.
 *
 * Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this file.
 *
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 *
 * @APPLE_OSREFERENCE_LICENSE_HEADER_END@
 */
/* Copyright (c) 1995, 1997 Apple Computer, Inc. All Rights Reserved */
/*-
 * Copyright (c) 1986, 1989, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)proc_internal.h	8.15 (Berkeley) 5/19/95
 */
/*
 * NOTICE: This file was modified by SPARTA, Inc. in 2005 to introduce
 * support for mandatory and extensible security protections.  This notice
 * is included in support of clause 2.2 (b) of the Apple Public License,
 * Version 2.0.
 */

#ifndef _SYS_PROC_INTERNAL_H_
#define	_SYS_PROC_INTERNAL_H_

#include <libkern/OSAtomic.h>
#include <sys/proc.h>
#if PSYNCH
#include <kern/thread_call.h>
#endif /* PSYNCH */

#if DEBUG
#define	__PROC_INTERNAL_DEBUG 1
#endif

struct lck_spin_t {
    unsigned long    opaque[10];
};

struct lck_mtx_t {
    union {
        struct {
            volatile uintptr_t		lck_mtxd_owner;
            union {
                struct {
                    volatile uint32_t
                lck_mtxd_waiters:16,
                lck_mtxd_pri:8,
                lck_mtxd_ilocked:1,
                lck_mtxd_mlocked:1,
                lck_mtxd_promoted:1,
                lck_mtxd_spin:1,
                lck_mtxd_is_ext:1,
                lck_mtxd_pad3:3;
                };
                uint32_t	lck_mtxd_state;
            };
            uint32_t			lck_mtxd_pad32;
        } lck_mtxd;
        struct {
            struct _lck_mtx_ext_		*lck_mtxi_ptr;
            uint32_t			lck_mtxi_tag;
            uint32_t			lck_mtxi_pad32;
        } lck_mtxi;
    } lck_mtx_sw;
};

/*
 * The short form for various locks that protect fields in the data structures.
 * PL = Process Lock
 * PGL = Process Group Lock
 * PFDL = Process File Desc Lock
 * PSL = Process Spin Lock
 * PPL = Parent Process Lock (planed for later usage)
 * LL = List Lock
 * SL = Session Lock
 */
struct label;

/*
 * Added by SPARTA, Inc.
 */
/*
 * Login context.
 */
struct lctx {
    LIST_ENTRY(lctx) lc_list;	/* List of all login contexts. */
    LIST_HEAD(, proc) lc_members;	/* Pointer to lc members. */
    int		lc_mc;		/* Member Count. */
    pid_t		lc_id;		/* Login context ID. */
    struct lck_mtx_t	lc_mtx;		/* Mutex to protect members */
    
    struct label	*lc_label;	/* Login context MAC label. */
};

/*
 * One structure allocated per session.
 */
struct	session {
    int			s_count;		/* Ref cnt; pgrps in session. (LL) */
    struct	proc *		s_leader;		/* Session leader.(static) */
    struct	vnode *		s_ttyvp;		/* Vnode of controlling terminal.(SL) */
    int			s_ttyvid;		/* Vnode id of the controlling terminal (SL) */
    struct	tty *		s_ttyp;			/* Controlling terminal. (SL + ttyvp != NULL) */
    pid_t			s_ttypgrpid;		/* tty's pgrp id */
    pid_t			s_sid;			/* Session ID (static) */
    char			s_login[MAXLOGNAME];	/* Setlogin() name.(SL) */
    int			s_flags;		/* Session flags (s_mlock)  */
    LIST_ENTRY(session)	s_hash;			/* Hash chain.(LL) */
    struct lck_mtx_t		s_mlock;		/* mutex lock to protect session */
    int			s_listflags;
};

#define SESSION_NULL (struct session *)0

/*
 * accessor for s_ttyp which treats it as invalid if s_ttyvp is not valid;
 * note that s_ttyp is not a reference in the session structre, so it can
 * become invalid out from under the session if the device is closed, without
 * this protection.  We can't safely make it into a reference without reflexive
 * close notification of tty devices through cdevsw[].
 *
 * NB:	<sys/tty.h> is not in scope and there is not typedef type enforcement,
 *	or '0' below would be 'TTY_NULL'.
 */
#define SESSION_TP(sp)	(((sp)->s_ttyvp != 0) ? (sp)->s_ttyp : 0)

/*
 * Session flags; used to tunnel information to lower layers and line
 * disciplines, etc.
 */
#define	S_DEFAULT	0x00000000	/* No flags set */
#define	S_NOCTTY	0x00000001	/* Do not associate controlling tty */
#define S_CTTYREF	0x00000010	/* vnode ref taken by cttyopen */


#define S_LIST_TERM	1		/* marked for termination */
#define	S_LIST_DEAD	2		/* already dead */
/*
 * One structure allocated per process group.
 */
struct	pgrp {
    LIST_ENTRY(pgrp)	pg_hash;	/* Hash chain. (LL) */
    LIST_HEAD(, proc) 	pg_members;	/* Pointer to pgrp members. (PGL) */
    struct	session *	pg_session;	/* Pointer to session. (LL ) */
    pid_t			pg_id;		/* Pgrp id. (static) */
    int			pg_jobc;	/* # procs qualifying pgrp for job control (PGL) */
    int			pg_membercnt;	/* Number of processes in the pgrocess group (PGL) */
    int			pg_refcount;	/* number of current iterators (LL) */
    unsigned int		pg_listflags;	/* (LL) */
    struct lck_mtx_t		pg_mlock;	/* mutex lock to protect pgrp */
};

#define PGRP_FLAG_TERMINATE 	1
#define PGRP_FLAG_WAITTERMINATE 2
#define PGRP_FLAG_DEAD 		4
#define PGRP_FLAG_ITERABEGIN 	8
#define PGRP_FLAG_ITERWAIT 	0x10

#define PGRP_NULL (struct pgrp *)0
struct proc;

#define PROC_NULL (struct proc *)0

#define PROC_UPDATE_CREDS_ONPROC(p) { \
p->p_uid =  kauth_cred_getuid(p->p_ucred); \
p->p_gid =  kauth_cred_getgid(p->p_ucred); \
p->p_ruid =  kauth_cred_getruid(p->p_ucred); \
p->p_rgid =  kauth_cred_getrgid(p->p_ucred); \
p->p_svuid =  kauth_cred_getsvuid(p->p_ucred); \
p->p_svgid =  kauth_cred_getsvgid(p->p_ucred); \
}
/*
 * Description of a process.
 *
 * This structure contains the information needed to manage a thread of
 * control, known in UN*X as a process; it has references to substructures
 * containing descriptions of things that the process uses, but may share
 * with related processes.  The process structure and the substructures
 * are always addressible except for those marked "(PROC ONLY)" below,
 * which might be addressible only on a processor on which the process
 * is running.
 */
struct	proc {
    LIST_ENTRY(proc) p_list;		/* List of all processes. */
    
    pid_t		p_pid;			/* Process identifier. (static)*/
    void * 		task;			/* corresponding task (static)*/
    struct	proc *	p_pptr;		 	/* Pointer to parent process.(LL) */
    pid_t		p_ppid;			/* process's parent pid number */
    pid_t		p_pgrpid;		/* process group id of the process (LL)*/
    uid_t		p_uid;
    gid_t		p_gid;
    uid_t		p_ruid;
    gid_t		p_rgid;
    uid_t		p_svuid;
    gid_t		p_svgid;
    uint64_t	p_uniqueid;		/* process unique ID - incremented on fork/spawn/vfork, remains same across exec. */
    uint64_t	p_puniqueid;		/* parent's unique ID - set on fork/spawn/vfork, doesn't change if reparented. */
    
    struct lck_mtx_t 	p_mlock;		/* mutex lock for proc */
    
    char		p_stat;			/* S* process status. (PL)*/
    char		p_shutdownstate;
    char		p_kdebug;		/* P_KDEBUG eq (CC)*/
    char		p_btrace;		/* P_BTRACE eq (CC)*/
    
    LIST_ENTRY(proc) p_pglist;		/* List of processes in pgrp.(PGL) */
    LIST_ENTRY(proc) p_sibling;		/* List of sibling processes. (LL)*/
    LIST_HEAD(, proc) p_children;		/* Pointer to list of children. (LL)*/
    TAILQ_HEAD( , uthread) p_uthlist; 	/* List of uthreads  (PL) */
    
    LIST_ENTRY(proc) p_hash;		/* Hash chain. (LL)*/
    TAILQ_HEAD( ,eventqelt) p_evlist;	/* (PL) */
    
    struct lck_mtx_t	p_fdmlock;		/* proc lock to protect fdesc */
    
    /* substructures: */
    kauth_cred_t	p_ucred;		/* Process owner's identity. (PL) */
    struct	filedesc *p_fd;			/* Ptr to open files structure. (PFDL) */
    struct	pstats *p_stats;		/* Accounting/statistics (PL). */
    struct	plimit *p_limit;		/* Process limits.(PL) */
    
    struct	sigacts *p_sigacts;		/* Signal actions, state (PL) */
    int		p_siglist;		/* signals captured back from threads */
    struct lck_spin_t	p_slock;		/* spin lock for itimer/profil protection */
    
#define	p_rlimit	p_limit->pl_rlimit
    
    struct	plimit *p_olimit;		/* old process limits  - not inherited by child  (PL) */
    unsigned int	p_flag;			/* P_* flags. (atomic bit ops) */
    unsigned int	p_lflag;		/* local flags  (PL) */
    unsigned int	p_listflag;		/* list flags (LL) */
    unsigned int	p_ladvflag;		/* local adv flags (atomic) */
    int		p_refcount;		/* number of outstanding users(LL) */
    int		p_childrencnt;		/* children holding ref on parent (LL) */
    int		p_parentref;		/* children lookup ref on parent (LL) */
    
    pid_t		p_oppid;	 	/* Save parent pid during ptrace. XXX */
    u_int		p_xstat;		/* Exit status for wait; also stop signal. */
    
    
    struct	itimerval p_realtimer;		/* Alarm timer. (PSL) */
    struct	timeval p_rtime;		/* Real time.(PSL)  */
    struct	itimerval p_vtimer_user;	/* Virtual timers.(PSL)  */
    struct	itimerval p_vtimer_prof;	/* (PSL) */
    
    struct	timeval	p_rlim_cpu;		/* Remaining rlim cpu value.(PSL) */
    int		p_debugger;		/*  NU 1: can exec set-bit programs if suser */
    boolean_t	sigwait;	/* indication to suspend (PL) */
    void	*sigwait_thread;	/* 'thread' holding sigwait(PL)  */
    void	*exit_thread;		/* Which thread is exiting(PL)  */
    int	p_vforkcnt;		/* number of outstanding vforks(PL)  */
    void *  p_vforkact;     	/* activation running this vfork proc)(static)  */
    int	p_fpdrainwait;		/* (PFDL) */
    pid_t	p_contproc;	/* last PID to send us a SIGCONT (PL) */
    
    /* Following fields are info from SIGCHLD (PL) */
    pid_t	si_pid;			/* (PL) */
    u_int   si_status;		/* (PL) */
    u_int	si_code;		/* (PL) */
    uid_t	si_uid;			/* (PL) */
    
    void * vm_shm;			/* (SYSV SHM Lock) for sysV shared memory */
    
    user_addr_t			p_dtrace_argv;			/* (write once, read only after that) */
    user_addr_t			p_dtrace_envp;			/* (write once, read only after that) */
    struct lck_mtx_t			p_dtrace_sprlock;		/* sun proc lock emulation */
    int				p_dtrace_probes;		/* (PL) are there probes for this proc? */
    u_int				p_dtrace_count;			/* (sprlock) number of DTrace tracepoints */
    uint8_t                         p_dtrace_stop;                  /* indicates a DTrace-desired stop */
    struct dtrace_ptss_page*	p_dtrace_ptss_pages;		/* (sprlock) list of user ptss pages */
    struct dtrace_ptss_page_entry*	p_dtrace_ptss_free_list;	/* (atomic) list of individual ptss entries */
    struct dtrace_helpers*		p_dtrace_helpers;		/* (dtrace_lock) DTrace per-proc private */
    struct dof_ioctl_data*		p_dtrace_lazy_dofs;		/* (sprlock) unloaded dof_helper_t's */
    
    /* XXXXXXXXXXXXX BCOPY'ed on fork XXXXXXXXXXXXXXXX */
    /* The following fields are all copied upon creation in fork. */
#define	p_startcopy	p_argslen
    
    u_int	p_argslen;	 /* Length of process arguments. */
    int  	p_argc;			/* saved argc for sysctl_procargs() */
    user_addr_t user_stack;		/* where user stack was allocated */
    struct	vnode *p_textvp;	/* Vnode of executable. */
    off_t	p_textoff;		/* offset in executable vnode */
    
    sigset_t p_sigmask;		/* DEPRECATED */
    sigset_t p_sigignore;	/* Signals being ignored. (PL) */
    sigset_t p_sigcatch;	/* Signals being caught by user.(PL)  */
    
    u_char	p_priority;	/* (NU) Process priority. */
    u_char	p_resv0;	/* (NU) User-priority based on p_cpu and p_nice. */
    char	p_nice;		/* Process "nice" value.(PL) */
    u_char	p_resv1;	/* (NU) User-priority based on p_cpu and p_nice. */
    
    int	p_mac_enforce;			/* MAC policy enforcement control */
    
    char	p_comm[MAXCOMLEN+1];
    char	p_name[(2*MAXCOMLEN)+1];	/* PL */
    
    struct 	pgrp *p_pgrp;	/* Pointer to process group. (LL) */
    uint32_t	p_csflags;	/* flags for codesign (PL) */
    uint32_t	p_pcaction;	/* action  for process control on starvation */
    uint8_t p_uuid[16];		/* from LC_UUID load command */
    
    /*
     * CPU type and subtype of binary slice executed in
     * this process.  Protected by proc lock.
     */
    cpu_type_t	p_cputype;
    cpu_subtype_t	p_cpusubtype;
    
    /* End area that is copied on creation. */
    /* XXXXXXXXXXXXX End of BCOPY'ed on fork (AIOLOCK)XXXXXXXXXXXXXXXX */
#define	p_endcopy	p_aio_total_count
    int		p_aio_total_count;		/* all allocated AIO requests for this proc */
    int		p_aio_active_count;		/* all unfinished AIO requests for this proc */
    TAILQ_HEAD( , aio_workq_entry ) p_aio_activeq; 	/* active async IO requests */
    TAILQ_HEAD( , aio_workq_entry ) p_aio_doneq;	/* completed async IO requests */
    
    //struct klist p_klist;  /* knote list (PL ?)*/
    
    struct	rusage_superset *p_ru;	/* Exit information. (PL) */
    int		p_sigwaitcnt;
    thread_t 	p_signalholder;
    thread_t 	p_transholder;
    
    /* DEPRECATE following field  */
    u_short	p_acflag;	/* Accounting flags. */
    volatile u_short p_vfs_iopolicy;	/* VFS iopolicy flags. (atomic bit ops) */
    
    struct lctx *p_lctx;		/* Pointer to login context. */
    LIST_ENTRY(proc) p_lclist;	/* List of processes in lctx. */
    user_addr_t 	p_threadstart;		/* pthread start fn */
    user_addr_t 	p_wqthread;		/* pthread workqueue fn */
    int 	p_pthsize;			/* pthread size */
    uint32_t	p_pth_tsd_offset;	/* offset from pthread_t to TSD for new threads */
    user_addr_t	p_targconc;		/* target concurrency ptr */
    user_addr_t	p_stack_addr_hint;	/* stack allocation hint for wq threads */
    void * 	p_wqptr;			/* workq ptr */
    int 	p_wqsize;			/* allocated size */
    boolean_t       p_wqiniting;            /* semaphore to serialze wq_open */
    struct lck_spin_t	p_wqlock;		/* lock to protect work queue */
    struct  timeval p_start;        	/* starting time */
    void *	p_rcall;
    int		p_ractive;
    int	p_idversion;		/* version of process identity */
    void *	p_pthhash;			/* pthread waitqueue hash */
    volatile uint64_t was_throttled __attribute__((aligned(8))); /* Counter for number of throttled I/Os */
    volatile uint64_t did_throttle __attribute__((aligned(8)));  /* Counter for number of I/Os this proc throttled */
    
#if DIAGNOSTIC
    unsigned int p_fdlock_pc[4];
    unsigned int p_fdunlock_pc[4];
#if SIGNAL_DEBUG
    unsigned int lockpc[8];
    unsigned int unlockpc[8];
#endif /* SIGNAL_DEBUG */
#endif /* DIAGNOSTIC */
    uint64_t	p_dispatchqueue_offset;
    uint64_t	p_dispatchqueue_serialno_offset;
};
#endif	/* !_SYS_PROC_INTERNAL_H_ */

struct proclist {
    struct proc *lh_first;
};