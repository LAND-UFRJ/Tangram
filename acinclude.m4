######## autoqt begin ########

# Check for Qt compiler flags, linker flags, and binary packages
AC_DEFUN([gw_CHECK_QT],
[
AC_REQUIRE([AC_PROG_CXX])
AC_REQUIRE([AC_PATH_X])

AC_MSG_CHECKING([QTDIR])
AC_ARG_WITH([qtdir], [  --with-qtdir=DIR        Qt installation directory [default=$QTDIR]], QTDIR=$withval)
# Check that QTDIR is defined or that --with-qtdir given
if test x"$QTDIR" = x ; then
    QT_SEARCH="/usr/lib/qt31 /usr/local/qt31 /usr/lib/qt3 /usr/local/qt3 /usr/lib/qt2 /usr/local/qt2 /usr/lib/qt /usr/local/qt"
    for i in $QT_SEARCH; do
        if test -f $i/include/qglobal.h -a x$QTDIR = x; then QTDIR=$i; fi
    done
fi
if test x"$QTDIR" = x ; then
    AC_MSG_ERROR([*** QTDIR must be defined, or --with-qtdir option given])
fi
AC_MSG_RESULT([$QTDIR])

# Change backslashes in QTDIR to forward slashes to prevent escaping
# problems later on in the build process, mainly for Cygwin build
# environment using MSVC as the compiler
# TODO: Use sed instead of perl
QTDIR=`echo $QTDIR | perl -p -e 's/\\\\/\\//g'`

# Figure out which version of Qt we are using
AC_MSG_CHECKING([Qt version])
QT_VER=`grep 'define.*QT_VERSION_STR\W' $QTDIR/include/qglobal.h | perl -p -e 's/\D//g'`
case "${QT_VER}" in
    2*)
        QT_MAJOR="2"
    ;;
    3*)
        QT_MAJOR="3"
    ;;
    *)
        AC_MSG_ERROR([*** Don't know how to handle this Qt major version])
    ;;
esac
AC_MSG_RESULT([$QT_VER ($QT_MAJOR)])

# Check that moc is in path
AC_CHECK_PROG(MOC, moc, moc)
if test x$MOC = x ; then
        AC_MSG_ERROR([*** moc must be in path])
fi

# uic is the Qt user interface compiler
AC_CHECK_PROG(UIC, uic, uic)
if test x$UIC = x ; then
        AC_MSG_ERROR([*** uic must be in path])
fi

# qembed is the Qt data embedding utility.
# It is located in $QTDIR/tools/qembed, and must be compiled and installed
# manually, we'll let it slide if it isn't present
AC_CHECK_PROG(QEMBED, qembed, qembed)


# Calculate Qt include path
QT_CXXFLAGS="-I$QTDIR/include"

QT_IS_EMBEDDED="no"
# On unix, figure out if we're doing a static or dynamic link
case "${host}" in
    *-cygwin)
	AC_DEFINE_UNQUOTED(WIN32, "", Defined if on Win32 platform)
        if test -f "$QTDIR/lib/qt.lib" ; then
            QT_LIB="qt.lib"
            QT_IS_STATIC="yes"
            QT_IS_MT="no"
        elif test -f "$QTDIR/lib/qt-mt.lib" ; then
            QT_LIB="qt-mt.lib" 
            QT_IS_STATIC="yes"
            QT_IS_MT="yes"
        elif test -f "$QTDIR/lib/qt$QT_VER.lib" ; then
            QT_LIB="qt$QT_VER.lib"
            QT_IS_STATIC="no"
            QT_IS_MT="no"
        elif test -f "$QTDIR/lib/qt-mt$QT_VER.lib" ; then
            QT_LIB="qt-mt$QT_VER.lib"
            QT_IS_STATIC="no"
            QT_IS_MT="yes"
        fi
        ;;

    *)
        QT_IS_STATIC=`ls $QTDIR/lib/*.a 2> /dev/null`
        if test "x$QT_IS_STATIC" = x; then
            QT_IS_STATIC="no"
        else
            QT_IS_STATIC="yes"
        fi
        if test x$QT_IS_STATIC = xno ; then
            QT_IS_DYNAMIC=`ls $QTDIR/lib/*.so 2> /dev/null` 
            if test "x$QT_IS_DYNAMIC" = x;  then
                AC_MSG_ERROR([*** Couldn't find any Qt libraries])
            fi
        fi

        if test "x`ls $QTDIR/lib/libqt.* 2> /dev/null`" != x ; then
            QT_LIB="-lqt"
            QT_IS_MT="no"
        elif test "x`ls $QTDIR/lib/libqt-mt.* 2> /dev/null`" != x ; then
            QT_LIB="-lqt-mt"
            QT_IS_MT="yes"
        elif test "x`ls $QTDIR/lib/libqte.* 2> /dev/null`" != x ; then
            QT_LIB="-lqte"
            QT_IS_MT="no"
            QT_IS_EMBEDDED="yes"
        elif test "x`ls $QTDIR/lib/libqte-mt.* 2> /dev/null`" != x ; then
            QT_LIB="-lqte-mt"
            QT_IS_MT="yes"
            QT_IS_EMBEDDED="yes"
        fi
        ;;
esac
AC_MSG_CHECKING([if Qt is static])
AC_MSG_RESULT([$QT_IS_STATIC])
AC_MSG_CHECKING([if Qt is multithreaded])
AC_MSG_RESULT([$QT_IS_MT])
AC_MSG_CHECKING([if Qt is embedded])
AC_MSG_RESULT([$QT_IS_EMBEDDED])

QT_GUILINK=""
QASSISTANTCLIENT_LDADD="-lqassistantclient"
case "${host}" in
    *irix*)
        QT_LIBS="$QT_LIB"
        if test $QT_IS_STATIC = yes ; then
            QT_LIBS="$QT_LIBS -L$x_libraries -lXext -lX11 -lm -lSM -lICE"
        fi
        ;;

    *linux*)
        QT_LIBS="$QT_LIB"
        if test $QT_IS_STATIC = yes && test $QT_IS_EMBEDDED = no; then
            QT_LIBS="$QT_LIBS -L$x_libraries -lXext -lX11 -lm -lSM -lICE -ldl -ljpeg"
        fi
        ;;


    *osf*) 
        # Digital Unix (aka DGUX aka Tru64)
        QT_LIBS="$QT_LIB"
        if test $QT_IS_STATIC = yes ; then
            QT_LIBS="$QT_LIBS -L$x_libraries -lXext -lX11 -lm -lSM -lICE"
        fi
        ;;

    *solaris*)
        QT_LIBS="$QT_LIB"
        if test $QT_IS_STATIC = yes ; then
            QT_LIBS="$QT_LIBS -L$x_libraries -lXext -lX11 -lm -lSM -lICE -lresolv -lsocket -lnsl"
        fi
        ;;


    *win*)
        # linker flag to suppress console when linking a GUI app on Win32
        QT_GUILINK="/subsystem:windows"

	if test $QT_MAJOR = "3" ; then
	    if test $QT_IS_MT = yes ; then
        	QT_LIBS="/nodefaultlib:libcmt"
            else
            	QT_LIBS="/nodefaultlib:libc"
            fi
        fi

        if test $QT_IS_STATIC = yes ; then
            QT_LIBS="$QT_LIBS $QT_LIB kernel32.lib user32.lib gdi32.lib comdlg32.lib ole32.lib shell32.lib imm32.lib advapi32.lib wsock32.lib winspool.lib winmm.lib netapi32.lib"
            if test $QT_MAJOR = "3" ; then
                QT_LIBS="$QT_LIBS qtmain.lib"
            fi
        else
            QT_LIBS="$QT_LIBS $QT_LIB"        
            if test $QT_MAJOR = "3" ; then
                QT_CXXFLAGS="$QT_CXXFLAGS -DQT_DLL"
                QT_LIBS="$QT_LIBS qtmain.lib qui.lib user32.lib netapi32.lib"
            fi
        fi
        QASSISTANTCLIENT_LDADD="qassistantclient.lib"
        ;;

esac


if test x"$QT_IS_EMBEDDED" = "xyes" ; then
        QT_CXXFLAGS="-DQWS $QT_CXXFLAGS"
fi

if test x"$QT_IS_MT" = "xyes" ; then
        QT_CXXFLAGS="$QT_CXXFLAGS -D_REENTRANT -DQT_THREAD_SUPPORT"
fi

QT_LDADD="-L$QTDIR/lib $QT_LIBS"

if test x$QT_IS_STATIC = xyes ; then
    OLDLIBS="$LIBS"
    LIBS="$QT_LDADD"
    AC_CHECK_LIB(Xft, XftFontOpen, QT_LDADD="$QT_LDADD -lXft")
    LIBS="$LIBS"
fi

AC_MSG_CHECKING([QT_CXXFLAGS])
AC_MSG_RESULT([$QT_CXXFLAGS])
AC_MSG_CHECKING([QT_LDADD])
AC_MSG_RESULT([$QT_LDADD])

AC_SUBST(QT_CXXFLAGS)
AC_SUBST(QT_LDADD)
AC_SUBST(QT_GUILINK)
AC_SUBST(QASSISTANTCLIENT_LDADD)

])

######## autoqt end ########

dnl GNU Autoconf Macros 

dnl AC_CHECK_JAVA_HOME
AC_DEFUN([AC_CHECK_JAVA_HOME],[
AC_REQUIRE([AC_EXEEXT])dnl
TRY_JAVA_HOME=`ls -dr /usr/java/* 2> /dev/null | head -n 1`
if test x$TRY_JAVA_HOME != x; then
        PATH=$PATH:$TRY_JAVA_HOME/bin
fi
AC_PATH_PROG(JAVA_PATH_NAME, java$EXEEXT)
if test x$JAVA_PATH_NAME != x; then
        JAVA_HOME=`echo $JAVA_PATH_NAME | sed "s/\(.*\)[[/]]bin[[/]]java$EXEEXT$/\1/"`
fi;dnl
])

dnl AC_PROG_JAVAC
AC_DEFUN([AC_PROG_JAVAC],[
AC_REQUIRE([AC_EXEEXT])dnl
if test "x$JAVAPREFIX" = x; then
        test "x$JAVAC" = x && AC_CHECK_PROGS(JAVAC, "gcj$EXEEXT -C" guavac$EXEEXT jikes$EXEEXT javac$EXEEXT)
else
        test "x$JAVAC" = x && AC_CHECK_PROGS(JAVAC, "gcj$EXEEXT -C" guavac$EXEEXT jikes$EXEEXT javac$EXEEXT, $JAVAPREFIX)
fi
test "x$JAVAC" = x && AC_MSG_ERROR([no acceptable Java compiler found in \$PATH])
AC_PROG_JAVAC_WORKS
AC_PROVIDE([$0])dnl
])

dnl AC_PROG_JAR
AC_DEFUN([AC_PROG_JAR],[
AC_REQUIRE([AC_EXEEXT])dnl
if test "x$JAVAPREFIX" = x; then
        test "x$JAR" = x && AC_CHECK_PROGS(JAR, jar$EXEEXT)
else
        test "x$JAR" = x && AC_CHECK_PROGS(JAR, jar, $JAVAPREFIX)
fi
test "x$JAR" = x && AC_MSG_ERROR([no acceptable jar program found in \$PATH])
AC_PROVIDE([$0])dnl
])

dnl AC_PROG_JAVAH
AC_DEFUN([AC_PROG_JAVAH],[
dnl AC_REQUIRE([AC_CANONICAL_SYSTEM])dnl
AC_REQUIRE([AC_PROG_CPP])dnl
AC_PATH_PROG(JAVAH,javah)
if test x"`eval 'echo $ac_cv_path_JAVAH'`" != x ; then
  AC_TRY_CPP([#include <jni.h>],,[
    ac_save_CPPFLAGS="$CPPFLAGS"
changequote(, )dnl
    ac_dir=`echo $ac_cv_path_JAVAH | sed 's,\(.*\)/[^/]*/[^/]*$,\1/include,'`
    ac_machdep=`echo $build_os | sed 's,[-0-9].*,,' | sed 's,cygwin,win32,'`
changequote([, ])dnl
    CPPFLAGS="$ac_save_CPPFLAGS -I$ac_dir -I$ac_dir/$ac_machdep"
    AC_TRY_CPP([#include <jni.h>],
               ac_save_CPPFLAGS="$CPPFLAGS",
               AC_MSG_WARN([unable to include <jni.h>]))
    CPPFLAGS="$ac_save_CPPFLAGS"])
fi])

dnl AC_PROG_JAVA
AC_DEFUN([AC_PROG_JAVA],[
AC_REQUIRE([AC_EXEEXT])dnl
if test x$JAVAPREFIX = x; then
        test x$JAVA = x && AC_CHECK_PROGS(JAVA, kaffe$EXEEXT java$EXEEXT)
else
        test x$JAVA = x && AC_CHECK_PROGS(JAVA, kaffe$EXEEXT java$EXEEXT, $JAVAPREFIX)
fi
test x$JAVA = x && AC_MSG_ERROR([no acceptable Java virtual machine found in \$PATH])
AC_PROG_JAVA_WORKS
AC_PROVIDE([$0])dnl
])

dnl AC_PROG_JAVA_WORKS
AC_DEFUN([AC_PROG_JAVA_WORKS], [
AC_CHECK_PROG(uudecode, uudecode$EXEEXT, yes)
if test x$uudecode = xyes; then
AC_CACHE_CHECK([if uudecode can decode base 64 file], ac_cv_prog_uudecode_base64, [
dnl /**
dnl  * Test.java: used to test if java compiler works.
dnl  */
dnl public class Test
dnl {
dnl
dnl public static void
dnl main( String[] argv )
dnl {
dnl     System.exit (0);
dnl }
dnl
dnl }
cat << \EOF > Test.uue
begin-base64 644 Test.class
yv66vgADAC0AFQcAAgEABFRlc3QHAAQBABBqYXZhL2xhbmcvT2JqZWN0AQAE
bWFpbgEAFihbTGphdmEvbGFuZy9TdHJpbmc7KVYBAARDb2RlAQAPTGluZU51
bWJlclRhYmxlDAAKAAsBAARleGl0AQAEKEkpVgoADQAJBwAOAQAQamF2YS9s
YW5nL1N5c3RlbQEABjxpbml0PgEAAygpVgwADwAQCgADABEBAApTb3VyY2VG
aWxlAQAJVGVzdC5qYXZhACEAAQADAAAAAAACAAkABQAGAAEABwAAACEAAQAB
AAAABQO4AAyxAAAAAQAIAAAACgACAAAACgAEAAsAAQAPABAAAQAHAAAAIQAB
AAEAAAAFKrcAErEAAAABAAgAAAAKAAIAAAAEAAQABAABABMAAAACABQ=
====
EOF
if uudecode$EXEEXT Test.uue; then
        ac_cv_prog_uudecode_base64=yes
else
        echo "configure: __oline__: uudecode had trouble decoding base 64 file 'Test.uue'" >&AC_FD_CC
        echo "configure: failed file was:" >&AC_FD_CC
        cat Test.uue >&AC_FD_CC
        ac_cv_prog_uudecode_base64=no
fi
rm -f Test.uue])
fi
if test x$ac_cv_prog_uudecode_base64 != xyes; then
        rm -f Test.class
        AC_MSG_WARN([I have to compile Test.class from scratch])
        if test x$ac_cv_prog_javac_works = xno; then
                AC_MSG_ERROR([Cannot compile java source. $JAVAC does not work properly])
        fi
        if test x$ac_cv_prog_javac_works = x; then
                AC_PROG_JAVAC
        fi
fi
AC_CACHE_CHECK(if $JAVA works, ac_cv_prog_java_works, [
JAVA_TEST=Test.java
CLASS_TEST=Test.class
TEST=Test
changequote(, )dnl
cat << \EOF > $JAVA_TEST
/* [#]line __oline__ "configure" */
public class Test {
public static void main (String args[]) {
        System.exit (0);
} }
EOF
changequote([, ])dnl
if test x$ac_cv_prog_uudecode_base64 != xyes; then
        if AC_TRY_COMMAND($JAVAC $JAVACFLAGS $JAVA_TEST) && test -s $CLASS_TEST; then
                :
        else
          echo "configure: failed program was:" >&AC_FD_CC
          cat $JAVA_TEST >&AC_FD_CC
          AC_MSG_ERROR(The Java compiler $JAVAC failed (see config.log, check the CLASSPATH?))
        fi
fi
if AC_TRY_COMMAND($JAVA $JAVAFLAGS $TEST) >/dev/null 2>&1; then
  ac_cv_prog_java_works=yes
else
  echo "configure: failed program was:" >&AC_FD_CC
  cat $JAVA_TEST >&AC_FD_CC
  AC_MSG_ERROR(The Java VM $JAVA failed (see config.log, check the CLASSPATH?))
fi
rm -fr $JAVA_TEST $CLASS_TEST Test.uue
])
AC_PROVIDE([$0])dnl
]
)

dnl AC_PROG_JAVAC_WORKS
AC_DEFUN([AC_PROG_JAVAC_WORKS],[
AC_CACHE_CHECK([if $JAVAC works], ac_cv_prog_javac_works, [
JAVA_TEST=Test.java
CLASS_TEST=Test.class
cat << \EOF > $JAVA_TEST
/* [#]line __oline__ "configure" */
public class Test {
}
EOF
if AC_TRY_COMMAND($JAVAC $JAVACFLAGS $JAVA_TEST) >/dev/null 2>&1; then
  ac_cv_prog_javac_works=yes
else
  AC_MSG_ERROR([The Java compiler $JAVAC failed (see config.log, check the CLASSPATH?)])
  echo "configure: failed program was:" >&AC_FD_CC
  cat $JAVA_TEST >&AC_FD_CC
fi
rm -f $JAVA_TEST $CLASS_TEST
])
AC_PROVIDE([$0])dnl
])

dnl AC_PROG_SSH
AC_DEFUN([AC_PROG_SSH],[
AC_REQUIRE([AC_EXEEXT])dnl
AC_PATH_PROG(SSH, ssh$EXEEXT, nocommand)
if test "$SSH" = nocommand; then
        AC_MSG_ERROR([ssh not found in $PATH])
fi;dnl
])

dnl AC_JNI_INCLUDE_DIR
AC_DEFUN([AC_JNI_INCLUDE_DIR],[

JNI_INCLUDE_DIRS=""

test "x$JAVAC" = x && AC_MSG_ERROR(['$JAVAC' undefined])
AC_PATH_PROG(_ACJNI_JAVAC, $JAVAC, no)
test "x$_ACJNI_JAVAC" = xno && AC_MSG_ERROR([$JAVAC could not be found in path])

_ACJNI_FOLLOW_SYMLINKS("$_ACJNI_JAVAC")
_JTOPDIR=`echo "$_ACJNI_FOLLOWED" | sed -e 's://*:/:g' -e 's:/[[^/]]*$::'`
case "$host_os" in
        darwin*)        _JTOPDIR=`echo "$_JTOPDIR" | sed -e 's:/[[^/]]*$::'`
                        _JINC="$_JTOPDIR/Headers";;
        *)              _JINC="$_JTOPDIR/include";;
esac
if test -f "$_JINC/jni.h"; then
        JNI_INCLUDE_DIRS="$JNI_INCLUDE_DIRS $_JINC"
else
        _JTOPDIR=`echo "$_JTOPDIR" | sed -e 's:/[[^/]]*$::'`
        if test -f "$_JTOPDIR/include/jni.h"; then
                JNI_INCLUDE_DIRS="$JNI_INCLUDE_DIRS $_JTOPDIR/include"
        else
                AC_MSG_ERROR([cannot find java include files])
        fi
fi

# get the likely subdirectories for system specific java includes
case "$host_os" in
bsdi*)          _JNI_INC_SUBDIRS="bsdos";;
linux*)         _JNI_INC_SUBDIRS="linux genunix";;
osf*)           _JNI_INC_SUBDIRS="alpha";;
solaris*)       _JNI_INC_SUBDIRS="solaris";;
*)              _JNI_INC_SUBDIRS="genunix";;
esac

# add any subdirectories that are present
for JINCSUBDIR in $_JNI_INC_SUBDIRS
do
        if test -d "$_JTOPDIR/include/$JINCSUBDIR"; then
                JNI_INCLUDE_DIRS="$JNI_INCLUDE_DIRS $_JTOPDIR/include/$JINCSUBDIR"
        fi
done
])

# _ACJNI_FOLLOW_SYMLINKS <path>
# Follows symbolic links on <path>,
# finally setting variable _ACJNI_FOLLOWED
# --------------------
AC_DEFUN([_ACJNI_FOLLOW_SYMLINKS],[
# find the include directory relative to the javac executable
_cur="$1"
while ls -ld "$_cur" 2>/dev/null | grep " -> " >/dev/null; do
        AC_MSG_CHECKING(symlink for $_cur)
        _slink=`ls -ld "$_cur" | sed 's/.* -> //'`
        case "$_slink" in
        /*) _cur="$_slink";;
        # 'X' avoids triggering unwanted echo options.
        *) _cur=`echo "X$_cur" | sed -e 's/^X//' -e 's:[[^/]]*$::'`"$_slink";;
        esac
        AC_MSG_RESULT($_cur)
done
_ACJNI_FOLLOWED="$_cur"
])# _ACJNI

dnl AC_PROG_SCP
AC_DEFUN([AC_PROG_SCP],[
AC_REQUIRE([AC_EXEEXT])dnl
AC_PATH_PROG(SCP, scp$EXEEXT, nocommand)
if test "$SCP" = nocommand; then
        AC_MSG_ERROR([scp not found in $PATH])
fi;dnl
])
