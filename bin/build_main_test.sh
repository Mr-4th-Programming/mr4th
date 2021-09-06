#!/bin/bash

bin_path="$(dirname "$0")"
source "$bin_path/bld_core.sh"

bld_unit $src_path/main.cpp test
