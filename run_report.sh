#!/bin/bash

if [ "$#" -lt 2 ]; then
    echo 'Intended usage'
    echo '    run_report TIMESTAMP SHA [RESULTS_DIR]'
    exit
fi

RESULTS_DIR=results
TEMP_RESULTS_DIR=../results

if [ "$#" -gt 2 ]; then
    if [[ "$3" = /* ]]; then
        RESULTS_DIR=$3
        TEMP_RESULTS_DIR=$3
    else
        RESULTS_DIR=$3
        TEMP_RESULTS_DIR=../$3
    fi
fi

mkdir -p $RESULTS_DIR
mkdir -p $RESULTS_DIR/m32
mkdir -p $RESULTS_DIR/m64

echo 'Generating report';
g++ -std="c++11" report/report.cpp -o build/report
cd temp && ../build/report $1 $2 $TEMP_RESULTS_DIR
