
all : Interface Document

Interface : thotlib_Interface.c

Document : thotlib_Document.c


thotlib_Interface.c : ../classes/thotlib/Interface.class
	kaffeh -stubs  thotlib.Interface

../classes/thotlib/Interface.class : ../classes/thotlib/Interface.java
	cd ../classes/thotlib ; javac Interface.java

thotlib_Document.c : ../classes/thotlib/Document.class
	kaffeh -stubs  thotlib.Document

../classes/thotlib/Document.class : ../classes/thotlib/Document.java
	cd ../classes/thotlib ; javac Document.java
