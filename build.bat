@echo off

set opts=-DENABLE_ASSERT=1
set libs=Winmm.lib Userenv.lib Advapi32.lib
set cl_opts=-FC -GR- -EHa- -nologo -Zi %opts%
set clang_opts=%opts%
set exe_name=test.exe
set code=%cd%
pushd build
cl -Fe%exe_name% %cl_opts% %libs% %code%\main.cpp
rem clang -o %exe_name% %clang_opts% %libs% %code%\main.cpp

rem set dll_name=test_dll.dll
rem cl -Fe%dll_name% %cl_opts% %code%\dll_main.cpp /LD

popd
