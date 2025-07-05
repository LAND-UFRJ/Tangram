lib_LTLIBRARIES = libma.la

libma_la_SOURCES= ma.cpp ma.h

if MACOSX
PLUGIN_FILES = .libs/libma.dylib*
else
PLUGIN_FILES = .libs/libma.so*
endif

ARIMA_COMMON_OBJS = white_noise.o

libma_la_DEPENDENCIES= ${ARIMA_COMMON_OBJS}

include ../Makefile.inc

INCLUDES += -I../arima_common

AM_LDFLAGS += ${ARIMA_COMMON_OBJS}

# Stores arima_common object files in the current directory, since this code
# invokes plugin's headers
${ARIMA_COMMON_OBJS}:
	$(CC) $(AM_CXXFLAGS) -fPIC -c ../arima_common/$*.c -o $@
