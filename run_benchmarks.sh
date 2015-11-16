#!/bin/bash

if [ "$#" -lt 3 ]; then
	echo 'Intended usage'
    echo '    run_benchmarks BOOST_ROOT TIMESTAMP SHA'
    exit
fi

mkdir -p results
mkdir -p build

for f in benchmarks/*.cpp; do
    rm -f build/benchmark
    echo "Benchmarking: $f";
    g++ -O2 -std="c++11" -I$1 $f -o build/benchmark
    for i in `seq 1 20`;
        do
            ./build/benchmark >> results/$3
        done
	rm -f build/benchmark
done
