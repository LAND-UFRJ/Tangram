lib_LTLIBRARIES = libhmm.la

libhmm_la_SOURCES= hmm.cpp hmm.h           \
                   hmm_io.cpp              \
                   hmm_param.cpp           \
                   param.c param.h         \
                   hmm_util.c hmm_util.h   \
                   train.c train.h         \
                   transient.c transient.h \
                   viterbi.c viterbi.h     \
                   forecast.c forecast.h   \
                   forward.c forward.h     \
                   hmm_struct.cpp          \
                   steady.c steady.h

if MACOSX
PLUGIN_FILES = .libs/libhmm.dylib*
else
PLUGIN_FILES = .libs/libhmm.so*
endif

HMM_COMMON_OBJS =  matrix.o    \
                   random.o    \
                   tangram2.o

libhmm_la_DEPENDENCIES= ${HMM_COMMON_OBJS}

include ../Makefile.inc

INCLUDES += -I../hmm_common         \
            -I${LIB_TANGRAM_MTK_INC}

AM_LDFLAGS += ${HMM_COMMON_OBJS}

# Stores hmm_common object files in the current directory, since this code
# invokes plugin's headers
${HMM_COMMON_OBJS}:
	$(CC) $(AM_CXXFLAGS) -fPIC -c ../hmm_common/$*.c -o $@

