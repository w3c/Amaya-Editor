# package.make
# $Id$
# COPYRIGHT

# This makefile should be included in all packages Makefiles. To use it, define
# the PACKAGES variable to the set of packages defined in your directory,
# and the PACKAGE variable to this package name.
# So, if you have a 'foo' package, included in 'w3c' and containing 'bar1'
# and 'bar2' sub packages, your Makefile should look like this:
# ----------
# PACKAGE=w3c.foo
# PACKAGES=bar1 bar2
# include $(MAKEDIR)/package.make
# ----------
#
# This make file defines the following targets:
# all:	 to build the class files from the java files.
# clean: to clean all sub packages
# doc:   to build the appropriate documentation files from the source
# The 'doc' target uses DESTDIR variable that should point to the absolute 
# path of the target directory (in which doc files will be created).

all::
	@@for p in $(PACKAGES); do \
		echo 'building ' $(PACKAGE).$$p; \
		if [ -z "$(TARGET)" ]; then \
			(cd $$p; make MAKEDIR=$(MAKEDIR)); \
		else \
			(cd $$p; make MAKEDIR=$(MAKEDIR) VPATH=$(VPATH)/$$p); \
		fi \
	done

doc::
	@@for p in $(PACKAGES); do \
		echo 'doc ' $(PACKAGE).$$p; \
		(cd $$p; make MAKEDIR=$(MAKEDIR) DESTDIR=$(DESTDIR) doc); \
	done

clean::
	@@for p in $(PACKAGES); do \
		echo 'cleaning ' $(PACKAGE).$$p; \
		(cd $$p ; make MAKEDIR=$(MAKEDIR) clean) ; \
	done

