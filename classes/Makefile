.SUFFIXES: .java .class

.java.class : 
	javac $*.java

SUBDIRS=thotlib amaya new_amaya thot test

all : http subdir

http :
	@(if [ -d w3c ] ; then cd w3c ; make MAKEDIR=`pwd` ; fi)

zips :
	zip jigsaw.zip `find w3c -name \*.class`
	zip thotlib.zip `find thotlib -name \*.class`
	zip amaya.zip `find amaya -name \*.class`

subdir :
	@(for subdir in $(SUBDIRS) ; do \
	  if [ -d $$subdir ] ; \
	  then \
	      cd $$subdir ; make ; cd .. ; \
	  fi ; done)

