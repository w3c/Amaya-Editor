.SUFFIXES: .java .class

.java.class : 
	javac $*.java

AMAYA_DIRS=org/w3c/amaya
THOTLIB_DIRS=org/w3c/thotlib
JIGSAW_DIRS=org/w3c/www org/w3c/tools org/w3c/util org/w3c/jigsaw

CLASSROOT=`pwd`

ZIP=zip -0
COMPRESS=zip -9

all : debug w3c_compile

debug :
	@echo CLASSPATH=$$CLASSPATH
	@echo PATH=$$PATH
	@echo MAKEDIR=$(MAKEDIR)

w3c_compile:
	@(if [ -d org/w3c ] ; then cd org/w3c ; export MAKEDIR="`pwd`/../../../makefiles"; make AMAYAPACKAGES='amaya thotlib' JIGSAWPACKAGES='' ; fi)

zips :
	rm -f jigsaw.zip thotlib.zip amaya.zip sources.zip
	$(ZIP) jigsaw.zip `find $(JIGSAW_DIRS) -name \*.class`
	$(ZIP) thotlib.zip `find $(THOTLIB_DIRS) -name \*.class`
	$(ZIP) amaya.zip `find $(AMAYA_DIRS) -name \*.class`
	$(COMPRESS) sources.zip \
	`find $(JIGSAW_DIRS) $(THOTLIB_DIRS) $(AMAYA_DIRS) -name \*.java` \
	`find $(JIGSAW_DIRS) $(THOTLIB_DIRS) $(AMAYA_DIRS) -name Makefile` \
	       makefile Makefile

clean :
	@(if [ -d org/w3c ] ; then cd org/w3c ; export MAKEDIR="`pwd`/../../../makefiles"; make AMAYAPACKAGES='amaya thotlib' JIGSAWPACKAGES='' clean ; fi)
