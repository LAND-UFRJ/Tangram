lib_LTLIBRARIES = libfloatvalue.la

libfloatvalue_la_SOURCES= floatvalue.cpp floatvalue.h

if MACOSX
PLUGIN_FILES = .libs/libfloatvalue.dylib*
else
PLUGIN_FILES = .libs/libfloatvalue.so*
endif

ARIMA_COMMON_OBJS = acf.o

libfloatvalue_la_DEPENDENCIES= ${ARIMA_COMMON_OBJS}

include ../Makefile.inc

INCLUDES += -I../arima_common

AM_LDFLAGS += ${ARIMA_COMMON_OBJS}

# Stores arima_common object files in the current directory, since this code
# invokes plugin's headers
${ARIMA_COMMON_OBJS}:
	$(CC) $(AM_CXXFLAGS) -fPIC -c ../arima_common/$*.c -o $@
