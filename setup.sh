#!/bin/bash

input="dependencies.txt"

while IFS= read -r line
do
  IFS=' ' read -ra args <<< "$line"
  git clone -b "${args[1]}" --depth=1 "${args[2]}" "ThirdParty/${args[0]}/source"
done < "$input"
