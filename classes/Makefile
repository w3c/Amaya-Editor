.SUFFIXES: .java .class

.java.class : 
	javac $*.java

ZIP=zip -0
SUBDIRS=thotlib amaya new_amaya thot test

all : http subdir

http :
	@(if [ -d w3c ] ; then cd w3c ; make MAKEDIR=`pwd` ; fi)

zips :
	rm -f jigsaw.zip thotlib.zip amaya.zip
	$(ZIP) jigsaw.zip `find w3c -name \*.class`
	$(ZIP) thotlib.zip `find thotlib -name \*.class`
	$(ZIP) amaya.zip `find amaya -name \*.class`

subdir :
	@(for subdir in $(SUBDIRS) ; do \
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
