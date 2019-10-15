#!/bin/bash

cd Benchmark
benchmarks=($(ls))

for benchmark in "${benchmarks[@]}"; do
  cd $benchmark
  ../../build/Benchmark/$benchmark/$benchmark --benchmark_format=json --benchmark_out=$benchmark-REPORT.json
  cd ..
done
