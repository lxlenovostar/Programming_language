#include "../lib/lx_sock.h"
#include "../lib/error.h"

int
main(int argc, char **argv)
{
	int	i, maxi, listenfd, connfd, sockfd;
	int nready;
	ssize_t n;
	char buf[MAXLINE];	
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;
	struct pollfd client[OPEN_MAX];

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT);

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	client[0].fd = listenfd;
	client[0].events = POLLRDNORM;
	for (i = 1; i < OPEN_MAX; i++) {
		/* -1 indicates available entry */
		client[i].fd = -1;
	}
	/* max index into client[] array */
	maxi = 0;

	for ( ; ; ) {
		nready = Poll(client, maxi + 1, INFTIM);
		
		/* new client connection */
		if (client[0].revents & POLLRDNORM) {
			clilen = sizeof(cliaddr);
			connfd = Accept(listenfd, (SA *)&cliaddr, &clilen);

			for (i = 1; i < OPEN_MAX; i++)
				if (client[i].fd < 0) {
					/* save descriptor */
					client[i].fd = connfd; 
					break;
				}

			if (i == OPEN_MAX)
				err_quit("too many clients");

			client[i].events = POLLRDNORM;

			/* max index in client[] array */
			if (i > maxi)
				maxi = i;

			/* no more readable descriptors */
			if (--nready <= 0)
				continue;
		}

		/* check all clients for data */
		for (i = 1; i <= maxi; i++) {
			if ((sockfd = client[i].fd) < 0)
				continue;
			
			/* 
              The reason we check for POLLERR is because some implementations return 
			  this event when an RST is received for a connection.
             */
			if (client[i].revents & (POLLRDNORM | POLLERR)) {
				if ((n = read(sockfd, buf, MAXLINE)) < 0) {
					/* connection reset by client */
					if (errno == ECONNRESET) {
						Close(sockfd);
						client[i].fd = -1;
					} else 
						err_sys("read error");
				} else if (n == 0) {
					/* connection closed by client */
					Close(sockfd);
					client[i].fd = -1;
				} else 
					Writen(sockfd, buf, n);

				/* no more readable descriptors */
				if (--nready <= 0)
					break;
			}
		}
	}
}
