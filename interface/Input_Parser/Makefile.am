lib_LTLIBRARIES = libInput_parser.la

libInput_parser_la_SOURCES = Entrada.c
libInput_parser_la_CFLAGS = -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux

# Needs to build Entrada.h
BUILT_SOURCES = Entrada.h

Entrada.h:
	$(JAVAC) -h . Entrada.java

EXTRA_DIST = Entrada.java
CLEANFILES = Entrada.h Entrada.class
