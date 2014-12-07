#ifndef MAPSERVER_H
#define MAPSERVER_H

#include "../asciimap.h"

#define DEFAULT_PORT 23032
#define DEFAULT_IP "127.0.0.1"

#define CLI_MAP_CHAR 'M'
#define CLI_KILL_CHAR 'K'
#define CLI_GAME_OVER_CHAR 'Q'
#define SRV_ERR_CHAR "E"
#define SRV_MAP_CHAR "M"

typedef struct cli_map_request
{
	int width; /* Map width. Zero if we don't care about size */
	int height; /* Map height */
} cli_map_request_t;

typedef struct cli_kill_request
{
	char charToKill;
   	int x;
	int y;
} cli_kill_request_t;

typedef struct cli_game_over
{
	char game;
	char over;
} cli_game_over_t;

typedef struct srv_err_response_t
{
	/*
	srv_response_type_t type;
	union
	{
		struct {
			int err_len;
			char err[BSIZE];
		} err_data;
		struct {
			int width;
			int height;
			char map[BSIZE];
		} map_data;
	} data;
	*/

	int err_len;
} srv_err_response_t;

typedef struct srv_map_response_t
{
	int width;
	int height;
} srv_map_response_t;

#endif /* MAPSERVER_H */
