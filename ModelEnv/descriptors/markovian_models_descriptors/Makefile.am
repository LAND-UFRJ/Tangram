bin_PROGRAMS = descriptors

descriptors_SOURCES = operational_time.c burst_initial_prob.c interface_omega.c\
		      descriptors.c

descriptors_CPPFLAGS = -Wall -g -I$(top_srcdir)/ModelEnv/libMatrix -I$(top_srcdir)/ModelEnv/solver

descriptors_LDADD = -L$(top_srcdir)/ModelEnv/libMatrix -lMatrix -L$(top_srcdir)/ModelEnv/solver -lsolv
