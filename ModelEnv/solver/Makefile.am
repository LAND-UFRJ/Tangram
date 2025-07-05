bin_PROGRAMS = solv
noinst_LIBRARIES = libsolv.a

solv_SOURCES = solv.c interfaces_solvers.c convergence.c gauss_siedel.c jacobi.c \
               sor.c power.c gth_no_block.c gth.c uniformization.c breward.c \
               interface_breward.c bounds.c parameters.c \
               interface_omega.c operational_time.c det_event.c \
               expected_trans_reward.c expected_trans_imp_rew.c ImpMatrix.c\
               rate_cumulative_approximation.c cumulative_reward_iterative.c\
               sor-flex.c iterative_methods_reward.c functions_espa.c\
               espa_direct.c espa_iterative.c\
               interface_breward.h bounds.h parameters.h \
               interface_omega.h det_event.h \
               ImpMatrix.h functions_espa_esra.h structs.h pmf.h

libsolv_a_SOURCES = $(solv_SOURCES)
libsolv_a_CPPFLAGS = -I$(top_srcdir)/ModelEnv/libMatrix/ -O6 -g -Wall

solv_CPPFLAGS = -I$(top_srcdir)/ModelEnv/libMatrix/ -O6 -g -Wall

solv_LDADD = -L$(top_srcdir)/ModelEnv/libMatrix/ -lMatrix
