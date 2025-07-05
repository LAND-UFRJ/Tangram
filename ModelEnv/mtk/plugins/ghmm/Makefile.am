lib_LTLIBRARIES = libghmm.la

libghmm_la_SOURCES= ghmm.cpp ghmm.h           \
                    ghmm_io.cpp               \
                    ghmm_param.cpp            \
                    ghmm_struct.cpp           \
                    train.c train.h           \
                    train_fast.c train_fast.h \
                    viterbi.c viterbi.h       \
                    acf.c                     \
                    param.c param.h

if MACOSX
PLUGIN_FILES = .libs/libghmm.dylib*
else
PLUGIN_FILES = .libs/libghmm.so*
endif


HMM_COMMON_OBJS = matrix.o    \
                  random.o    \
                  steady.o    \
                  transient.o \
                  tangram2.o  \
                  forward.o   \
                  backward.o

libghmm_la_DEPENDENCIES= ${HMM_COMMON_OBJS}

include ../Makefile.inc

INCLUDES += -I../hmm_common          \
            -I${LIB_TANGRAM_MTK_INC} \
            -I${LIB_MATRIX_INC}

AM_LDFLAGS += ${HMM_COMMON_OBJS}

# Stores hmm_common object files in the current directory, since this code
# invokes plugin's headers
${HMM_COMMON_OBJS}:
	$(CC) $(AM_CXXFLAGS) -fPIC -c ../hmm_common/$*.c -o $@
