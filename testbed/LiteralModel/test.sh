#!/bin/bash

BASENAME=MM1k_Literal
DIFF="diff --brief"
METHOD_NUMBER=5

if [ $# = 3 ]; then
    if [ ! -d $2 ]; then
        mkdir -p $2
    fi

    cd $2
    make -f $3 clean
    CONT=1 
    cp $1/$BASENAME.obj   .
    cp $1/$BASENAME.maxvalues .
    echo -n " -->Generating a Literal Model"
    make -f $3 chain BASE=$BASENAME     
    $DIFF $1/$BASENAME.generator_mtx.param $BASENAME.generator_mtx.param
         if [ $? = 0 ];then
             echo "--> Comparing $BASENAME.generator_mtx.param: OK"
         else
             echo "--> Comparing $BASENAME.generator_mtx.param: FAILED"
             exit 1
         fi
    $DIFF $1/$BASENAME.generator_mtx.expr $BASENAME.generator_mtx.expr
         if [ $? = 0 ];then
             echo "--> Comparing $BASENAME.generator_mtx.expr: OK"
         else
             echo "--> Comparing $BASENAME.generator_mtx.expr: FAILED"
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
