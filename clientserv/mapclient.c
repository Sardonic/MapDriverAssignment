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
	cli_map_request_t req;
	char *ip_addr;

	/* Set up request */
	{
		/* set up the defaults */
		portno = DEFAULT_PORT;
		ip_addr = DEFAULT_IP;
		req.height = 0;
		req.width = 0;
		int opt;
		while ((opt = getopt (argc, argv, "i:w:h:")) != -1)
		{
		    switch(opt)
		    {
			case 'i':
				ip_addr = optarg;
			break;
			case 'w':
				req.width = atoi(optarg);
			break;
			case 'h':
				req.height = atoi(optarg);
			break;
			default:
				printf("Usage: the -i, -w, and -h options correspond to ip, width and height respectively.\n");
			break;
		    }
		}

	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		error("Could not create socket");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	if (inet_pton(AF_INET, ip_addr, &serv_addr.sin_addr) <= 0)
		error("inet_pton error");

	if (connect(sockfd,
			(struct sockaddr*)&serv_addr,
			sizeof(serv_addr)) < 0)
		error("connect() error");

	char msg[2];
	snprintf(msg, 2, "%c", 'M');
	n = write(sockfd, msg, 1);
	n = write(sockfd,&req,sizeof(req));
	if (n < 0) 
		error("ERROR writing to socket");

	close(sockfd);

}
