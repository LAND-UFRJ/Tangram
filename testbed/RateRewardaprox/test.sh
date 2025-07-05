#!/bin/bash

BASENAME=raterewardaprox
DIFF="diff --brief"
REWARD=Database.database_availability
MEASURE_OPT_1=2
MEASURE_OPT_2=1
ERLANGSTAGES=50
DEFAULT_PRECISION=1e-06

if [ $# = 3 ]; then

    if [ ! -d $2 ]; then
        mkdir -p $2
    fi

    cd $2
    make -f $3 clean
    cp $1/$BASENAME.obj   .
    cp $1/$BASENAME.maxvalues   .
    cp $1/$BASENAME.init_prob   .
    cp $1/$BASENAME.int_rewd    .
    cp $1/$BASENAME.int_direct  .
    cp $1/$BASENAME.partition   .  
    echo -n "-->Solving with Cumulative Rate Reward method - Direct"
    make -f $3 cumul-reward-direct-solv BASE=$BASENAME METHOD=13 ERLANG_STAGES=$ERLANGSTAGES REWARD_NAME=$REWARD MEASURE_OPT=$MEASURE_OPT_1    
    $DIFF $1/$BASENAME.TS.DIRECT.Cumulative_SET_Reward $BASENAME.TS.DIRECT.Cumulative_SET_Reward
         if [ $? = 0 ];then
             echo "--> DIFF: $BASENAME.TS.DIRECT.Cumulative_SET_Reward OK"
         else
             echo "--> DIFF: $BASENAME.TS.DIRECT.Cumulative_SET_Reward FAILED"
             exit 1
         fi
    cp $1/$BASENAME.int_iterative .
    cp $1/$BASENAME.int_rewi  .
    echo -n "-->Solving with Cumulative Rate Reward method - Iterative"
    make -f $3 cumul-reward-it-solv BASE=$BASENAME METHOD=14 REWARD_NAME=$REWARD MEASURE_OPT=$MEASURE_OPT_1 METHOD=3 PRECISION=$DEFAULT_PRECISION$   
    $DIFF $1/$BASENAME.TS.ITERATIVE.Cumulative_SET_Reward $BASENAME.TS.ITERATIVE.Cumulative_SET_Reward
         if [ $? = 0 ];then
             echo "--> DIFF: $BASENAME.TS.ITERATIVE.Cumulative_SET_Reward OK"
         else
             echo "--> DIFF: $BASENAME.TS.ITERATIVE.Cumulative_SET_Reward FAILED"
             exit 1
         fi
    echo -n "-->Solving with Efficient Transient Probability Distribution method - Direct"
    make -f $3 approx-prob-direct-solv BASE=$BASENAME METHOD=15 ERLANG_STAGES=$ERLANGSTAGES MEASURE_OPT=$MEASURE_OPT_2    
    $DIFF $1/$BASENAME.TS.espa_dir.STATE_PROB.1.000000e+00 $BASENAME.TS.espa_dir.STATE_PROB.1.000000e+00
         if [ $? = 0 ];then
             echo "--> DIFF: $BASENAME.TS.espa_dir.STATE_PROB.1.000000e+00  OK"
         else
             echo "--> DIFF: $BASENAME.TS.espa_dir.STATE_PROB.1.000000e+00  FAILED"
             exit 1
         fi
    echo -n "-->Solving with Efficient Transient Probability Distribution method - Iterative"
    make -f $3 approx-prob-it-solv BASE=$BASENAME METHOD=16 MEASURE_OPT=$MEASURE_OPT_2 METHOD=3 PRECISION=$DEFAULT_PRECISION$   
    $DIFF $1/$BASENAME.TS.espa_iterative.STATE_PROB.1.000000e+00 $BASENAME.TS.espa_iterative.STATE_PROB.1.000000e+00
         if [ $? = 0 ];then
             echo "--> DIFF: $BASENAME.TS.espa_iterative.STATE_PROB.1.000000e+00 OK"
         else
             echo "--> DIFF: $BASENAME.TS.espa_iterative.STATE_PROB.1.000000e+00 FAILED"
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
