//
//  process.h
//  masochist
//
//  Created by Squiffy on 11/11/14.
//  Copyright (c) 2014 squiffypwn. All rights reserved.
//

#ifndef masochist_process_h
#define masochist_process_h

kern_return_t hideProcess(pid_t pid);
kern_return_t showProcess(pid_t pid);
kern_return_t changeProcessOwnership(uid_t uid, gid_t gid);


#endif
