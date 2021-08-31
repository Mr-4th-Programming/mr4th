#!/bin/bash

###### Get Paths ##############################################################
og_path=$PWD
cd "$(dirname "$0")"
cd ..

 root_path=$PWD
  bin_path="$root_path/bin"
local_path="$root_path/local"
build_path="$root_path/build"
  src_path="$root_path/src"

###### Get Local Parameters ###################################################

source "$local_path/bld_params.sh"

echo "[$compiler] [$compile_mode] [$linker] [$arch]"

###### Restore Path ###########################################################
cd $og_path
