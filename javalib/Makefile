MACHINE=`../batch/MachineType`

all : Interface Document Application Tree Presentation Language Registry Pivot


Interface : thotlib_Interface.c thotlib_Interface.h thotlib_Interface_stubs.c \
            thotlib_Interface_stubs.h thotlib_Interface_native.java

thotlib_Interface_stubs.c thotlib_Interface_stubs.h thotlib_Interface_native.java :\
  ../thotlib/include/interface.h
	javastub -C thotlib_Interface ../thotlib/include/interface.h

thotlib_Interface.c thotlib_Interface.h : ../classes/thotlib/Interface.class
	kaffeh -stubs  thotlib.Interface
	kaffeh thotlib.Interface

../classes/thotlib/Interface.class : ../classes/thotlib/Interface.java
	cd ../classes/thotlib ; javac Interface.java


Document : thotlib_Document.c thotlib_Document.h thotlib_Document_stubs.c \
            thotlib_Document_stubs.h thotlib_Document_native.java

thotlib_Document_stubs.c thotlib_Document_stubs.h thotlib_Document_native.java :\
  ../thotlib/include/document.h
	javastub -C thotlib_Document ../thotlib/include/document.h

thotlib_Document.c thotlib_Document.h : ../classes/thotlib/Document.class
	kaffeh -stubs  thotlib.Document
	kaffeh thotlib.Document

../classes/thotlib/Document.class : ../classes/thotlib/Document.java
	cd ../classes/thotlib ; javac Document.java

Application : thotlib_Application.c thotlib_Application.h thotlib_Application_stubs.c \
            thotlib_Application_stubs.h thotlib_Application_native.java

thotlib_Application_stubs.c thotlib_Application_stubs.h thotlib_Application_native.java :\
  ../thotlib/include/application.h
	javastub -C thotlib_Application ../thotlib/include/application.h

thotlib_Application.c thotlib_Application.h : ../classes/thotlib/Application.class
	kaffeh -stubs  thotlib.Application
	kaffeh thotlib.Application

../classes/thotlib/Application.class : ../classes/thotlib/Application.java
	cd ../classes/thotlib ; javac Application.java


Tree : thotlib_Tree.c thotlib_Tree.h thotlib_Tree_stubs.c \
            thotlib_Tree_stubs.h thotlib_Tree_native.java

thotlib_Tree_stubs.c thotlib_Tree_stubs.h thotlib_Tree_native.java :\
  ../thotlib/include/tree.h
	javastub -C thotlib_Tree ../thotlib/include/tree.h

thotlib_Tree.c thotlib_Tree.h : ../classes/thotlib/Tree.class
	kaffeh -stubs  thotlib.Tree
	kaffeh thotlib.Tree

../classes/thotlib/Tree.class : ../classes/thotlib/Tree.java
	cd ../classes/thotlib ; javac Tree.java


Presentation : thotlib_Presentation.c thotlib_Presentation.h thotlib_Presentation_stubs.c \
            thotlib_Presentation_stubs.h thotlib_Presentation_native.java

thotlib_Presentation_stubs.c thotlib_Presentation_stubs.h thotlib_Presentation_native.java :\
  ../thotlib/include/presentation.h
	javastub -C thotlib_Presentation ../thotlib/include/presentation.h

thotlib_Presentation.c thotlib_Presentation.h : ../classes/thotlib/Presentation.class
	kaffeh -stubs  thotlib.Presentation
	kaffeh thotlib.Presentation

../classes/thotlib/Presentation.class : ../classes/thotlib/Presentation.java
	cd ../classes/thotlib ; javac Presentation.java


Language : thotlib_Language.c thotlib_Language.h thotlib_Language_stubs.c \
            thotlib_Language_stubs.h thotlib_Language_native.java

thotlib_Language_stubs.c thotlib_Language_stubs.h thotlib_Language_native.java :\
  ../thotlib/include/language.h
	javastub -C thotlib_Language ../thotlib/include/language.h

thotlib_Language.c thotlib_Language.h : ../classes/thotlib/Language.class
	kaffeh -stubs  thotlib.Language
	kaffeh thotlib.Language

../classes/thotlib/Language.class : ../classes/thotlib/Language.java
	cd ../classes/thotlib ; javac Language.java


Registry : thotlib_Registry.c thotlib_Registry.h thotlib_Registry_stubs.c \
            thotlib_Registry_stubs.h thotlib_Registry_native.java

thotlib_Registry_stubs.c thotlib_Registry_stubs.h thotlib_Registry_native.java :\
  ../thotlib/include/registry.h
	javastub -C thotlib_Registry ../thotlib/include/registry.h

thotlib_Registry.c thotlib_Registry.h : ../classes/thotlib/Registry.class
	kaffeh -stubs  thotlib.Registry
	kaffeh thotlib.Registry

../classes/thotlib/Registry.class : ../classes/thotlib/Registry.java
	cd ../classes/thotlib ; javac Registry.java


Pivot : thotlib_Pivot.c thotlib_Pivot.h thotlib_Pivot_stubs.c \
            thotlib_Pivot_stubs.h thotlib_Pivot_native.java

thotlib_Pivot_stubs.c thotlib_Pivot_stubs.h thotlib_Pivot_native.java :\
  ../thotlib/include/pivot.h
	javastub -C thotlib_Pivot ../thotlib/include/pivot.h

thotlib_Pivot.c thotlib_Pivot.h : ../classes/thotlib/Pivot.class
	kaffeh -stubs  thotlib.Pivot
	kaffeh thotlib.Pivot

../classes/thotlib/Pivot.class : ../classes/thotlib/Pivot.java
	cd ../classes/thotlib ; javac Pivot.java


