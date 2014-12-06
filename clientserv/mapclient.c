/*our includes */
#include "mapserver.h"

/* needed includes*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h> /* only for -c99 */
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

	char responseType;
	read(sockfd, &responseType, 1); /* read response type */

	if(responseType == 'M')
	{
		srv_map_response_t mapResponse;
		read(sockfd, &mapResponse, sizeof(mapResponse)); /* get width and height */

		int BUF_SIZE = mapResponse.width;
		char buff[BUF_SIZE];
		read(sockfd, buff, BUF_SIZE + 1); /* read remaining garbage bytes */

		while((n = read(sockfd, buff, BUF_SIZE - 1)) > 0)
		{
			buff[n] = 0;
			printf("%s", buff);
		}
		if (n < 0)
		{
			fprintf(stderr, "Error");
		}
	}
	else
	{
		srv_err_response_t errorResponse;
		read(sockfd, &errorResponse, sizeof(errorResponse)); /* get error response size */

		int BUF_SIZE = errorResponse.err_len;
		char buff[BUF_SIZE];
		while((n = read(sockfd, buff, BUF_SIZE - 1)) > 0)
		{
			buff[n] = 0;
			fprintf(stderr, "%s\n", buff);
		}
		if (n < 0)
		{
			fprintf(stderr, "Error\n");
		}
	}

	close(sockfd);
	return 0;
}
