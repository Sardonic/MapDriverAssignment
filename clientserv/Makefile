
CC=gcc
DEBUG=-g -D_DEBUG
INCLUDE=-I. -I..
FLAGS=-Wall -Wextra
SERV_EXE=mapserver
SERV_G_EXE=mapserverg
SERV_G_C=mapserverg.c
OBJ=mapserver.o
CLI_EXE=mapclient
CLI_C=mapclient.c
CLI_G_EXE=mapclientg
CLI_G_C=mapclientg.c
TEST_FORK_C=../testForkExec.c
TEST_FORK_EXE=../testForkExec

all: $(SERV_EXE) $(CLI_EXE) $(TEST_FORK_EXE) $(SERV_G_EXE) $(CLI_G_EXE)

$(SERV_EXE): $(OBJ)
	$(CC) $(DEBUG) $(INCLUDE) $(FLAGS) $(OBJ) -o $(SERV_EXE)

$(CLI_EXE): $(CLI_C)
	$(CC) $(DEBUG) $(INCLUDE) $(FLAGS) $(CLI_C) -o $(CLI_EXE)

$(TEST_FORK_EXE): $(TEST_FORK_C)
	$(CC) $(DEBUG) $(INCLUDE) $(FLAGS) $(TEST_FORK_C) -o $(TEST_FORK_EXE)

$(SERV_G_EXE): $(SERV_G_C)
	$(CC) $(DEBUG) $(INCLUDE) $(FLAGS) $(SERV_G_C) -o $(SERV_G_EXE)

$(CLI_G_EXE): $(CLI_G_C)
	$(CC) $(DEBUG) $(INCLUDE) $(FLAGS) $(CLI_G_C) -o $(CLI_G_EXE)

%.o: %.c
	$(CC) -c $(DEBUG) $(INCLUDE) $(FLAGS) $<

start:
	./$(SERV_EXE) &
	netstat -vatn | grep 23032

kill:
	killall ./$(SERV_EXE)

kill-client:
	kill `ps -ef | grep tmpid | grep -v grep | awk '{print $$2}'`

test:
	for i in `pgrep -f tmpid`; do kill -10 $$i; sleep 1; done;

clean:
	rm -f $(SERV_EXE) $(CLI_EXE) $(TEST_FORK_EXE)
