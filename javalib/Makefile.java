#
# Makefile fragment when embedding the Java Kaffe interpreter
#

ZIPS=amaya.zip biss.zip classes.zip jigsaw.zip sources.zip thotlib.zip
JAVA_BINS=javac javadoc kaffe kaffeh

AMAYA_JAVA_OPTIONS= -DAMAYA_JAVA

AMAYA_JAVA_INCLUDES= -I$(THOTDIR)/javalib -I$(THOTDIR)/javalib/f

AMAYA_JAVA_OBJ= \
	javaamaya.o 

AMAYA_JAVA_LIBS= ../javalib/libThotJava.a ../javalib/libAmayaJava.a \
   ../libkaffe_bissawt.a ../libkaffe_vm.a ../libkaffe_native.a \
   ../libkaffe_agent.a ../libkaffe_net.a

java_force :

../javalib/libThotJava.a ../javalib/libAmayaJava.a : java_force
	@(cd ../javalib ; make libThotJava libAmayaJava)

../libkaffe_bissawt.a ../libkaffe_vm.a ../libkaffe_native.a ../libkaffe_agent.a ../libkaffe_net.a :
	@(cd .. ;  make kaffe)

AMAYA_JAVA_INSTALL=amaya_java_install
AMAYA_JAVA_UNINSTALL=amaya_java_uninstall

amaya_java_install:
	@(if test ! -d $(bindir) ; then $(MKDIR) $(bindir) ; fi)
	(for i in $(JAVA_BINS) ; do \
	      $(INSTALL_BIN) ../bin/$$i $(bindir) ; \
	  done)
	@(if test ! -d $(datadir) ; then $(MKDIR) $(datadir) ; fi)
	@(if test ! -d $(datadir)/thot ; then $(MKDIR) $(datadir)/thot ; fi)
	@(if test ! -d $(datadir)/thot/classes ; then $(MKDIR) $(datadir)/thot/classes ; fi)
	(for i in $(ZIPS) ; do \
	      $(INSTALL_DATA) $(THOTDIR)/classes/$$i $(datadir)/thot/classes ; \
	  done)

amaya_java_uninstall:
	(for file in $(JAVA_BINS) ; do \
	      $(RM) -f $(bindir)/$$file ; \
	  done)
	$(RM) -rf $(datadir)/thot/classes

	
