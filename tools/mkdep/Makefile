#
# Makefile for mkdep
#
CC=gcc
# CC=cc
# CFLAGS=-g -Wall
# CFLAGS=-pg -O -Wall
CFLAGS= -O


all : mkdep

mkdep : mkdep.o
	$(CC) $(CFLAGS) -o $@ mkdep.o

mkdep.o : mkdep.c
	$(CC) $(CFLAGS) -c $<

clean :
	rm mkdep *.o 

install : all
	-@(mkdir ../../`../../batch/MachineType`/bin)
	cp mkdep ../../`../../batch/MachineType`/bin

test: all
	./mkdep -I/users/veillard/opera/w3c-libwww-5.0a/Library/src -I/users/veillard/opera/thotlib/internals/h -I/users/veillard/opera/thotlib/internals/var -I/users/veillard/opera/thotlib/internals/f -I/users/veillard/opera/thotlib/include /users/veillard/opera/amaya/*.c
