#!/bin/bash

if [ "$#" -lt 3 ]; then
	echo 'Intended usage'
    echo '    run BOOST_ROOT TIMESTAMP SHA'
    exit
fi

mkdir -p build
mkdir -p results

cd results
for f in ../benchmarks/*.cpp; do
    rm -f ../build/benchmark
#    echo "Compiling $f";
    g++ -O2 -std="c++11" -I$1 $f -o ../build/benchmark
    for i in `seq 1 10`;
        do
#            echo "Run $i"
            ../build/benchmark >> $3
        done
	rm -f ../build/benchmark
done

#echo 'Compiling report generator';
g++ -std="c++11" ../report/report.cpp -o ../build/report
#echo 'Generating report';
../build/report $2 $3

rm -f $3
rm -Rf ../build

