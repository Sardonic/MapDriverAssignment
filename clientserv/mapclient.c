#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<stdlib.h>
#include<errno.h>
#include<arpa/inet.h>
#include<string.h>

int DEFAULT_PORT = 23032;
char* IP_DEFAULT = "127.0.0.1";


int main(int argc, char *argv[])
{
	char requestBuff[sizeof(char) + sizeof(int)];

	int sockfd = 0, n = 0, portno = DEFAULT_PORT;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	/* TODO: check arguments*/

	memset(&requestBuff, '0' , sizeof(requestBuff));

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf ("\n Error : Could not create socket\n");
		return 1;
	}
	
	memset(&serv_addr, '0' , sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(DEFAULT_PORT);
	
	if(inet_pton(AF_INET, IP_DEFAULT, &serv_addr.sin_addr) <=0)
	{
		printf("\n inet_pton error occured\n");
		return 1;
	}

	if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\n Error : Connection Failed \n");
		return -1;
	}


};


