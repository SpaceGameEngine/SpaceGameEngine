#!/bin/bash

input="dependencies.txt"

while IFS= read -r line; do
  IFS=' ' read -ra args <<<"$line"
  clone_path="ThirdParty/${args[0]}/source"

  if [ -d "${clone_path}" ]; then
    echo "Directory ${clone_path} exists. Omit it."
  else
    git clone -b "${args[1]}" --depth=1 "${args[2]}" "${clone_path}"
  fi

done <"$input"
