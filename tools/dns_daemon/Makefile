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
	@(if [ "`../../batch/MachineType`" = "solaris2" ] ;\
	then \
	    $(CC) $(CFLAGS) -o $@ dns_daemon.o -lsocket ;\
	else \
	    $(CC) $(CFLAGS) -o $@ dns_daemon.o ;\
	fi)

dns_daemon.o : dns_daemon.c
	$(CC) $(CFLAGS) -c $<

clean :
	rm -f dns_daemon *.o

install : all
	-@(mkdir ../../`../../batch/MachineType`/bin)
	cp dns_daemon ../../`../../batch/MachineType`/bin


