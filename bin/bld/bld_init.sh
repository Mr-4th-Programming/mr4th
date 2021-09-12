#!/bin/bash

bld_path=$(dirname "$0")
local_path="$bld_path/local"
mkdir -p "$local_path"
cp $bld_path/default.bld_params.sh $local_path/bld_params.sh
