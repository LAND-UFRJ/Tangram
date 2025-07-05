lib_LTLIBRARIES = libhmm_batch_variable.la

libhmm_batch_variable_la_SOURCES= hmm_batch.cpp hmm_batch.h \
                                  hmm_batch_io.cpp          \
                                  hmm_batch_param.cpp       \
                                  hmm_batch_struct.cpp      \
                                  train.c train.h           \
                                  train_fast.c train_fast.h \
                                  viterbi.c viterbi.h       \
                                  param.c param.h           \
                                  transient.c transient.h   \
                                  forward.c forward.h       \
                                  backward.c backward.h     \
                                  batches.c batches.h
                                  

if MACOSX
PLUGIN_FILES = .libs/libhmm_batch_variable.dylib*
else
PLUGIN_FILES = .libs/libhmm_batch_variable.so*
endif

HMM_COMMON_OBJS = matrix.o   \
                  random.o   \
                  tangram2.o

libhmm_batch_variable_la_DEPENDENCIES= ${HMM_COMMON_OBJS}

include ../Makefile.inc

INCLUDES += -I../hmm_common          \
            -I${LIB_TANGRAM_MTK_INC} \
            -I${LIB_MATRIX_INC}

AM_LDFLAGS += ${HMM_COMMON_OBJS}

# Stores hmm_common object files in the current directory, since this code
# invokes plugin's headers
${HMM_COMMON_OBJS}:
	$(CC) $(AM_CXXFLAGS) -fPIC -c ../hmm_common/$*.c -o $@
