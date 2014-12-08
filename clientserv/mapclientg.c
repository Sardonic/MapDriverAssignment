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
#include <signal.h>
#include <fcntl.h>
#include <sys/prctl.h>
#include <sys/wait.h>

typedef struct mapNode
{
	pid_t pid;
	char character;
	int x;
	int y;
	struct mapNode* nextNode;

} MapNode;

MapNode* headNode = NULL;
int sockfd = -1;

int logfd = -1;

void logmsg(const char* msg)
{
	if (logfd >= 0)
	{
		write(logfd, msg, strlen(msg));
		write(logfd, "\n", 1);
	}
}

void error(const char *msg)
{
	perror(msg);
	exit(0);
}

static void sig_hup(int signo)
{
	if (signo == SIGHUP)
	{
		MapNode* currentNode = headNode;
		MapNode** pp = &currentNode;

		while (currentNode != NULL)
		{
			currentNode = currentNode->nextNode;

			kill((*pp)->pid, SIGHUP);

			free(*pp);
			*pp = NULL;
			pp = &currentNode;
		}
	}
}

static void sig_usr(int signo)
{
	if(signo == SIGUSR1)
	{
		printf("received SIGUSR1\n");
	}
	else
	{
		fprintf(stderr, "received signal: %d, I didn't expect that.\n", signo);
	}
}

int requestMap(int sockfd, cli_map_request_t req)
{
	int n;
	char msg[2];
	snprintf(msg, 2, "%c", 'M');
	n = write(sockfd, msg, 1);
	n = write(sockfd,&req,sizeof(req));
	if (n < 0) 
	{
		error("ERROR writing to socket");
		return -1;
	}

	return 0;
}

char* retrieveMap(int sockfd, int* mapWidth, int* mapHeight)
{
	char* outputMap;
	int n;
	char responseType;
	read(sockfd, &responseType, 1); /* read response type */

	if(responseType == 'M')
	{
		srv_map_response_t mapResponse;
		read(sockfd, &mapResponse, sizeof(mapResponse)); /* get width and height */

		*mapWidth = mapResponse.width + 1;
		*mapHeight = mapResponse.height;
		const int BUF_SIZE = mapResponse.width;
		char buff[BUF_SIZE];

		/* map size + all the newlines + null terminator */
		const int FULL_MAP_SIZE = (BUF_SIZE * mapResponse.height) + mapResponse.height + 1; 
		outputMap = malloc(FULL_MAP_SIZE);
		memset(outputMap, 0, FULL_MAP_SIZE);

		while((n = read(sockfd, buff, BUF_SIZE - 1)) > 0)
		{
			buff[n] = 0;
			strcat(outputMap, buff);
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

		const int BUF_SIZE = errorResponse.err_len;
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

	return outputMap;
}

/* Removes all characters not in filter from the map,
 * returns number of chars remaining */
int parseMap(char* map, char* filter)
{
	int remainingChars = 0;
	char* currentChar = map;

	while(*currentChar != 0)
	{
		if(*currentChar > 32) /* don't want to replace spaces or newlines */
		{
			char* filterChar = filter;
			int bDeleteChar = 1;
			while(*filterChar != 0)
			{
				if(*filterChar == *currentChar)
				{
					bDeleteChar = 0;
					break;
				}

				filterChar++;
			}

			if(bDeleteChar)
			{
				*currentChar = ' ';
			}
			else
			{
				remainingChars++;
			}
		}

		currentChar++;
	}

	return remainingChars;
}

void handleChildBusiness(char* name, int sockfd, cli_kill_request_t req)
{
	int namelen = strlen(name);
	char newName[namelen];

	snprintf(newName, namelen, "tmpid %c %d %d", req.charToKill, req.x, req.y);
	strncpy(name, newName, namelen);

	struct sigaction act;
	act.sa_handler = sig_usr;

	if (sigaction(SIGUSR1, &act, NULL) == -1)
	{
		fprintf(stderr, "Can't catch SIGUSR1: %s", strerror(errno));
	}

	/*
	if (sigaction(SIGHUP, &act, NULL) == -1)
	{
		fprintf(stderr, "Can't catch SIGHUP: %s", strerror(errno));
		exit(1);
	}
	*/

	pause();

	int n;
	char msg[2];
	snprintf(msg, 2, "%c", 'K');
	n = write(sockfd, msg, 1);
	n = write(sockfd,&req,sizeof(req));
	if (n < 0) 
	{
		error("ERROR writing to socket");
	}
	logmsg("Wrote kill request.");

	exit(req.charToKill);
}

void forkChars(char* map, int width, char* name, int sockfd)
{
	MapNode* headNode = malloc(sizeof(MapNode));
	MapNode* currentNode = headNode;
	currentNode->nextNode = NULL;

	char* currentMapChar = map;
	int row = 0;
	int col = 0;
	while(*currentMapChar != 0)
	{
		while(*currentMapChar != '\n')
		{
			if(*currentMapChar > 32)
			{
				pid_t pid = fork();

				if(pid == 0)
				{
					cli_kill_request_t req;
					req.charToKill = *currentMapChar;
					req.x = col;
					req.y = row;
					handleChildBusiness(name, sockfd, req);
				}
				else
				{
					currentNode->pid = pid;
					currentNode->character = *currentMapChar;
					currentNode->x = col;
					currentNode->y = row;
					currentNode->nextNode = malloc(sizeof(MapNode));
					currentNode = currentNode->nextNode;
					currentNode->nextNode = NULL;
				}
			}

			currentMapChar++;
			col++;
		}
	
		currentMapChar++;
		row++;
		col = 0;
	}

	/* Reap kids when we die */
	{
		struct sigaction act;
		act.sa_handler = sig_hup;
		sigaction(SIGHUP, &act, NULL);
	}


	pid_t n = 0;
	while((n = wait(NULL)) > 0) /* only the parent gets here */
	{
		if(n != 0)
		{
			currentNode = headNode;
			/* is the pid found in the head of the linked list? */
			if(currentNode->pid == n)
			{
				int mapPos = currentNode->x + currentNode->y * width;
				map[mapPos] = ' ';

				headNode = currentNode->nextNode;
				free(currentNode);
			}
			else
			{
				/* maybe it's in the middle or the end? */
				do
				{
					MapNode* prevNode = currentNode;
					currentNode = currentNode->nextNode;

					if(currentNode->pid == n)
					{
						int mapPos = currentNode->x + currentNode->y * width;
						map[mapPos] = ' ';

						prevNode->nextNode = currentNode->nextNode;
						free(currentNode);
						break;
					}

				}while(currentNode->nextNode != NULL);
			}

			printf("Current Map:\n%s", map);

		}
	}
}

void atExit()
{
	cli_game_over_t go;
	go.over = 'O';
	int n;
	char msg[2];
	snprintf(msg, 2, "%c", 'G');
	n = write(sockfd, msg, 1);
	n = write(sockfd,&go,sizeof(go));
	if (n < 0) 
	{
		error("ERROR writing to socket");
	}

	close(sockfd);
}

int main(int argc, char *argv[])
{
	int portno;
	struct sockaddr_in serv_addr;
	cli_map_request_t req;
	char *ip_addr;

	logfd = open("mapclientg.log", O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
	
	/* Set exit logic */
	atexit(atExit);

	/* Kill all children when we die */
	//prctl(PR_SET_PDEATHSIG, SIGHUP);

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

	int requestSuccessful = requestMap(sockfd, req);

	if(requestSuccessful == 0)
	{
		int mapWidth = 0;
		int mapHeight = 0;
		char* fullMap = retrieveMap(sockfd, &mapWidth, &mapHeight);
		if(fullMap != NULL)
		{
			char* initialsFilter = "SABJDK";
			int remainingChars = parseMap(fullMap, initialsFilter);

			printf("There are %d characters remaining.\n", remainingChars);
			printf("%s", fullMap);

			forkChars(fullMap, mapWidth, argv[0], sockfd);
			free(fullMap);
		}
	}

	close(logfd);

	return 0;
}
