lib_LTLIBRARIES = libintvalue.la

libintvalue_la_SOURCES= intvalue.cpp intvalue.h

if MACOSX
PLUGIN_FILES = .libs/libintvalue.dylib*
else
PLUGIN_FILES = .libs/libintvalue.so*
endif

ARIMA_COMMON_OBJS = acf.o

libintvalue_la_DEPENDENCIES= ${ARIMA_COMMON_OBJS}

include ../Makefile.inc

INCLUDES += -I../arima_common

AM_LDFLAGS += ${ARIMA_COMMON_OBJS}

# Stores arima_common object files in the current directory, since this code
# invokes plugin's headers
${ARIMA_COMMON_OBJS}:
	$(CC) $(AM_CXXFLAGS) -fPIC -c ../arima_common/$*.c -o $@
