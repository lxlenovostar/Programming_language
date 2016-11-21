#include "../lib/lx_sock.h"
#include "../lib/error.h"

int
main(int argc, char **argv)
{
	int	i, maxi, maxfd, listenfd, connfd, sockfd;
	int nready, client[FD_SETSIZE];
	ssize_t n;
	fd_set rset, allset;
	char buf[MAXLEN];	
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT);

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	//Signal(SIGCHLD, sig_chld);	/* must call waitpid() */
	
	maxfd = listenfd;	/* initialize */
	maxi = -1;	        /* index into clinet[] array */
	for (i = 0; i < FD_SETSIZE; i++)
		client[i] = -1; 	/* -1 indicates available entry */
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);

	for ( ; ; ) {
		rset = allset; 	/* structure assignment */
		nready = Select(maxfd + 1, &rset, NULL, NULL, NULL);

		/* new client connection */
		if (FD_ISSET(listenfd, &rset)) {
			clilen = sizeof(cliaddr);
			connfd = Accept(listenfd, (SA *)&cliaddr, &clilen);

			for (i = 0; i < FD_SETSIZE; i++)
				if (client[i] < 0) {
					/* save descriptor */
					client[i] = connfd;
					break;
				}

			if (i == FD_SETSIZE)
				err_quit("too many clients");

			/* add new descriptor to set */
			FD_SET(connfd, &allset);

			/* for select */
			if (connfd > maxfd)
				maxfd = connfd; 	

			/* max index in client[] array */
			if (i > maxi)
				maxi = i;

			/* no more readable descriptors */
			if (--nready <= 0)
				continue;
		}

		/* check all clients for data */
		for (i = 0; i <= maxi; i++) {
			if ((sockfd = client[i]) < 0)
				continue;
			if (FD_ISSET(sockfd, &rset)) {
				/* connection closed by client */
				if ((n = Read(sockfd, buf, MAXLEN)) == 0) {
					Close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i] = -1;
				} else {
					Writen(sockfd, buf, n);
				}

				/* no more readable descriptors */
				if (--nready <= 0)
					break;
			}
		}

	}
}
