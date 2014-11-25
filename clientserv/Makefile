
CC=gcc
DEBUG=-g
INCLUDE=-I. -I..
FLAGS=--std=c99 -Wall -Wextra -Werror
EXE=mapserver
OBJ=mapserver.o

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(DEBUG) $(INCLUDE) $(FLAGS) $(OBJ) -o $(EXE)

%.o: %.c
	$(CC) -c $(DEBUG) $(INCLUDE) $(FLAGS) $<
