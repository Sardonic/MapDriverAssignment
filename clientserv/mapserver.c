#include "mapserver.h"

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

/* Error messages returned to the client */
const char* ERR_MSG = "ERROR: Malformed request.\n";

static void fatal(const char* msg)
{
	perror(msg);
	exit(1);
}

char* truncate_to_width(char* line, unsigned int width)
{
	if (strlen(line) <= width)
		return line;

	line[width] = '\n';

	return line;
}

bool is_request_good(const cli_map_request_t* cli_req)
{
	bool returnval = false;

	if (cli_req->width < 0)
		returnval = false;
	else if (cli_req->width != 0 && cli_req->height < 0)
		returnval = false;
	else
		returnval = true;

	return returnval;
}

int respond_with_unknown_request_error(int connfd, char cmd)
{
	char msg[50] = {0};
	int len = 0;
	int n;

	len = snprintf(msg,
			50,
			"%cERROR: Unrecognized command char: %c",
			SRV_ERR_CHAR,
			cmd);
	n = write(connfd, msg, strlen(msg) + 1);
	if (n < 0)
		fatal(NULL);

#ifdef _DEBUG
	printf("Unknown request error.\n");
	printf("Wrote message: %s", msg);
	printf("Message length: %d", len);
#endif

	return 0;
}

int respond_to_map_request(int connfd, const cli_map_request_t* cli_req)
{
	int width,
	    height;

	width = cli_req->width;
	height = cli_req->height;
	srv_map_response_t map_resp;

	map_resp.width = width;
	map_resp.height = height;
	
	/* Check message validity */
	if (!is_request_good(cli_req))
	{
		return -1;
	}

	if (width == 0)
	{
		width = 50;
		height = 50;
	}

	char* line = "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\n";
	char msg[50] = {0};
	int acc = 0;
	memset(&msg[acc], SRV_MAP_CHAR, sizeof(char));
	acc += sizeof(char);
	memcpy(&msg[acc], &map_resp, sizeof(map_resp));
	acc += sizeof(map_resp);
	strncat(&msg[acc], line, 50 - acc);
	acc += strlen(&msg[acc]);

	int n;
	n = write(connfd, msg, acc);
	if (n < 0)
		fatal(NULL);

#ifdef _DEBUG
	printf("Received map request\n");
	//printf("Wrote message: %s\n", msg);
	write(STDOUT_FILENO, msg, acc);
	printf("Message has length: %d\n", acc);
#endif

	return 0;
}


int main(void)
{
	int sockfd;
	int connfd;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	int n;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		fatal(NULL);

	memset(&serv_addr, 0, sizeof(serv_addr));
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
		//char buf[1024 * 16] = {0};
		//int buf_len = 0;

		connfd = accept(sockfd,
				(struct sockaddr *) &cli_addr,
				&clilen);
#ifdef _DEBUG
		printf("Received request\n");
#endif

		if (connfd < 0)
			fatal(NULL);

		memset(&serv_addr, 0, sizeof(serv_addr));

		/* Identify request */
		{
			char cmd = 0;
			n = read(connfd, &cmd, sizeof(cmd));
			if (n < 0)
				fatal(NULL);

			printf("Command char: %c\n", cmd);
			switch (cmd)
			{
			case 'M':
				{
					cli_map_request_t cli_req;
					n = read(connfd, &cli_req, sizeof(cli_req));
					if (n < 0)
						fatal(NULL);
					respond_to_map_request(connfd, &cli_req);
				}
				break;
			default:
				respond_with_unknown_request_error(connfd, cmd);
				break;
				/* Respond with error */
			}
		}
	}

#if 0
	while (1)
	{
		connfd = accept(sockfd,
				(struct sockaddr *) &cli_addr,
				&clilen);
		if (connfd < 0)
			fatal(NULL);

		memset(&serv_addr, 0, sizeof(serv_addr));
		n = read(connfd, &cli_req, sizeof(cli_request_t));
		if (n < 0)
			fatal(NULL);

		/* Print request info */
		{
			printf("Request:\t%c\n", cli_req.cmd);
			if (cli_req.cmd != 'M')
			{
				fprintf(stderr, "Request %c is invalid\n",
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

		/* Process request */
		/* Read first character */
		

		/*
		srv_response_t response;
		generate_response(cli_req, &response);
		*/
		/* TODO: Write back to socket */

		close(connfd);
		sleep(1); /* I mean, how often do we get requests, really? */
	}

	return 0;
#endif
}
