MACHINE=`../batch/MachineType`

#JAVASTUB=../$(MACHINE)/bin/javastub -T JavaTypes.def
JAVASTUB=javastub -T JavaTypes.def

APICLASSES=Application Document Tree Content Attribute Reference \
        Language Presentation View Selection Interface Registry \
	Dialog
        
CLASSES=Element SSchema Document ElementType Attribute AttributeType

all : 
	@(mv -f JavaTypes.err JavaTypes.err.old ; \
	  for classe in $(APICLASSES) ; do \
	   include=`echo $$classe | gawk '{print tolower($$0)}'`.h ; \
	   echo "Producing stubs and API class for $$classe from $$include" ; \
	   $(JAVASTUB) -M thotlib_API$$classe \
	               -J ../classes/thotlib/API$$classe.java \
		       ../thotlib/include/$$include 2>> JavaTypes.err ; \
	   echo "Compiling new Java class API$$classe" ; \
	   (cd ../classes ; javac thotlib/API$$classe.java) ; \
	   echo "Producing header for class API$$classe" ; \
	   kaffeh thotlib.API$$classe ; \
	  done ; \
	  for classe in $(CLASSES) ; do \
           echo "Compiling Java class $$classe" ; \
	   (cd ../classes ; javac thotlib/$$classe.java) ; \
           echo "Producing header for class $$classe" ; \
	   kaffeh thotlib.$$classe ; \
	  done ; \
	 )

