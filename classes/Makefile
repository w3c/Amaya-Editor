.SUFFIXES: .java .class

.java.class : 
	javac $*.java

CLASSROOT=`pwd`
ZIP=zip -0
COMPRESS=zip -9

SUBDIRS=thotlib amaya thot external biss

all : http subdir

http :
	@(if [ -d w3c ] ; then cd w3c ; make MAKEDIR=`pwd` ; fi)

zips :
	rm -f jigsaw.zip thotlib.zip amaya.zip sources.zip
	$(ZIP) jigsaw.zip `find w3c -name \*.class`
	$(ZIP) thotlib.zip `find thotlib -name \*.class`
	$(ZIP) amaya.zip `find amaya -name \*.class`
	$(COMPRESS) sources.zip `find thotlib amaya w3c -name \*.java` \
	       `find thotlib amaya w3c -name Makefile` \
	       w3c/file.make w3c/package.make Makefile

subdir :
	@(CLASSROOT="$(CLASSROOT)" ; export CLASSROOT; \
	  CLASSPATH="$(CLASSPATH):$(CLASSROOT)/external" ; export CLASSPATH; \
	  for subdir in $(SUBDIRS) ; do \
	  if [ -d $$subdir ] ; \
	  then \
	      cd $$subdir ; make ; cd .. ; \
	  fi ; done)

clean :
	@(if [ -d w3c ] ; then cd w3c ; make MAKEDIR=`pwd` clean ; fi)
	@(for subdir in $(SUBDIRS) ; do \
	  if [ -d $$subdir ] ; \
	  then \
	      cd $$subdir ; make clean ; cd .. ; \
	  fi ; done)
