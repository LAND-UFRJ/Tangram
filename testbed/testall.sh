#!/bin/bash

TESTDIR="Availability Database LiteralModel  Operationaltime \
         RateRewardaprox Bounded_Reward Deterministic_Server \
         Go_Back_N MM1k Outputqueueing"
         
REPORTFILE="$PWD/test_report_`date +%Y%m%d%H%M%S`.log"         

echo "############################################################"
echo "  TANGRAM2_HOME=$TANGRAM2_HOME"
echo "      JAVA_HOME=$JAVA_HOME"
echo "############################################################"

if [ $# = 1 ]; then
    CURDIR=$PWD
    for i in $TESTDIR;
    do 
        cd $i;
        ./test.sh $PWD/tangram2.0/ $PWD/$1_vs_2.0 $CURDIR/Makefile.Tangram2
        if [ $? -ne 0 ]; 
        then 
            echo "############################################################"
            echo "  TEST $i ($TANGRAM2_HOME AGAINST TANGRAM2.0) FAILED"
            echo "############################################################"
            echo "Test $i ($TANGRAM2_HOME against tangram2.0) failed" >> ${REPORTFILE}
        else
            echo "############################################################"
            echo "  TEST $i ($TANGRAM2_HOME AGAINST TANGRAM2.0) SUCESS"
            echo "############################################################"
            echo "Test $i ($TANGRAM2_HOME against tangram2.0) sucess" >> ${REPORTFILE}
        fi;       

        ./test.sh $PWD/tangram3.0/ $PWD/$1_vs_3.0 $CURDIR/Makefile.Tangram2
        if [ $? -ne 0 ]; 
        then 
            echo "############################################################"
            echo "  TEST $i ($TANGRAM2_HOME AGAINST TANGRAM3.0) FAILED"
            echo "############################################################"
            echo "Test $i ($TANGRAM2_HOME against tangram3.0) failed" >> ${REPORTFILE}
        else
            echo "############################################################"
            echo "  TEST $i ($TANGRAM2_HOME AGAINST TANGRAM3.0) SUCESS"
            echo "############################################################"
            echo "Test $i ($TANGRAM2_HOME against tangram3.0) sucess" >> ${REPORTFILE}
        fi;       

        cd -;
    done
else
    echo
    echo " Usage:"
    echo
    echo "    $0 <results_dirname>"
    echo
    echo " where: "
    echo "       <results_dirname> : directory where resulting files will be placed"
    echo
    exit 1
fi
