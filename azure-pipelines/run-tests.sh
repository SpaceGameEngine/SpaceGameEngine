#!/bin/bash

cd Test
tests=($(ls))
cd ..

for test in "${tests[@]}"; do
  if [ $test != "TestModule" ];then
    ./build/Test/$test/$test --gtest_output=xml:$test-REPORT.xml
  fi
done
