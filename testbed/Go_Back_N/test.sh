#!/bin/bash

BASENAME=go_back_n
DIFF="diff --brief"
REWARD=Channel.packet_accepted
DEFAULT_PRECISION=1e-06
EXPECTED_RATE_REWARD=17

if [ $# = 3 ]; then
    if [ ! -d $2 ]; then
        mkdir -p $2
    fi

    cd $2
    make -f $3 clean
    cp $1/$BASENAME.obj   .
    cp $1/$BASENAME.maxvalues .
    cp  $1/$BASENAME.init_prob .
    cp $1/$BASENAME.intervals .  
    echo -n "-->Solving with Expected Cumulative Impulse Reward method"
    make -f $3 expected-reward-solv BASE=$BASENAME METHOD=$EXPECTED_RATE_REWARD REWARD_NAME=$REWARD   
    $DIFF $1/$BASENAME.TS.expected_imp_reward.Channel.packet_accepted $BASENAME.TS.expected_imp_reward.Channel.packet_accepted
         if [ $? = 0 ];then
             echo "--> DIFF: $BASENAME.TS.expected_imp_reward.Channel.packet_accepted OK"
         else
             echo "--> DIFF: $BASENAME.TS.expected_imp_reward.Channel.packet_accepted FAILED"
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
