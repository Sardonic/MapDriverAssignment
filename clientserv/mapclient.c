/*our includes */
#include "mapserver.h"

/* needed includes*/
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
	char *ip_addr;

	/* Set up request */
	{
		/* set up the defaults */
		req.cmd = MAP_REQ_CHAR;
		portno = DEFAULT_PORT;
		ip_addr = DEFAULT_IP;
		req.height = 0;
		req.width = 0;
		
		/*switch case magic to properly set things based on user */
		switch(argc)
		{
			case 5:
				req.height = atoi(argv[4]);
				req.width = atoi(argv[3]);
			case 3:
				ip_addr = argv[2];
			case 2:
				portno = atoi(argv[1]);
				break;
		}
		

	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		error("Could not create socket");

	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	if (inet_pton(AF_INET, ip_addr, &serv_addr.sin_addr) <= 0)
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
