#
# Makefile for mkdep
#
CC=gcc
# CC=cc
# CFLAGS=-g -Wall
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