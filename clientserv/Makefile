
CC=gcc
DEBUG=-g
INCLUDE=-I. -I..
FLAGS=--std=c99
EXE=mapserver
OBJ=mapserver.o

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(DEBUG) $(INCLUDE) $(FLAGS) $(OBJ) -o $(EXE)

%.o: %.c
	$(CC) -c $(DEBUG) $(INCLUDE) $(FLAGS) $<
