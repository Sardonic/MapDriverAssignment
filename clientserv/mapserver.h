#ifndef MAPSERVER_H
#define MAPSERVER_H

#include "../asciimap.h"

#define DEFAULT_PORT 23032
#define DEFAULT_IP "127.0.0.1"

#define MAP_REQ_CHAR 'M'

typedef struct cli_request
{
	char cmd; /* Command we want to do. Always 'M' for now */
	int width; /* Map width. Zero if we don't care about size */
	int height; /* Map height */
} cli_request_t;

typedef struct srv_response
{
	int width;
	int height;
	char map[BSIZE];
} srv_response_t;

#endif /* MAPSERVER_H */
