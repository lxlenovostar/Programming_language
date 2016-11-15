/* include signal */
#include    "error.h"
#include	"lx_sock.h"

/*
 A pointer to a signal handling function is the second argument to the function, 
 as well as the return from the function.
 */
Sigfunc *
signal(int signo, Sigfunc *func)
{
	struct sigaction	act, oact;

	act.sa_handler = func;
	/*
     POSIX allows us to specify a set of signals that will be blocked when our signal handler 
     is called. Any signal that is blocked cannot be delivered to a process. We set the sa_mask
     member to the empty set, which means that no additional signals will be blocked while our 
     signal handler is running. POSIX guarantees that the signal being caught is always blocked 
     while its handler is executing.
     */
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (signo == SIGALRM) {
#ifdef	SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT;	/* SunOS 4.x */
#endif
	} else {
#ifdef	SA_RESTART
		act.sa_flags |= SA_RESTART;		/* SVR4, 44BSD */
#endif
	}
	if (sigaction(signo, &act, &oact) < 0)
		return(SIG_ERR);
	return(oact.sa_handler);
}
/* end signal */

Sigfunc *
Signal(int signo, Sigfunc *func)	/* for our signal() function */
{
	Sigfunc	*sigfunc;

	if ( (sigfunc = signal(signo, func)) == SIG_ERR)
		err_sys("signal error");
	return(sigfunc);
}
