#!/bin/bash

###### Parse Arguments ########################################################
command=$1
args=()
for ((i=2; i<=$#; i+=1)); do
  args+=(${!i})
done

###### Source the Core ########################################################
bld_path="$(dirname $(realpath "$0"))"
source "$bld_path/bld_core.sh"

###### Dispatch ###############################################################
if [ "$command" == "cmp" ]; then
  bld_compile "${args[@]}"
elif [ "$command" == "lnk" ]; then
  bld_link "${args[@]}"
elif [ "$command" == "lib" ]; then
  bld_lib "${args[@]}"
elif [ "$command" == "unit" ]; then
  bld_unit "${args[@]}"
else
  echo "unknown command '$command'"
fi

