INCLUDES = -I./h

#DEFINES = -DDEBUG

CC = gcc

CFLAGS = -g -Wall

OBJFILES = list.o awstring.o awpair.o awnode.o awtree.o

all : $(OBJFILES) 

clean :
	@(rm *.o )

dist : 
	@(rm $(FINALFILES) $(OBJFILES))
.c.o :
	$(CC) $(CFLAGS) $(INCLUDES)  $(DEFINES) -c $< -o $@

