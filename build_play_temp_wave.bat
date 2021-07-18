@echo off

set opts=-DENABLE_ASSERT=1
set libs=Winmm.lib Userenv.lib Advapi32.lib
set cl_opts=-FC -GR- -EHa- -nologo -Zi %opts%
set exe_name=play_temp_wave.exe
set code=%cd%
pushd build
cl -Fe%exe_name% %cl_opts% %libs% %code%\play_temp_wave.cpp

popd
