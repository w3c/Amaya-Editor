# file.make
# $Id$
# COPYRIGHT

# This makefile should be included in all leaves packages directory. It uses
# the FILES variable to know what are the files to be compiled.
# For example, if you have a package 'foo' containing 'a.java' and 'b.java'
# your Makefile should look like this:
# ----------
# PACKAGE=foo
# FILES=a.java b.java
# include $(MAKEDIR)/files.make
# ----------
#
# This file defines the following targets:
# all:	 to build the class files from the java files.
# clean: to clean all sub packages
# doc:   to build the appropriate documentation files from the source
# The 'doc' target uses DESTDIR variable that should point to the absolute 
# path of the target directory (in which doc files will be created).

.SUFFIXES: .java .class

.java.class:
	@@echo "Compiling " $< ; \
	if [ -z "$(TARGET)" ]; then \
		javac $(JAVAFLAGS) $<; \
	else \
		javac -d $(TARGET) $(JAVAFLAGS) $<; \
	fi

all:: $(FILES:.java=.class)

doc::
	javadoc -d $(DESTDIR) $(FILES)

clean::
	@@rm -rf *~ *.class
