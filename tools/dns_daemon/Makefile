#
# Makefile for dns_daemon
#
CC=gcc
# CC=cc
CFLAGS=-g -Wall
# CFLAGS=-pg -O -Wall
# CFLAGS= -O -Wall

all : dns_daemon

dns_daemon : dns_daemon.o
	$(CC) $(CFLAGS) -o $@ dns_daemon.o -lsocket

dns_daemon.o : dns_daemon.c
	$(CC) $(CFLAGS) -c $<

clean :
	rm -f dns_daemon *.o

install : all
	-@(mkdir ../../`../../batch/MachineType`/bin)
	cp dns_daemon ../../`../../batch/MachineType`/bin

test: all
	./dns_daemon -T types.lst -M $(CLASSTEST) ../../thotlib/include/interface.h 

