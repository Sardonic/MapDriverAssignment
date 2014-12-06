
CC=gcc
DEBUG=-g -D_DEBUG
INCLUDE=-I. -I..
FLAGS=-Wall -Wextra
SERV_EXE=mapserver
OBJ=mapserver.o
CLI_EXE=mapclient
CLI_C=mapclient.c
TEST_FORK_C=../testForkExec.c
TEST_FORK_EXE=../testForkExec

all: $(SERV_EXE) $(CLI_EXE) $(TEST_FORK_EXE)

$(SERV_EXE): $(OBJ)
	$(CC) $(DEBUG) $(INCLUDE) $(FLAGS) $(OBJ) -o $(SERV_EXE)

$(CLI_EXE):
	$(CC) $(DEBUG) $(INCLUDE) $(FLAGS) $(CLI_C) -o $(CLI_EXE)

$(TEST_FORK_EXE):
	$(CC) $(DEBUG) $(INCLUDE) $(FLAGS) $(TEST_FORK_C) -o $(TEST_FORK_EXE)

%.o: %.c
	$(CC) -c $(DEBUG) $(INCLUDE) $(FLAGS) $<

start:
	./$(SERV_EXE) &
	netstat -vatn | grep 23032

kill:
	killall ./$(SERV_EXE)

clean:
	rm -f $(SERV_EXE) $(CLI_EXE) $(TEST_FORK_EXE)
