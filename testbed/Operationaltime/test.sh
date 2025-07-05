#!/bin/bash

BASENAME=operationaltime
DIFF="diff --brief"
REWARD=Dispatcher_CPUS.reliabiliy
DEFAULT_PRECISION=1e-06

if [ $# = 3 ]; then
    if [ ! -d $2 ]; then
        mkdir -p $2
    fi

    cd $2
    make -f $3 clean
    cp $1/$BASENAME.obj   .
    cp $1/$BASENAME.maxvalues .
    cp $1/$BASENAME.init_prob .
    cp $1/$BASENAME.intervals . 
    cp $1/$BASENAME.reward_levels.Dispatcher_CPUS.reliabiliy .
    echo -n "-->Solving with Operational time and related measures method"
    make -f $3 reward-solv BASE=$BASENAME METHOD=10 REWARD_NAME=$REWARD PRECISION=$DEFAULT_PRECISION    
    $DIFF $1/$BASENAME.TS.operational_time $BASENAME.TS.operational_time
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
