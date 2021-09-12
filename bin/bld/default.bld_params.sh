#!/bin/bash

compiler="cl"
compile_mode="debug"
arch="x64"
linker="lld-link"
ctx_opts=""

cd $bld_path/../..
root_path=$PWD

build_path="$root_path/build"
src_path="$root_path/src"
compiler_flags_path="$root_path/bin/compiler_flags.txt"
linker_flags_path="$root_path/bin/linker_flags.txt"
file_extensions_path="$root_path/bin/file_extensions.txt"
