.SUFFIXES: .java .class

.java.class : 
	javac $*.java

SUBDIRS=thotlib amaya new_amaya thot test

all : http subdir

http :
	@(cd w3c ; make MAKEDIR=`pwd`)

zip :
	zip jigsaw.zip `find w3c -name \*.class`

subdir :
	@(for subdir in $(SUBDIRS) ; do cd $$subdir ; make ; cd .. ; done)

