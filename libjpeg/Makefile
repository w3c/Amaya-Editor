# Generated automatically from Makefile.in by configure.
#
# Makefile for libjpeg directory
# Generated from libjpeg/Makefile.in
# Daniel Veillard, 1997
#

.SUFFIXES: .c .o

VPATH= .
THOTDIR = ..

include ../Options

INCLUDES= -I.. -I.  -I$(THOTDIR)/thotlib/include -I$(THOTDIR)/thotlib/internals/var -I$(THOTDIR)/thotlib/internals/h -I$(THOTDIR)/thotlib/internals/f -I$(THOTDIR)/libjpeg -I$(THOTDIR)/libpng -I$(THOTDIR)/libpng/zlib

prefix = /usr/local
exec_prefix = ${prefix}
libdir = ${exec_prefix}/lib

#
# Rule to build objects
#
.c.o :
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

all : ../libjpeg.a

clean :
	$(RM) libjpeg.a
	$(RM) *.o

install :

uninstall :

include .depends

../tools/mkdep/mkdep : 
	@(cd ../tools/mkdep ; $(MAKE) CC="$(CC)" CFLAGS="$(CFLAGS)")

.depends depend : ../tools/mkdep/mkdep
	../tools/mkdep/mkdep -relative -vpath $(VPATH) $(INCLUDES) $(VPATH)/*.c  > .depends

#########################################################################
#									#
#		Rules to build the library				#
#									#
#########################################################################

JPEG_OBJ = \
	jcapimin.o \
	jcapistd.o \
	jccoefct.o \
	jccolor.o \
	jcdctmgr.o \
	jchuff.o \
	jcinit.o \
	jcmainct.o \
	jcmarker.o \
	jcmaster.o \
	jcomapi.o \
	jcparam.o \
	jcphuff.o \
	jcprepct.o \
	jcsample.o \
	jctrans.o \
	jdapimin.o \
	jdapistd.o \
	jdatadst.o \
	jdatasrc.o \
	jdcoefct.o \
	jdcolor.o \
	jddctmgr.o \
	jdhuff.o \
	jdinput.o \
	jdmainct.o \
	jdmarker.o \
	jdmaster.o \
	jdmerge.o \
	jdphuff.o \
	jdpostct.o \
	jdsample.o \
	jdtrans.o \
	jerror.o \
	jfdctflt.o \
	jfdctfst.o \
	jfdctint.o \
	jidctflt.o \
	jidctfst.o \
	jidctint.o \
	jidctred.o \
	jmemmgr.o \
	jmemnobs.o \
	jquant1.o \
	jquant2.o \
	jutils.o

../libjpeg.a : $(JPEG_OBJ)
	$(AR) ../libjpeg.a $(JPEG_OBJ)
	$(RANLIB) ../libjpeg.a
	@$(ECHO) "====>" libjpeg is done


