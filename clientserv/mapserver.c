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

int main(int argc, char* argv[])
{
	int sockfd, connfd, portno;
	socklen_t clilen;
	char buffer[256];
	cli_request_t cli_req;
	struct sockaddr_in serv_addr, cli_addr;
	int n;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		fatal(NULL);

	memset(&serv_addr, '0', sizeof(serv_addr));
	portno = DEFAULT_PORT;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
			sizeof(serv_addr)) < 0)
		fatal(NULL);

	listen(sockfd, 5);
	clilen = sizeof(cli_addr);

	connfd = accept(sockfd,
			(struct sockaddr *) &cli_addr,
			&clilen);
	if (connfd < 0)
		fatal(NULL);

	memset(&serv_addr, '0', sizeof(serv_addr));
	n = read(connfd, &cli_req, sizeof(cli_request_t));
	if (n < 0)
		fatal(NULL);

	/* printf("Here is the message: %s\n", buffer); */
	{
		printf("Request:\t%c\n", cli_req.cmd);
		printf("Width:\t%d\n", cli_req.width);
		printf("Height:\t%d\n", cli_req.height);
	}

	/* n = write(connfd, "I got your message", 18); */
	if (n < 0)
		fatal(NULL);

	close(connfd);
	close(sockfd);

	return 0;
}
