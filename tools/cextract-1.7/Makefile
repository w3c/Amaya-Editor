# Generated automatically from Makefile.in by configure.
#
# Makefile for cextract-1.7
# Generated from tools/cextract-1.7/Makefile.in
# Daniel Veillard, 1997
#
VPATH= .
THOTDIR = ../..

include ../../Options

LIBS	= -ldl   -lm

prefix = /usr/local
exec_prefix = ${prefix}
bindir = ${exec_prefix}/bin

OBJS = \
      	parse.o \
	main.o \
	io.o

all cextract : ../../bin ../../bin/cextract

../../bin:
	@($(MKDIR) ../../bin)

../../bin/cextract : $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

.c.o :
	$(CC) '-DCPP="'"$(CPP)"'"' -O -c $< -o $@

clean :
	$(RM) -f *.o ../../bin/cextract

install :

uninstall :

include .depends

../../tools/mkdep/mkdep : 
	@(cd ../../tools/mkdep ; $(MAKE) CC="$(CC)" CFLAGS="$(CFLAGS)")

.depends depend : ../../tools/mkdep/mkdep
	../../tools/mkdep/mkdep -relative -vpath $(VPATH) $(INCLUDES) $(VPATH)/*.c  > .depends

proto :

