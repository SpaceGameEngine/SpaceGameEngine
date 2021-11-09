#!/bin/bash

cd Test
tests=($(ls))
cd ..

for test in "${tests[@]}"; do
  ./build/Test/$test/$test --gtest_output=xml:$test-REPORT.xml
done
