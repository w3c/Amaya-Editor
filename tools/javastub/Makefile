#
# Makefile for javastub
#
CC=gcc
# CC=cc
# CFLAGS=-g -Wall
# CFLAGS=-pg -O -Wall
 CFLAGS= -O



all : javastub

javastub : javastub.o
	$(CC) $(CFLAGS) -o $@ javastub.o

javastub.o : javastub.c
	$(CC) $(CFLAGS) -c $<

clean :
	rm javastub *.o thotlib_Interface*

install : all
	-@(mkdir ../../`../../batch/MachineType`/bin)
	cp javastub ../../`../../batch/MachineType`/bin

test: all
	./javastub -C thotlib_Interface ../../thotlib/include/interface.h 

