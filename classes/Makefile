.SUFFIXES: .java .class

.java.class : 
	javac $*.java

SUBDIRS=thotlib amaya thot test

all : subdir

subdir :
	@(for subdir in $(SUBDIRS) ; do cd $$subdir ; make ; cd .. ; done)

