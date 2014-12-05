#include "mapserver.h"

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>

/* Error codes */
#define ECHAR -1
#define ENEGHEIGHT -2
#define ENEGWIDTH -3

const char* ERR_MSGS[] = {"ERROR: Unrecognized char",
			"ERROR: Height is negative",
			"ERROR: Width is negative"};

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

int is_request_good(const cli_map_request_t* cli_req)
{
	int returnval = 0;

	if (cli_req->width < 0)
		returnval = ENEGWIDTH;
	else if (cli_req->width != 0 && cli_req->height < 0)
		returnval = ENEGHEIGHT;
	else
		returnval = 0;

	return returnval;
}

int respond_err(int connfd, int err)
{
	char msg[50] = {0};
	int n;
	int index;
	index = err * -1 - 1;

	/* TODO: Write message length to socket */
	strncpy(msg, ERR_MSGS[index], 50);

	n = write(connfd, msg, strlen(msg) + 1);
	if (n < 0)
		fatal(NULL);

#ifdef _DEBUG
	printf("Wrote message: %s\n", msg);
	printf("Message length: %d\n", n);
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

	
	/* Check message validity */
	int err = 0;
	if ((err = is_request_good(cli_req)) < 0)
	{
#ifdef _DEBUG
		printf("Problem with map request\n");
#endif
		respond_err(connfd, err);
		return -1;
	}

	if (width == 0)
	{
		width = 50;
		height = 50;
	}

	map_resp.width = width;
	map_resp.height = height;

	/* TODO: Read from /dev/asciimap instead of making up line of text */

	int n;
	char map[BSIZE];
	int mapfd = open("/dev/asciimap", O_RDONLY);
	if (mapfd < 0)
		fatal("Error opening /dev/asciimap");

	n = read(mapfd, map, BSIZE);
	if (n < 0)
		fatal("Error reading /dev/asciimap");

	//char* line = "12345678901234567890\n";
#define MSGLEN 50

	char msg[MSGLEN] = {0};
	int str_len = 0;
	memset(&msg[str_len], SRV_MAP_CHAR, sizeof(char));
	str_len += sizeof(char);
	memcpy(&msg[str_len], &map_resp, sizeof(map_resp));
	str_len += sizeof(map_resp);
	strncat(&msg[str_len], map, MSGLEN - str_len - 1);
	str_len += strlen(&msg[str_len]) + 1;

	n = write(connfd, msg, str_len);
	if (n < 0)
		fatal(NULL);

#ifdef _DEBUG
	printf("Received map request\n");
	//printf("Wrote message: %s\n", msg);
	write(STDOUT_FILENO, msg, str_len);
	printf("Message has length: %d\n", str_len);
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
				respond_err(connfd, ECHAR);
				break;
			}
		}
	}

}
