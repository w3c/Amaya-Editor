#
# Makefile fragment when embedding the Java Kaffe interpreter
#

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

