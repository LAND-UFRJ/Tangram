bin_PROGRAMS = trans_param comp_matrix

trans_param_SOURCES = formulc.c random.c trans_param.c formulc.h
trans_param_CPPFLAGS = -O -g -Wall -I$(top_srcdir)/ModelEnv/libMatrix
trans_param_LDADD = -L$(top_srcdir)/ModelEnv/libMatrix -lMatrix

comp_matrix_SOURCES = comp_matrix.c
comp_matrix_CPPFLAGS = -O -g -Wall -I$(top_srcdir)/ModelEnv/libMatrix
comp_matrix_LDADD = -L$(top_srcdir)/ModelEnv/libMatrix -lMatrix
