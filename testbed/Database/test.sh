#!/bin/bash

BASENAME=database
DIFF="diff --brief"
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
    echo -n "-->Solving with Point Probabilities method"
    make -f $3 pointprob-solv BASE=$BASENAME PRECISION=$DEFAULT_PRECISION    
    $DIFF $1/$BASENAME.TS.pp.1.00000e+01 $BASENAME.TS.pp.1.00000e+01
         if [ $? = 0 ];then
             echo "--> DIFF OK"
         else
             echo "--> DIFF FAILED"
             exit 1
         fi
    echo -n "--> Testing the Matrix Visualization - State Ordering Program"
    cd mtx_vis    
    make -f $3 clean
    cp $1/$BASENAME.maxvalues .    
    make -f $3 mtx_vist BASE=$BASENAME      
    $DIFF $1/mtx_vis/$BASENAME.vstat $1/mtx_vis/$BASENAME.vstat  
         if [ $? = 0 ];then
             echo "--> DIFF $BASENAME.vstat OK"
         else
             echo "--> DIFF $BASENAME.vstat FAILED"
             exit 1
         fi
    
    $DIFF $1/mtx_vis/$BASENAME.generator_mtx $1/mtx_vis/$BASENAME.generator_mtx  
         if [ $? = 0 ];then
             echo "--> DIFF $BASENAME.generator_mtx OK"
         else
             echo "--> DIFF $BASENAME.generator_mtx FAILED"
             exit 1
         fi

    $DIFF $1/mtx_vis/$BASENAME.state_variable $1/mtx_vis/$BASENAME.state_variable  
         if [ $? = 0 ];then
             echo "--> DIFF $BASENAME.state_variable OK"
         else
             echo "--> DIFF $BASENAME.state_variable FAILED"
             exit 1
         fi

    $DIFF $1/mtx_vis/$BASENAME.rate_reward.Database.Data_avail $1/mtx_vis/$BASENAME.rate_reward.Database.Data_avail  
         if [ $? = 0 ];then
             echo "--> DIFF $BASENAME.rate_reward.Database.Data_avail OK"
         else
             echo "--> DIFF $BASENAME.rate_reward.Database.Data_avail FAILED"
             exit 1
         fi

    $DIFF $1/mtx_vis/$BASENAME.states $1/mtx_vis/$BASENAME.states  
         if [ $? = 0 ];then
             echo "--> DIFF $BASENAME.states OK"
         else
             echo "--> DIFF $BASENAME.states FAILED"
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
