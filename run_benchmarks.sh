#!/bin/bash

if [ "$#" -lt 2 ]; then
	echo 'Intended usage'
    echo '    run_benchmarks BOOST_ROOT SHA'
    exit
fi

mkdir -p temp
mkdir -p build

for f in benchmarks/*.cpp; do
    rm -f build/benchmark
    echo "Benchmarking: $f";
    g++ -O2 -std="c++11" -I$1 $f -o build/benchmark
    for i in `seq 1 20`;
        do
            ./build/benchmark >> temp/$2
        done
	rm -f build/benchmark
done
