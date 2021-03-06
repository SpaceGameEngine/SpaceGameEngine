#!/bin/bash

cd Test
tests=($(ls))

for test in "${tests[@]}"; do
  cd $test
  ../../build/Test/$test/$test --gtest_output=xml:$test-REPORT.xml
  cd ..
done
