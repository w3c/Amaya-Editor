.SUFFIXES: .java .class

.java.class : 
	javac $*.java

AMAYA_DIRS=w3c/amaya
THOTLIB_DIRS=w3c/thotlib
JIGSAW_DIRS=w3c/www w3c/tools w3c/util w3c/jigsaw

CLASSROOT=`pwd`

ZIP=zip -0
COMPRESS=zip -9

SUBDIRS=thotlib amaya thot external biss

all : debug http subdir

debug :
	@echo CLASSPATH=$$CLASSPATH
	@echo PATH=$$PATH

http :
	@(if [ -d w3c ] ; then cd w3c ; make ; fi)

zips :
	rm -f jigsaw.zip thotlib.zip amaya.zip sources.zip
	$(ZIP) jigsaw.zip `find $(JIGSAW_DIRS) -name \*.class`
	$(ZIP) thotlib.zip `find $(THOTLIB_DIRS) -name \*.class`
	$(ZIP) amaya.zip `find $(AMAYA_DIRS) -name \*.class`
	$(COMPRESS) sources.zip \
	`find $(JIGSAW_DIRS) $(THOTLIB_DIRS) $(AMAYA_DIRS) -name \*.java` \
	`find $(JIGSAW_DIRS) $(THOTLIB_DIRS) $(AMAYA_DIRS) -name Makefile` \
	       makefile Makefile

subdir :
	@(CLASSROOT="$(CLASSROOT)" ; export CLASSROOT; \
	  CLASSPATH="$(CLASSPATH):$(CLASSROOT)/external" ; export CLASSPATH; \
	  for subdir in $(SUBDIRS) ; do \
	  if [ -d $$subdir ] ; \
	  then \
	      cd $$subdir ; make ; cd .. ; \
	  fi ; done)

clean :
	@(if [ -d w3c ] ; then cd w3c ; make clean ; fi)
	@(for subdir in $(SUBDIRS) ; do \
	  if [ -d $$subdir ] ; \
	  then \
	      cd $$subdir ; make clean ; cd .. ; \
	  fi ; done)
