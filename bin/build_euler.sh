#!/bin/bash

bld_path="$(dirname $(realpath "$0"))"/bld
source "$bld_path/bld_core.sh"

bld_print_implicit_opts
bld_compile $src_path/euler_cpp.cpp

cd "$build_path"
ml64 -c -nologo -Zi $src_path/euler_asm.asm

bld_link euler euler_cpp.obj euler_asm.obj
