#!/bin/bash

BASENAME=server_det
DIFF="diff --brief"
DEFAULT_PRECISION=1e-06
GTH=g

if [ $# = 3 ]; then
    if [ ! -d $2 ]; then
        mkdir -p $2
    fi

    cd $2
    make -f $3 clean
    cp $1/$BASENAME.obj   .
    cp $1/$BASENAME.maxvalues .
    cp $1/$BASENAME.NM.chns_betw_embed_pnts .
    cp $1/$BASENAME.NM.embedded_chain_mapping .
    cp $1/$BASENAME.NM.embedded_points .
    cp $1/$BASENAME.NM.embedded_points.expr .
    cp $1/$BASENAME.NM.interest_measures .
    echo -n "-->Solving with Non-Markovian method - GTH solution"
    make -f $3 nonmarkovian-gth-solv BASE=$BASENAME METHOD=11 PRECISION=$DEFAULT_PRECISION SOLUTION=$GTH    
    $DIFF $1/$BASENAME.SS.NM.expected_cycle_length $BASENAME.SS.NM.expected_cycle_length
         if [ $? = 0 ];then
             echo "--> DIFF:$BASENAME.SS.NM.expected_cycle_length OK"
         else
             echo "--> DIFF:$BASENAME.SS.NM.expected_cycle_length FAILED"
             exit 1
         fi
    $DIFF $1/$BASENAME.SS.NM.marginal_probs $BASENAME.SS.NM.marginal_probs
         if [ $? = 0 ];then
             echo "--> DIFF:$BASENAME.SS.NM.marginal_probs OK"
         else
             echo "--> DIFF:$BASENAME.SS.NM.marginal_probs FAILED"
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
