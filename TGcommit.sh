#!/bin/bash

################################################################################
#  File: TGcommit.sh
#  Author: {allyson}@land.ufrj.br
#  Date: 2008-09-20
#
#  Description: performs some tests before commiting changes to TangramII CVS
#
#  Copyright 2008 LAND/UFRJ
#  http://www.land.ufrj.br
#
################################################################################

VERSION_FILE="configure.ac"
T2MAIN_WINDOW_FILE="interface/Tangram2_Interface/T2main_app.java"
CHANGELOG="ChangeLog"

if [ ! -f $VERSION_FILE ]; then
    echo "[TGCOMMIT] You don't seem to be at the right place"
    echo "[TGCOMMIT] File '$VERSION_FILE' not found"
    exit 1
fi

# Test if any file needs to be updated
# FIXME: Verify this command line, may be buggy in some cases
if [ "$1" != "--nocheck" ]; then
    x=`cvs -n update 2>/dev/null | grep -v "?" | grep -v "^M " | grep -v "^A " | grep -v "^R "`
    if [ X"$x" != X ]; then
	    echo "[TGCOMMIT] You may need to update some files first:"
	    echo "####"
	    echo $x
	    echo "####"
	    exit 1
    fi
fi

# Ask user which version number should be increased
# FIXME: Are the options description correct?
VERSION=`grep "^AC_INIT" $VERSION_FILE | cut -f 2 -d ","`
echo "[TGCOMMIT] Current Tangram-II version: $VERSION"
echo "Which version number should be increased?"
echo "[1] the new version will be incompatible with the previous"
echo "[2] new features were added"
echo "[3] small feature added, bug fixed"
read IDX
while [ $IDX != 1 ] && [ $IDX != 2 ] && [ $IDX != 3 ]; do
	echo "Current FM version: $VERSION"
	echo "Which version number should be increased?"
	echo "[1] the new version will be incompatible with the previous"
	echo "[2] new features were added"
	echo "[3] small feature added, bug fixed"
	read IDX
done

# Show files that are about to be committed
FILES=`cvs diff 2>/tmp/cvsdiff-$USER | grep "^RCS file" | sed -e 's/RCS file: \/home\/develop\/CVS\/tangram2\///' -e 's/,v//'`
echo "[TGCOMMIT] The following files will be commited:"
for i in $FILES; do
    echo "    $i"
done

echo "-----------------"
NEWFILES=`grep 'new entry' /tmp/cvsdiff-$USER | awk '{print $3}'`;
\rm -f /tmp/cvsdiff-$USER
echo "[TGCOMMIT] The following new files will be added to the repository:"
for j in $NEWFILES; do
    echo "    $j"
done
echo "-----------------"
echo
echo "[TGCOMMIT] Continue?[y|n]"
read ANSWER
while [ "x$ANSWER" != "xy" ] && [ "x$ANSWER" != "xn" ]; do
    echo "[TGCOMMIT] The files listed above will be committed. Continue?[y|n]"
    read ANSWER
done

if [ "x$ANSWER" = "xn" ]; then
    echo "[TGCOMMIT] Commit aborted."
    exit 0
fi

# Increment version properly
NEW_VERSION=`echo $VERSION | awk -v INDEX=$IDX '{split($1,v,"."); v[INDEX]++; if ( INDEX == 1 ){ v[2]=0; v[3]=0;}else{ if ( INDEX == 2 ) v[3]=0;}printf("%s.%s.%s", v[1],v[2],v[3])}'`

# Need to replace "." in version to "\." or sed will be confused
VERSION=`echo $VERSION | sed -e "s/\./\\\./g"`

# Replace version number in file VERSION
cvs edit $VERSION_FILE
echo "[TGCOMMIT] Updating file $VERSION_FILE"
sed -i -e "s/$VERSION/$NEW_VERSION/" $VERSION_FILE
if [ "$?" != 0 ]; then
    echo "[TGCOMMIT] Update of file $VERSION_FILE failed"
    exit 1
fi

# Replace version number in Tangram-II main window
cvs edit $T2MAIN_WINDOW_FILE
echo "[TGCOMMIT] Updating file $T2MAIN_WINDOW_FILE"
sed -i -e "s/private String version = \"[0-9]\+\.[0-9]\+\.[0-9]\+\"/private String version = \"$NEW_VERSION\"/" $T2MAIN_WINDOW_FILE
if [ "$?" != 0 ]; then
    echo "[TGCOMMIT] Update of file $T2MAIN_WINDOW_FILE failed"
    cvs unedit $VERSION_FILE
    exit 1
fi

TAG="ver_`echo $NEW_VERSION | sed -e "s/\./_/g"`"
tmpfile=/tmp/fm-$USER-`date +%Y%m%d`
echo "CVS: Type the log message" > $tmpfile
echo "CVS: Lines beginning with 'CVS:' will be ignored" >> $tmpfile
echo >> $tmpfile
echo "####################" >> $tmpfile
echo "version $NEW_VERSION" >> $tmpfile
echo "cvs tag $TAG" >> $tmpfile
echo >> $tmpfile
echo "Modified files:" >> $tmpfile
echo "    $FILES" >> $tmpfile
echo "-----------------" >> $tmpfile
echo "Added files:" >> $tmpfile
echo "    $NEWFILES" >> $tmpfile
echo "-----------------" >> $tmpfile
echo >> $tmpfile
EDITOR=vi

$EDITOR $tmpfile

echo "[TGCOMMIT] Updating ChangeLog"
cvs edit $CHANGELOG
mv $CHANGELOG $CHANGELOG.old
cat $tmpfile $CHANGELOG.old | grep -v '^CVS:'>> $CHANGELOG

# Try to commit if it goes ok then tag with the new version
echo "[TGCOMMIT] Committing"
cvs commit -m "`cat $tmpfile | grep -v '^CVS:'`"
if [ "$?" != 0 ]; then
    echo "[TGCOMMIT] commit failed!"
    echo " You MUST answer 'y' to revert changes on $VERSION_FILE and $CHANGELOG"
    echo " or the next time this script will fail"
    cvs unedit $VERSION_FILE $CHANGELOG $EXTRA_FILES
    \rm -f $tmpfile $CHANGELOG.old
    exit 1
fi

# Deleting temp files
\rm -f $tmpfile $CHANGELOG.old

cvs tag $TAG
if [ "$?" != 0 ]; then
    echo "[TGCOMMIT] tag failed!"
    exit 1
fi
echo "[TGCOMMIT] New CVS tag added: $TAG"

echo "[TGCOMMIT] Updating local copy "
./autotools.sh
./configure
make clean

echo "[TGCOMMIT] Do you want to compile tangram2 now?[y|n]"
read ANSWER
while [ x"$ANSWER" != xy ] && [ x"$ANSWER" != xn ]; do
    echo "[TGCOMMIT] Do you want to compile tangram2 now?[y|n]"
    read ANSWER
done
if [ x"$ANSWER" = xy ]; then
    make
fi

echo "[TGCOMMIT] Files committed successfully, new version is $NEW_VERSION"

exit 0
