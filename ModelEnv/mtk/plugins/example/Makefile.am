lib_LTLIBRARIES = libexample.la

libexample_la_SOURCES= example.cpp example.h

if MACOSX
PLUGIN_FILES = .libs/libexample.dylib*
else
PLUGIN_FILES = .libs/libexample.so*
endif

include ../Makefile.inc
