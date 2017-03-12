#!/bin/bash

if [ "$#" -lt 2 ]; then
	echo 'Intended usage'
    echo '    run_benchmarks BOOST_ROOT SHA'
    exit
fi

mkdir -p temp
mkdir -p temp/m32
mkdir -p temp/m64
mkdir -p build

for f in benchmarks/*.cpp; do
	echo "Benchmarking: $f";
    
    rm -f build/benchmark	
    g++ -m32 -O2 -std="c++11" -I$1 $f -o build/benchmark

    for i in `seq 1 10`;
        do
            ./build/benchmark >> temp/m32/$2
            printf '.'
        done
    printf '\n'
    
    rm -f build/benchmark
    g++ -m64 -O2 -std="c++11" -I$1 $f -o build/benchmark

    for i in `seq 1 10`;
        do
			./build/benchmark >> temp/m64/$2
            printf '.'
        done
    printf '\n'
    
    rm -f build/benchmark
done
