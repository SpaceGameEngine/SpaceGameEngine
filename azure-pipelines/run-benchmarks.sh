#!/bin/bash

cd Benchmark
benchmarks=($(ls))

for benchmark in "${benchmarks[@]}"; do
  cd $benchmark
  ../../build/Benchmark/$benchmark/$benchmark
  cd ..
done
