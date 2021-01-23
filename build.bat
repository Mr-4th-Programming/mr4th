@echo off

set opts=-FC -GR- -EHa- -nologo -Zi -Fetest
set clang_opts=-o test.exe
set code=%cd%
pushd build
clang %clang_opts% %code%\main.cpp
popd
