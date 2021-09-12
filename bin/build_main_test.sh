#!/bin/bash

bld_path="$(dirname $(realpath "$0"))"/bld
source "$bld_path/bld_core.sh"

bld_print_implicit_opts
bld_unit $src_path/main.cpp test
