#/bin/bash

# This script is used to update autotools files
# It should be run every time one modifies *.am *.in *.ac files

aclocal="aclocal"
# autoconf="autoconf-2.5x"
autoconf="autoconf"
# autoheader="autoheader-2.5x"
autoheader="autoheader"
if [ $(uname) = "Darwin" ]; then
    libtoolize="glibtoolize --force --copy"
else
    libtoolize="libtoolize --force --copy"
fi
automake="automake --add-missing --copy"

echo -n "Running $aclocal ..."
$aclocal 
if [ $? = 0 ]; then
    echo " OK" 
else
    echo "FAILED"
    exit 1
fi

echo -n "Running $autoconf ..."
$autoconf 
if [ $? = 0 ]; then
    echo " OK" 
else
    echo "FAILED"
    exit 1
fi

echo -n "Running $autoheader ..."
$autoheader 
if [ $? = 0 ]; then
    echo " OK" 
else
    echo "FAILED"
    exit 1
fi

echo -n "Running $libtoolize ..."
$libtoolize
if [ $? = 0 ]; then
    echo " OK" 
else
    echo "FAILED"
    exit 1
fi

echo -n "Running $automake ..."
$automake
if [ $? = 0 ]; then
    echo " OK" 
else
    echo "FAILED"
    exit 1
fi

# We do not use GPL, so we have to remove the COPYING file
#if [ -f COPYING ];then
#    \rm -f COPYING
#fi

echo "Autotools files updated sucessfully"

