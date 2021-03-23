@echo off

set opts=-DENABLE_ASSERT=1
set cl_opts=-FC -GR- -EHa- -nologo -Zi -Fetest %opts% Winmm.lib
set clang_opts=-o test.exe %opts%
set code=%cd%
pushd build
cl %cl_opts% %code%\main.cpp
rem clang %clang_opts% %code%\main.cpp
popd
