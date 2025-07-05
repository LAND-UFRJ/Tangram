bin_PROGRAMS = pmf

pmf_SOURCES = pmf.c calcul.c output.c readfiles.c btools.c formulc.c random.c\
              pmf.h formulc.h stdarg.h.sun standalone.c

pmf_CPPFLAGS = -Wall -O -g -I$(JAVA_HOME)include -I$(JAVA_HOME)include/linux

if SUN_OS
   pmf_CPPFLAGS += -D_SOLARIS_ -I$(JAVA_HOME)include/solaris
endif
