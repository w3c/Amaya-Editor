SHELL="/bin/sh"
MACHINE=`../batch/MachineType`

JAVASTUB=../$(MACHINE)/bin/javastub -T JavaTypes.def
#JAVASTUB=javastub -T JavaTypes.def

JAVAC=../$(MACHINE)/bin/javac
#JAVAC=javac

KAFFEH=../$(MACHINE)/bin/kaffeh
#KAFFEH=kaffeh

APICLASSES=Application Document Tree Content Attribute Reference \
        Language Presentation View Selection Interface Registry \
	Dialog LibMsg AppAction
        
CLASSES=Element SSchema Document ElementType Attribute \
        AttributeType PRule Extra Language IntPtr Action
	
HEADER=StringBuffer

AMAYAAPI_CLASSES=JavaAmaya AmayaMsg
AMAYAAPI_CLASSES_UP=Html
AMAYA_CLASSES=HTTPRequest

all :
	@(echo Do not run make here, type \"make stubs\" from the platform directory ;\
	  exit 1)

stubs : thotlib_itf amaya_itf amaya_itf_up

thotlib_itf:
	@(mv -f JavaTypes.err JavaTypes.err.old ; \
	  for classe in $(APICLASSES) ; do \
	   include=`echo $$classe | gawk '{print tolower($$0)}'`.h ; \
	   echo "Producing stubs and API class for $$classe from $$include" ; \
	   $(JAVASTUB) -M thotlib_API$$classe \
	               -J ../classes/thotlib/API$$classe.java \
		       ../thotlib/include/$$include 2>> JavaTypes.err ; \
	   echo "Compiling new Java class API$$classe" ; \
	   (cd ../classes ; $(JAVAC) thotlib/API$$classe.java) ; \
	   echo "Producing header for class API$$classe" ; \
	   $(KAFFEH) thotlib.API$$classe ; \
	  done ; \
	  for classe in $(CLASSES) ; do \
           echo "Compiling Java class $$classe" ; \
	   (cd ../classes ; $(JAVAC) thotlib/$$classe.java) ; \
           echo "Producing header for class $$classe" ; \
	   $(KAFFEH) thotlib.$$classe ; \
	  done ; \
	  for classe in $(HEADER) ; do \
           echo "Producing header for class $$classe" ; \
	   $(KAFFEH) java.lang.$$classe ; \
	  done ; \
	 )

amaya_itf:
	@(mv -f JavaTypes.err JavaTypes.err.old ; \
	  for classe in $(AMAYAAPI_CLASSES) ; do \
	   include=`echo $$classe | gawk '{print tolower($$0)}'`.h ; \
	   echo "Producing stubs and API class for $$classe from $$include" ; \
	   $(JAVASTUB) -M amaya_API$$classe \
	               -J ../classes/amaya/API$$classe.java \
		       ../amaya/$$include 2>> JavaTypes.err ; \
	   echo "Compiling new Java class API$$classe" ; \
	   (cd ../classes ; $(JAVAC) amaya/API$$classe.java) ; \
	   echo "Producing header for class API$$classe" ; \
	   $(KAFFEH) amaya.API$$classe ; \
	  done ; \
	  for classe in $(AMAYA_CLASSES) ; do \
           echo "Compiling Java class $$classe" ; \
	   (cd ../classes ; $(JAVAC) amaya/$$classe.java) ; \
           echo "Producing header for class $$classe" ; \
	   $(KAFFEH) amaya.$$classe ; \
	  done ; \
	 )


amaya_itf_up:
	 @(mv -f JavaTypes.err JavaTypes.err.old ; \
	   for classe in $(AMAYAAPI_CLASSES_UP) ; do \
	    include=`echo $$classe | gawk '{print toupper($$0)}'`.h ; \
	    echo "Producing stubs and API class for $$classe from $$include" ; \
	    $(JAVASTUB) -M amaya_API$$classe \
			-J ../classes/amaya/API$$classe.java \
			../amaya/$$include 2>> JavaTypes.err ; \
	    echo "Compiling new Java class API$$classe" ; \
	    (cd ../classes ; $(JAVAC) amaya/API$$classe.java) ; \
	    echo "Producing header for class API$$classe" ; \
	    $(KAFFEH) amaya.API$$classe ; \
	   done ; \
	   for classe in $(AMAYA_CLASSES) ; do \
	    echo "Compiling Java class $$classe" ; \
	    (cd ../classes ; $(JAVAC) amaya/$$classe.java) ; \
	    echo "Producing header for class $$classe" ; \
	    $(KAFFEH) amaya.$$classe ; \
	   done ; \
	  )

