MACHINE=`../batch/MachineType`

all : Interface Document

Interface : thotlib_Interface.c thotlib_Interface.h thotlib_Interface_stubs.c \
            thotlib_Interface_stubs.h thotlib_Interface_native.java

Document : thotlib_Document.c thotlib_Document.h thotlib_Document_stubs.c \
            thotlib_Document_stubs.h thotlib_Document_native.java

thotlib_Document_stubs.c thotlib_Document_stubs.h thotlib_Document_native.java : ../thotlib/include/document.h
	javastub -C thotlib_Document ../thotlib/include/document.h

thotlib_Interface_stubs.c thotlib_Interface_stubs.h thotlib_Interface_native.java : ../thotlib/include/interface.h
	javastub -C thotlib_Interface ../thotlib/include/interface.h

thotlib_Interface.c thotlib_Interface.h : ../classes/thotlib/Interface.class
	kaffeh -stubs  thotlib.Interface
	kaffeh thotlib.Interface

../classes/thotlib/Interface.class : ../classes/thotlib/Interface.java
	cd ../classes/thotlib ; javac Interface.java

thotlib_Document.c thotlib_Document.h : ../classes/thotlib/Document.class
	kaffeh -stubs  thotlib.Document
	kaffeh thotlib.Document

../classes/thotlib/Document.class : ../classes/thotlib/Document.java
	cd ../classes/thotlib ; javac Document.java
