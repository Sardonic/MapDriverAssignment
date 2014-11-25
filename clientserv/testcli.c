#include "mapserver.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <errno.h>
#include <arpa/inet.h>

void error(const char *msg)
{
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	cli_request_t req;
	char recvBuff[256];

	/* Set up request */
	{
		req.cmd = MAP_REQ_CHAR;
		req.width = 128;
		req.height = 128;
	}

	memset(recvBuff, '0', sizeof(recvBuff));

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		error("Could not create socket");

	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(DEFAULT_PORT);
	if (inet_pton(AF_INET, DEFAULT_IP, &serv_addr.sin_addr) <= 0)
		error("inet_pton error");

	if (connect(sockfd,
			(struct sockaddr*)&serv_addr,
			sizeof(serv_addr)) < 0)
		error("connect() error");

	n = write(sockfd,&req,sizeof(req));
	if (n < 0) 
		error("ERROR writing to socket");

	close(sockfd);

}
