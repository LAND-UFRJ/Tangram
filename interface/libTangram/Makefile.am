lib_LTLIBRARIES = libTangram.la

libTangram_la_SOURCES = Processo.c
libTangram_la_CFLAGS = -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux

# Needs to build Processo.h
BUILT_SOURCES = Processo.h

Processo.h:
	$(JAVAC) Processo.java
	$(JAVAH) -classpath . -jni Processo

EXTRA_DIST = Processo.java

CLEANFILES = Processo.class Processo.h
