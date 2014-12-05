
CC=gcc
DEBUG=-g -D_DEBUG
INCLUDE=-I. -I..
FLAGS=--std=c99 -Wall -Wextra
SERV_EXE=mapserver
OBJ=mapserver.o
CLI_EXE=mapclient
CLI_C=mapclient.c

all: $(SERV_EXE) $(CLI_EXE)

$(SERV_EXE): $(OBJ)
	$(CC) $(DEBUG) $(INCLUDE) $(FLAGS) $(OBJ) -o $(SERV_EXE)

$(CLI_EXE):
	$(CC) $(DEBUG) $(INCLUDE) $(FLAGS) $(CLI_C) -o $(CLI_EXE)

%.o: %.c
	$(CC) -c $(DEBUG) $(INCLUDE) $(FLAGS) $<

clean:
	rm -f $(SERV_EXE) $(CLI_EXE)
