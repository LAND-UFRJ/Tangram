#libMatrix
LIB_MATRIX_DIR   = $(top_srcdir)/ModelEnv/libMatrix

bin_PROGRAMS = avgReward

avgReward_SOURCES = avgReward.cpp 

avgReward_CPPFLAGS = -I${LIB_MATRIX_DIR} -O6 -g -Wall

avgReward_LDFLAGS = -lMatrix -L${LIB_MATRIX_DIR}
