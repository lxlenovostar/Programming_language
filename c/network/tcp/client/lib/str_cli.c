#include	"lx_sock.h"
#include 	"error.h"

void
str_cli(FILE *fp, int sockfd)
{
	int maxfdp1;
	fd_set rset;
	char sendline[MAXLINE], recvline[MAXLINE];

	FD_ZERO(&rset);
	
	for ( ; ;) {
		FD_SET(fileno(fp), &rset);
		FD_SET(sockfd, &rset);
		maxfdp1 = max(fileno(fp), sockfd) + 1;

		Select(maxfdp1, &rset, NULL, NULL, NULL);

		/* socket is readable */
		if (FD_ISSET(sockfd, &rset)) {
			if (Readline(sockfd, recvline, MAXLINE) == 0)
				err_quit("str_cli: server terminated prematurely");
			Fputs(recvline, stdout);
		}

		/* input is readable */
		if (FD_ISSET(fileno(fp), &rset)) {
			if (Fgets(sendline, MAXLINE, fp) == NULL)
				return; /* all done */

			Writen(sockfd, sendline, strlen(sendline));
		}
	}

	/*
	while (Fgets(sendline, MAXLINE, fp) != NULL) {

		Writen(sockfd, sendline, strlen(sendline));

		if (Readline(sockfd, recvline, MAXLINE) == 0)
			err_quit("str_cli: server terminated prematurely");

		Fputs(recvline, stdout);
	}
	*/
}
