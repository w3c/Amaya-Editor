.SUFFIXES: .java .class

.java.class : 
	javac $*.java

SUBDIRS=thotlib amaya new_amaya thot test

all : http subdir

http :
	@(cd w3c ; make MAKEDIR=`pwd`)

zips :
	zip jigsaw.zip `find w3c -name \*.class`
	zip thotlib.zip `find thotlib -name \*.class`
	zip amaya.zip `find amaya -name \*.class`

subdir :
	@(for subdir in $(SUBDIRS) ; do cd $$subdir ; make ; cd .. ; done)

