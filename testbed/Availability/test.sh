#!/bin/bash

BASENAME=availability
DIFF="diff --brief"


if [ $# = 3 ]; then
    
    if [ ! -d $2 ]; then
        mkdir -p $2
    fi

    
    cd $2
    make -f $3 clean
    CONT=1 
    cp $1/$BASENAME.obj   .
    cp $1/$BASENAME.maxvalues .
    cp $1/$BASENAME.partition .
    echo -n " -->Solving with GTH method"
    make -f $3 gthblock-solv BASE=$BASENAME     
    $DIFF $1/$BASENAME.SS.gthb $BASENAME.SS.gthb
         if [ $? = 0 ];then
             echo "--> DIFF OK"
         else
             echo "--> DIFF FAILED"
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
