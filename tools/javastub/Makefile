#
# Makefile for javastub
#
CC=gcc
# CC=cc
CFLAGS=-g -Wall
# CFLAGS=-pg -O -Wall
# CFLAGS= -O -Wall

all : javastub

javastub : javastub.o
	$(CC) $(CFLAGS) -o $@ javastub.o

javastub.o : javastub.c
	$(CC) $(CFLAGS) -c $<

clean :
	rm -f javastub *.o thotlib_APIInterface* thotlib_APITree*

install : all
	-@(mkdir ../../`../../batch/MachineType`/bin)
	cp javastub ../../`../../batch/MachineType`/bin

test: all
	./javastub -T JavaTypes.def -M thotlib_APIContent ../../thotlib/include/content.h 
	./javastub -T JavaTypes.def -M thotlib_APITree ../../thotlib/include/tree.h 
#	./javastub -T JavaTypes.def -M thotlib_APIInterface ../../thotlib/include/interface.h 

