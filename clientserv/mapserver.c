#include "mapserver.h"

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>


static void fatal(const char* msg)
{
	perror(msg);
	exit(1);
}

int main(void)
{
	int sockfd;
	int connfd;
	socklen_t clilen;
	cli_request_t cli_req;
	struct sockaddr_in serv_addr, cli_addr;
	int n;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		fatal(NULL);

	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(DEFAULT_PORT);
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
				sizeof(serv_addr)) < 0)
		fatal(NULL);

	listen(sockfd, 5);
	clilen = sizeof(cli_addr);

	while (1)
	{
		connfd = accept(sockfd,
				(struct sockaddr *) &cli_addr,
				&clilen);
		if (connfd < 0)
			fatal(NULL);

		memset(&serv_addr, '0', sizeof(serv_addr));
		n = read(connfd, &cli_req, sizeof(cli_request_t));
		if (n < 0)
			fatal(NULL);

		/* Print request info */
		{
			printf("Request:\t%c\n", cli_req.cmd);
			if (cli_req.cmd != 'M')
			{
				fprintf(stderr, "Request %c is invalid",
						cli_req.cmd);
			}
			else
			{
				if (cli_req.width == 0)
				{
					printf("Width is 0. Will decide map size.\n");
				}
				else
				{
					printf("Width:\t\t%d\n", cli_req.width);
					printf("Height:\t\t%d\n", cli_req.height);
				}
			}
		}



		close(connfd);
		sleep(1); /* I mean, how often do we get requests, really? */
	}

	return 0;
}
