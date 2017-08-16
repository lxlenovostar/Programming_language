/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2017.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Lesser General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the files COPYING.lgpl-v3 and COPYING.gpl-v3 for details.           *
\*************************************************************************/

/* Listing 37-2 */

#include <sys/stat.h>
#include <fcntl.h>
#include "become_daemon.h"
#include "tlpi_hdr.h"

int                                     /* Returns 0 on success, -1 on error */
becomeDaemon(int flags)
{
    int maxfd, fd;

	/* 
	 * Perform a fork(), after which the parent exits and the child continues. 
	 * (As a consequence, the daemon becomes a child of the init process.) 
	 * This step is done for two reasons:
	 *
	 * 1.Assuming the daemon was started from the command line, the parent’s
	 * termination is noticed by the shell, which then displays another shell prompt
	 * and leaves the child to continue in the background.
	 * 2.The child process is guaranteed not to be a process group leader, since it
	 * inherited its process group ID from its parent and obtained its own unique
	 * process ID, which differs from the inherited process group ID. This is
	 * required in order to be able to successfully perform the next step.
	 * */
    switch (fork()) {                   /* Become background process */
    case -1: return -1;
    case 0:  break;                     /* Child falls through... */
    default: _exit(EXIT_SUCCESS);       /* while parent terminates */
    }

	/*
	 * The child process calls setsid() to start a new session and free
	 * itself of any association with a controlling terminal.
	 * */
    if (setsid() == -1)                 /* Become leader of new session */
        return -1;

	/*
	 * O_NOCTTY: Don’t let pathname become the controlling terminal
	 *
	 * If the daemon never opens any terminal devices thereafter, then we don’t need
	 * to worry about the daemon reacquiring a controlling terminal. If the daemon
	 * might later open a terminal device, then we must take steps to ensure that the
	 * device does not become the controlling terminal. We can do this in two ways:
	 * 1.Specify the O_NOCTTY flag on any open() that may apply to a terminal device.
	 * 2.Alternatively, and more simply ,perform a second fork() after the setsid() 
	 * call, and again have the parent exit and the (grand)child continue. This ensures 
	 * that the child is not the session leader, and thus, according to the System V 
	 * conventions for the acquisition of a controlling terminal (which Linux follows), 
	 * the process can never reacquire a controlling terminal
	 * */
    switch (fork()) {                   /* Ensure we are not session leader */
    case -1: return -1;
    case 0:  break;
    default: _exit(EXIT_SUCCESS);
    }

	/*
	 * Clear the process umask, to ensure that, when the daemon creates
	 * files and directories, they have the requested permissions.
	 * */
    if (!(flags & BD_NO_UMASK0))
        umask(0);                       /* Clear file mode creation mask */

	/*
	 * Change the process’s current working directory, typically to the r
	 * oot directory (/). This is necessary because a daemon usually runs 
	 * until system shutdown; if the daemon’s current working directory i
	 * is on a file system other than the one containing /, then that file 
	 * system can’t be unmounted (Section 14.8.2). Alternatively, the daemon c
	 * an change its working directory to a location where it does its job or 
	 * a location defined in its configuration file, as long as we know that 
	 * the file system containing this directory never needs to be unmounted. 
	 * For example, cron places itself in /var/spool/cron.
	 * */
    if (!(flags & BD_NO_CHDIR))
        chdir("/");                     /* Change to root directory */

	/*
	 * Close all open file descriptors that the daemon has inherited from 
	 * its parent.(A daemon may need to keep certain inherited file descriptors 
	 * open, so this step is optional, or open to variation.) This is done for 
	 * a variety of reasons. Since the daemon has lost its controlling terminal 
	 * and is running in the background, it makes no sense for the daemon to keep 
	 * file descriptors 0, 1, and 2 open if these refer to the terminal. Furthermore, 
	 * we can’t unmount any file systems on which the long-lived daemon holds files o
	 * pen. And, as usual, we should close unused open file descriptors because file 
	 * descriptors are a finite resource.
	 * */
    if (!(flags & BD_NO_CLOSE_FILES)) { /* Close all open files */
        maxfd = sysconf(_SC_OPEN_MAX);
        if (maxfd == -1)                /* Limit is indeterminate... */
            maxfd = BD_MAX_CLOSE;       /* so take a guess */

        for (fd = 0; fd < maxfd; fd++)
            close(fd);
    }

	/*
	 * /dev/null is a virtual device that always discards the data written to it. 
	 * When we want to eliminate the standard output or error of a shell command, 
	 * we can redirect it to this file. Reads from this device always return 
	 * end-of-file.
	 *
	 * After having closed file descriptors 0, 1, and 2, a daemon normally opens 
	 * /dev/null and uses dup2() (or similar) to make all those descriptors refer 
	 * to this device.
	 * This is done for two reasons:
	 * – It ensures that if the daemon calls library functions that perform I/O on
	 * these descriptors, those functions won’t unexpectedly fail.
	 * – It prevents the possibility that the daemon later opens a file using descriptor
	 * 1 or 2, which is then written to—and thus corrupted—by a library function
	 * that expects to treat these descriptors as standard output and standard error.
	 * */
    if (!(flags & BD_NO_REOPEN_STD_FDS)) {
        close(STDIN_FILENO);            /* Reopen standard fd's to /dev/null */

        fd = open("/dev/null", O_RDWR);

        if (fd != STDIN_FILENO)         /* 'fd' should be 0 */
            return -1;
        if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO)
            return -1;
        if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO)
            return -1;
    }

    return 0;
}
