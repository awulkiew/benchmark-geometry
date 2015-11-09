#!/bin/bash

mkdir -p build

cd results
for f in ../benchmarks/*.cpp; do
    g++ -O2 -std="c++11" -I$BOOST_ROOT $f -o ../build/benchmark
    for i in `seq 1 10`;
        do
            ../build/benchmark >> $BENCHMARK_SHA
        done
done

g++ -std="c++11" ../report/report.cpp -o ../build/report
# ./report $BENCHMARK_TIMESTAMP $BENCHMARK_SHA

#cd ..
#rm -R build