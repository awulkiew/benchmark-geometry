#!/bin/bash

if [ "$#" -lt 3 ]; then
	echo 'Intended usage'
    echo '    run BOOST_ROOT TIMESTAMP SHA'
    exit
fi

./run_benchmarks.sh $1 $2 $3
./run_report.sh $2 $3
./run_clean.sh $3