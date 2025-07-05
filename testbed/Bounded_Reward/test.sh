#!/bin/bash

BASENAME=bounded_reward
DIFF="diff --brief"
REWARD=birth_death.buffer
DEFAULT_PRECISION=1e-06

if [ $# = 3 ]; then

    if [ ! -d $2 ]; then
        mkdir -p $2
    fi

    cd $2
    make -f $3 clean
    cp $1/$BASENAME.maxvalues .
    cp $1/$BASENAME.obj   .
    cp $1/$BASENAME.init_prob .
    cp $1/$BASENAME.intervals .  
    cp $1/$BASENAME.reward_levels.birth_death.buffer .    
    echo -n "-->Solving Bounded Cumulative Rate Reward method"
    make -f $3 reward-solv BASE=$BASENAME METHOD=8 REWARD_NAME=$REWARD PRECISION=$DEFAULT_PRECISION    
    $DIFF $1/$BASENAME.TS.brew.cumulat_distrib.1.00000e+00 $BASENAME.TS.brew.cumulat_distrib.1.00000e+00
         if [ $? = 0 ];then
             echo "--> DIFF OK"
         else
             echo "--> DIFF FAILED"
             exit 1
         fi
    echo -n "-->Solving Fraction of Time above a level method"
    make -f $3 reward-solv BASE=$BASENAME METHOD=9 REWARD_NAME=$REWARD PRECISION=$DEFAULT_PRECISION    
    $DIFF $1/$BASENAME.TS.brew.expected_period.1.00000e+00 $BASENAME.TS.brew.expected_period.1.00000e+00
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
