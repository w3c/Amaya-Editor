# Generated automatically from Makefile.in by configure.
#
# Makefile for mkdep
# Automatically generated from tools/mkdep/Makefile.in
# Daniel Veillard, 1997
#

include ../../Options

LIBS	= -ldl  

prefix = /usr/local
exec_prefix = ${prefix}
bindir = ${exec_prefix}/bin

all : mkdep

mkdep : mkdep.o
	$(LD) $(LDFLAGS) -o $@ mkdep.o

mkdep.o : ./mkdep.c
	$(CC) $(CFLAGS) -I../../ -c ./mkdep.c -o mkdep.o

clean :
	$(RM) *.o 

install :

uninstall :

test: all
	./mkdep -I../../../libwww/Library/src \
	        -I../../../thotlib/internals/h \
		-I../../../thotlib/internals/var \
		-I../../../thotlib/internals/f \
		-I../../../thotlib/include \
		../../../thotlib/*/*.c

depend :

proto :

