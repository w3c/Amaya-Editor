#
# Makefile for javastub
#
CC=gcc
# CC=cc
CFLAGS=-g -Wall
# CFLAGS=-pg -O -Wall
# CFLAGS= -O -Wall

CLASSTEST=thotlib_APIInterface


all : javastub

javastub : javastub.o
	$(CC) $(CFLAGS) -o $@ javastub.o

javastub.o : javastub.c
	$(CC) $(CFLAGS) -c $<

clean :
	rm -f javastub *.o $(CLASSTEST)*

install : all
	-@(mkdir ../../`../../batch/MachineType`/bin)
	cp javastub ../../`../../batch/MachineType`/bin

test: all
	./javastub -T types.lst -M $(CLASSTEST) ../../thotlib/include/interface.h 

