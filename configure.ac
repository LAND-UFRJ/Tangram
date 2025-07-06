#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(Tangram2,3.5.1,support@land.ufrj.br)
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([ModelEnv/pmf/formulc.c])
AC_CONFIG_HEADER([config.h])

# Checking host
AC_CANONICAL_HOST

AM_CONDITIONAL(OTHER_OS, false)
AM_CONDITIONAL(LINUX_OS, false)
AM_CONDITIONAL(SUN_OS, false)
AM_CONDITIONAL(MACOSX, false)
case $host_os in
  *darwin* ) AM_CONDITIONAL(MACOSX, true);;
  *sun* ) AM_CONDITIONAL(SUN_OS, true);;
  *linux* ) AM_CONDITIONAL(LINUX_OS, true);;
  * ) AM_CONDITIONAL(OTHER_OS, true);;
esac

# Changing prefix to prefix/Tangram2
if test "$prefix" = NONE; then
    prefix="/usr/local/Tangram2"
else
    prefix="$prefix/Tangram2"
fi
echo "Changing Tangram2's prefix to $prefix"

# Checks for arguments
AC_ARG_ENABLE( parallel, [  --enable-parallel       Enables Tangram2 parallel(PVM)] )
AC_ARG_ENABLE( atm,      [  --enable-atm            Enables Tangram2 with ATM support] )
AC_ARG_ENABLE( latex,    [  --enable-latex          Enables checking for latex] )
AC_ARG_WITH( tgif-dir,   [  --with-tgif-dir=PATH    tgif path directory] )
AC_ARG_WITH( mtkgui,     [  --without-mtkgui        do not build MTK graphical interface])

# Decides do build MTK gui
if test "$with_mtkgui" = ""; then
    with_mtkgui="yes"
fi

if test "$with_mtkgui" = "yes"; then 
    AM_CONDITIONAL( HAVE_MTKGUI, true )

    # Checks for QT: http://autoqt.sourceforge.net/
    #gw_CHECK_QT
    # TODO: configure crashes with --without-mtkgui option
    
    ### Check QT by hand :( ###
    # Check QTDIR
    QT_FAIL="no"
    AC_MSG_CHECKING([Qt installation directory])    
    if test -f $QTDIR/include/qglobal.h; then
        HAVE_QT_DIR="yes"
    else
        HAVE_QT_DIR="no"
        QT_FAIL="yes"
    fi
    AC_MSG_RESULT([$HAVE_QT_DIR])    
    
    if test $HAVE_QT_DIR == "no"; then
        AC_MSG_ERROR( Environment variable QTDIR not set... Please point this variable to your QT installation directory. )
    fi
    
    # Check moc
    AC_CHECK_PROG( MOC, moc, [yes], [no] )
    if test x$MOC = x ; then
        AC_MSG_ERROR([*** moc must be in path])
        QT_FAIL="yes"
    fi

    # Check multi-thread support    
    AC_MSG_CHECKING([Qt multi-thread support])    
    if test "x`ls $QTDIR/lib/libqt*-mt.* 2> /dev/null`" != x; then
        HAVE_QT_MT="yes"
    else
        HAVE_QT_MT="no"
        QT_FAIL="yes"        
    fi
    AC_MSG_RESULT([$HAVE_QT_MT])    
    
    if test $HAVE_QT_MT == "no"; then    
        AC_MSG_ERROR( Your QT installation is not multi-threaded... Please recompile QT with multi-thread support. )    
    fi
    
    if test $QT_FAIL == "yes"; then
        AC_MSG_ERROR( To build without MTK GUI support, set --without-mtkgui flag. )
    fi
else
    AM_CONDITIONAL( HAVE_MTKGUI, false )
fi

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_YACC
if test "$YACC" == "yacc"; then
    AC_CHECK_PROG([HAVE_YACC], [yacc], [yes], [no])
    if test "$HAVE_YACC" = "no"; then
        AC_MSG_ERROR(yacc or bison not found ... please install yacc or bison.)
    fi
fi
AC_PROG_LEX
if test "$LEX" != flex && test "$LEX" != lex; then
  AC_MSG_ERROR(lex or flex not found ... please install lex or flex.)
fi
AC_PROG_LN_S
AC_PROG_LIBTOOL

# TGIF
AC_CHECK_PROG([HAVE_TGIF], [tgif], [yes], [no])
if test "$HAVE_TGIF" = "no"; then
    AC_CHECK_FILE( $with_tgif_dir/tgif, HAVE_TGIF="yes", HAVE_TGIF="no" )
    if test "$HAVE_TGIF" = "no"; then
        AC_MSG_ERROR(tgif not found ... please make sure you have tgif installed. - http://bourbon.usc.edu:8001/tgif/
            hint: maybe you must to set --with-tgif-dir with tgif full path.)
    fi
fi

# XTERM
AC_CHECK_PROG([HAVE_XTERM], [xterm], [yes], [no])
if test "$HAVE_XTERM" = "no"; then
    AC_MSG_ERROR(xterm not found ... please install xterm.)
fi

# JAVAC  
AC_CHECK_FILE( $JAVA_HOME/bin/javac, HAVE_JAVAC="yes", HAVE_JAVAC="no" )
if test "$HAVE_JAVAC" = "no"; then
    AC_MSG_ERROR(javac not found ... please install jsdk - java.sun.com.
            hint: verify JAVA_HOME environment variable if jsdk has been already installed.)
else
    AC_SUBST([JAVAC],[$JAVA_HOME/bin/javac])
fi

# JAR
AC_CHECK_FILE( $JAVA_HOME/bin/jar, HAVE_JAR="yes", HAVE_JAR="no" )
if test "$HAVE_JAR" = "no"; then
    AC_MSG_ERROR(jar not found ... please install jsdk - java.sun.com)
else
    AC_SUBST([JAR],[$JAVA_HOME/bin/jar])
fi

# JAVA
AC_CHECK_FILE( $JAVA_HOME/bin/java, HAVE_JAVA="yes", HAVE_JAVA="no" )
if test "$HAVE_JAVA" = "no"; then
    AC_MSG_ERROR(java not found ... please install jsdk - java.sun.com)
fi

# Checking only when building documentation
# Latex
if test "$enable_latex" = "yes"; then
    AC_CHECK_PROG([HAVE_LATEX], [latex], [yes], [no])
    if test "$HAVE_LATEX" = "no"; then
        AC_MSG_ERROR(latex not found ... please install latex.)
    fi
    AC_CHECK_PROG([HAVE_DVIPDFM], [dvipdfm], [yes], [no])
    if test "$HAVE_DVIPDFM" = "no"; then
        AC_MSG_ERROR(dvipdfm not found ... please install dvipdfm.)
    fi   
fi

# SCP
AC_PROG_SCP

# SSH
AC_PROG_SSH

# Gnuplot
AC_CHECK_PROG([HAVE_GNUPLOT], [gnuplot], [yes], [no])
if test "$HAVE_GNUPLOT" = "no"; then
    AC_MSG_ERROR(gnuplot not found ... please install gnuplot. http://www.gnuplot.info/)
fi

# XPVM
if test "$enable_parallel" = "yes"; then
   AC_CHECK_PROG([HAVE_XPVM], [xpvm], [yes], [no])
   if test "$HAVE_XPVM" = "no"; then
       AC_MSG_ERROR(xpvm not found ... please install xpvm.)
  fi
fi

# Checks for libraries.
# X11
AC_PATH_X
if test "$no_x" = "yes"; then
    ERROR_MESSAGE="Can't locate locate the X Window System include files and libraries. Try the command line options --x-includes and --x-libraries"
    AC_MSG_ERROR($ERROR_MESSAGE)
fi
AC_PATH_XTRA
X_EXTRA_LIBS="$X_EXTRA_LIBS -lX11"
CFLAGS="$CFLAGS $X_CFLAGS"
LIBS="$X_LIBS $LIBS -lX11 $X_EXTRA_LIBS"

# Pthread
AC_CHECK_LIB([pthread], [pthread_create])

# Math
AC_CHECK_LIB([m], [sqrt])

# Flex
AC_CHECK_LIB([fl], [yywrap])

# Readline
AC_CHECK_LIB([readline], [read_history] )

# PVM3 only if --enable-parallel
if test "$enable_parallel" = "yes"; then
    AC_CHECK_LIB([pvm3], [pvm_config],[], AC_MSG_ERROR(libpvm3 not found ... please install pvm3.
		 hint: check if pvm and pvm-devel is installed.),[])
fi

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([arpa/inet.h fcntl.h inttypes.h netdb.h netinet/in.h stddef.h stdlib.h string.h strings.h sys/ioctl.h sys/param.h sys/socket.h sys/time.h unistd.h values.h])

# Xt
AC_CHECK_HEADERS([X11/StringDefs.h],[X_EXTRA_LIBS="$X_EXTRA_LIBS -lXt"],AC_MSG_ERROR(Xt headers not found ... please install libXt.),[])

# Xaw
AC_CHECK_HEADERS([X11/Xaw/Command.h],[X_EXTRA_LIBS="$X_EXTRA_LIBS -lXaw"],AC_MSG_ERROR(Xaw headers not found ... please install libXaw.),[])

# Readline
AC_CHECK_HEADERS([readline/readline.h readline/history.h],[],AC_MSG_ERROR(Readline headers not found ... please install libreadline-dev.),[])

# PVM3
if test "$enable_parallel" = "yes"; then
    AC_CHECK_HEADERS([pvm3.h],[AM_CONDITIONAL(HAVE_PVM, true)],AC_MSG_ERROR(pvm3 headers not found ... please install pvm3.),[])
else
    AM_CONDITIONAL(HAVE_PVM, false)
fi

# ATM
if test "$enable_atm" = "yes"; then
    AC_CHECK_HEADERS([atm.h],[AM_CONDITIONAL(HAVE_ATM, true)],AC_MSG_ERROR(atm headers not found ... please install atm.),[])
else
    AM_CONDITIONAL(HAVE_ATM, false)
fi

# JNI
AC_CHECK_FILE([$JAVA_HOME/include/jni.h],[], AC_MSG_ERROR(cannot find java headers ... not able to build interface),[])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_FUNC_STRTOD
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([alarm atexit bzero floor gethostbyname gettimeofday inet_ntoa memset pow putenv rint select socket sqrt strcasecmp strchr strrchr strstr])

AC_CONFIG_FILES([Makefile
                 ModelEnv/Makefile
                 ModelEnv/CAC_Algorithms/Makefile
                 ModelEnv/animplot/Makefile
                 ModelEnv/avgReward/Makefile
                 ModelEnv/chain_generator/Makefile
                 ModelEnv/descriptors/Makefile
                 ModelEnv/descriptors/markovian_models_descriptors/Makefile
                 ModelEnv/descriptors/traces_descriptors/Makefile
                 ModelEnv/histogram_mtx_generator/Makefile
                 ModelEnv/getNames/Makefile
                 ModelEnv/grammar/Makefile
                 ModelEnv/hash_table/Makefile
                 ModelEnv/libMatrix/Makefile
                 ModelEnv/pmf/Makefile
                 ModelEnv/solver/Makefile
                 ModelEnv/mtx_to_obj/Makefile
                 ModelEnv/mtk/Makefile
                 ModelEnv/mtk/libmtk/Makefile
                 ModelEnv/mtk/plugins/Makefile
                 ModelEnv/mtk/plugins/libTangramMTK/Makefile
                 ModelEnv/mtk/plugins/example/Makefile
                 ModelEnv/mtk/plugins/intvalue/Makefile
                 ModelEnv/mtk/plugins/floatvalue/Makefile
                 ModelEnv/mtk/plugins/ma/Makefile
                 ModelEnv/mtk/plugins/ghmm/Makefile
                 ModelEnv/mtk/plugins/hmm/Makefile
                 ModelEnv/mtk/plugins/hmm_batch/Makefile
                 ModelEnv/mtk/plugins/hmm_batch_variable/Makefile
                 ModelEnv/mtk/gui/Makefile
                 ModelEnv/trans_param/Makefile
                 ModelEnv/mtx_vis-st_permut/Makefile
                 ModelEnv/spmd/Makefile
                 ModelEnv/end_user_needed/Makefile
                 TraffGen/Makefile
                 TraffGen/Traffgen_IP/Makefile
                 TraffGen/Traffgen_ATM/Makefile 
                 TraffGen/Traffgen_ATM/Linux/Makefile
                 interface/Makefile
                 interface/Tangram2_Interface/Makefile
                 interface/libTangram/Makefile
                 interface/Input_Parser/Makefile
                 scripts/Makefile
                 etc/Makefile
                 Domain/Makefile
                 examples/Makefile])
AC_OUTPUT
