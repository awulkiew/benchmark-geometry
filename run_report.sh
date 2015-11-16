#!/bin/bash

if [ "$#" -lt 2 ]; then
	echo 'Intended usage'
    echo '    run_report TIMESTAMP SHA'
    exit
fi

echo 'Generating report';
g++ -std="c++11" report/report.cpp -o build/report
cd results && ../build/report $1 $2
