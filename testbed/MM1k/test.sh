#!/bin/bash

BASENAME=MM1k
DIFF="diff --brief"
METHODS="gauss jacobi sor power"
EXPECTED_RATE_REWARD=12
REWARD=Server_Queue.q_size
DEFAULT_PRECISION=1e-06

if [ $# = 3 ]; then
    if [ ! -d $2 ]; then
        mkdir -p $2
    fi

    cd $2
    make -f $3 clean
    CONT=1 
    cp $1/$BASENAME.maxvalues .
    cp $1/$BASENAME.obj .
    for i in $METHODS;do
        echo "--> Solving with iterative method number $i ..."
        make -f $3 iterative-solv BASE=$BASENAME METHOD=$CONT
        if [ $? != 0 ];then
            echo "FAILED"
            exit 1
        else
            $DIFF $1/$BASENAME.SS.$i $BASENAME.SS.$i
            if [ $? = 0 ];then
                echo "--> DIFF OK"
            else
                echo "--> DIFF FAILED"
                exit 1
            fi
        fi
        ((CONT++))
    done
    cp  $1/$BASENAME.init_prob .
    cp $1/$BASENAME.intervals .  
    echo -n "-->Solving with Expected Cumulative Rate Reward method"
    make -f $3 expected-reward-solv BASE=$BASENAME METHOD=$EXPECTED_RATE_REWARD REWARD_NAME=$REWARD PRECISION=$DEFAULT_PRECISION    
    $DIFF $1/$BASENAME.TS.exptr $BASENAME.TS.exptr
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
