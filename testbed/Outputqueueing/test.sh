#!/bin/bash

BASENAME=outputqueueing
DIFF="diff --brief"
METHOD_NUMBER=5

if [ $# = 3 ]; then

    if [ ! -d $2 ]; then
        mkdir -p $2
    fi

    cd $2
    make -f $3 clean
    CONT=1 
    cp $1/$BASENAME.maxvalues .
    cp $1/$BASENAME.obj   .
    echo -n " -->Solving with GTH method"
    make -f $3 gth-solv BASE=$BASENAME METHOD=$METHOD_NUMBER    
    $DIFF $1/$BASENAME.SS.gth $BASENAME.SS.gth
         if [ $? = 0 ];then
             echo "--> OK"
         else
             echo "--> FAILED"
             exit 1
         fi
         
    exit 0
else
    echo
    echo " Usage:"
    echo
    echo "    $0 <tangram_old_dir> <tangram_new_dir> <Makefile>"
    echo
    exit 1
fi
