#!/bin/bash

###### Usage ##################################################################
# This script is meant to be invoked by the 'source' command. The variable
# bld_path should first be set to contain the path to the root folder of the
# bld system:
#   bld_path="$(dirname $(realpath "$0"))"/<path-from-here-to-bld>
#   source "$bld_path/bld_core.sh"
#
#
# The following bld functions form the core "bld commands":
#   bld_compile, bld_link, bld_lib, bld_unit
#
#  And there are several more helper functions:
#   bld_print_implicit_opts, bld_load_local_opts
#
#
# Signatures:
#   bld_compile <source-file> <zero-or-more-options>
#   bld_link <out-name> <one-or-more-source-files-objs-or-libs> -- <zero-or-more-options>
#   bld_lib <out-name> <one-or-more-source-files-or-objs> -- <zero-or-more-options>
#   bld_unit <source-file> <zero-or-more-options>
#
#   bld_print_implicit_opts (no arguments)
#   bld_load_local_opts (no arguments)
#
#
# Shared notes for all:
# + Options are gathered from the command line, from the local context, and
# in the case of the commands bld_compile and bld_unit, from the source file.
# + Options in source files are specified between the strings //$ and //.
#  Object files and library files should be specified with the extensions
# .obj and .lib respectively. The system automatically modifies the names to
# match the naming used by the selected context.
# + When the option "diagnostics" is included extra details will be printed
# from the bld commands. Showing the full list of options, invokation lines,
# and the build path.
#
# bld_compile:
# + Creates an object file from a single source file via the selected compiler.
#
# bld_link:
# + Creates executables and shared binaries from source files, object files,
# and static libraries. First uses the bld_compile command on the source files.
# Uses the selected linker.
# + The output is a shared binary (.dll or .so) when the option "dll" is
# included.
#
# bld_lib:
# + Creates a static library from source files and object files. First uses
# the bld_compile command on the source files. Uses the OS to determine the
# correct archiver.
#
# bld_unit:
# + Creates an executable (or shared binary) from a single source file.
# This command essentially does a single bld_compile then bld_link. With
# two differences:
#  1. The options from the source file are visible to the bld_link.
#  2. The name of the executable is determined either from the first option 
#     in the list or from the source file name if there are no options.
#
# bld_print_implicit_opts:
# + Shows the implicit options loaded from the local parameters script.
#
# bld_load_local_opts:
# + It is possible and sometimes useful to modify the local parameters after
# they are loaded to create certain kinds of build scripts. This function
# resets the local parameters to their original states by rerunning the
# local parameters script.


###### Flags From Opts ########################################################

function bld_flags_from_opts {
  ###### parse arguments ####################################################
  local in_file=$1
  local opts=()
  for ((i=2; i<=$#; i+=1)); do
    opts+=(${!i})
  done
  
  ###### load file ##########################################################
  local flags_raw=()
  IFS=$'\r\n' GLOBIGNORE='*' command eval 'flags_raw=($(cat $in_file))'
  
  ###### filter #############################################################
  local flags=()
  for ((i=0;i<${#flags_raw[@]};i+=1)); do
    local flag=${flags_raw[i]}
    
    ###### skip blanks and comments #######################################
    if [[ -z "${flag// }" ]]; then
      continue
    fi
    if [[ "${flag:0:1}" == "#" ]]; then
      continue
    fi
    
    ###### parse line filters #############################################
    local line_filters=()
    while [[ $flag = *">"* ]]; do
      line_filters+=("${flag%%>*}")
      flag="${flag#*>}"
    done
    
    ###### check filters ##################################################
    local can_include=1
    for ((j=0;j<${#line_filters[@]};j+=1)); do
      can_include=0
      for ((k=0;k<${#opts[@]};k+=1)); do
        if [[ ${opts[k]} = ${line_filters[j]} ]]; then
          can_include=1
          break
        fi
      done
      if [[ "$can_include" = "0" ]]; then
        break
      fi
    done
    if [[ "$can_include" = "1" ]]; then
      flags+=("${flag}")
    fi
  done
  
  echo "${flags[@]}"
}

###### Opts From Src ##########################################################

function bld_opts_from_src {
  ###### split file into tokens #############################################
  local in_file=$1
  local tokens=($(grep "//\\$" $in_file))
  
  ###### parse ##############################################################
  local in_params_range="0"
  local params=()
  for ((i=0; i<${#tokens[@]}; i+=1)); do
    local string="${tokens[i]}"
    if [[ "$in_params_range" == "0" ]]; then
      if [[ "$string" == "//$" ]]; then
        in_params_range="1"
      fi
    elif [[ "$in_params_range" == "1" ]]; then
      if [[ "${string:0:2}" == "//" ]]; then
        break
      fi
      params+=($string)
    fi
  done
  
  echo "${params[@]}"
}

###### Dedup ##################################################################

function bld_dedup {
  ###### parse arguments ####################################################
  local in=()
  for ((i=1; i<=$#; i+=1)); do
    in+=(${!i})
  done
  
  ###### dedup ##############################################################
  local out=()
  for ((i=0; i<${#in[@]}; i+=1)); do
    local string=${in[i]}
    local is_dup="0"
    for ((j=0; j<${#out[@]}; j+=1)); do
      if [[ "$string" == "${out[j]}" ]]; then
        is_dup="1"
        break
      fi
    done
    if [[ "$is_dup" == "0" ]]; then
      out+=($string)
    fi
  done
  
  echo "${out[@]}"
}

###### Has Opt ################################################################

function bld_has_opt {
  ###### parse arguments ####################################################
  local key_opt=$1
  local opts=()
  for ((i=2; i<=$#; i+=1)); do
    opts+=(${!i})
  done
  
  ###### scan ###############################################################
  local has_key=0
  for ((i=0;i<${#opts[@]};i+=1)); do
    local opt=${opts[i]}
    if [[ "$opt" == "$key_opt" ]]; then
      has_key=1
      break
    fi
  done
  
  echo $has_key
}

###### Load Local Options #####################################################

function bld_load_local_opts {
  ###### os cracking ########################################################
  os="undefined"
  if [ "$OSTYPE" == "win32" ] ||
     [ "$OSTYPE" == "msys"  ]; then
    os="windows"
  elif [ "$OSTYPE" == "linux-gnu" ]; then
    os="linux"
  elif [ "$OSTYPE" == "darwin" ]; then
    os="mac"
  fi
  ###### load parameters from the local script ################################
  if [ -f "$local_path/bld_params.sh" ]; then
    source "$local_path/bld_params.sh"
  fi
}

###### Implicit Opts ##########################################################

function bld_implicit_opts {
  echo $compiler $compile_mode $os $arch $linker $ctx_opts
}

###### Print Implicit Options #################################################

function bld_print_implicit_opts {
  local opts=($(bld_implicit_opts))
  local bracketed=()
  for ((i=0; i<=${#opts[@]}; i+=1)); do
    local opt="${opts[i]}"
    if [ "$opt" != "" ]; then
      bracketed+=("[${opts[i]}]")
    fi
  done
  echo "${bracketed[@]}"
}

###### Compile ################################################################

function bld_compile {
  ###### parse arguments ####################################################
  local in_file=$1
  local opts=()
  for ((i=2; i<=$#; i+=1)); do
      opts+=(${!i})
  done
  if [ "$in_file" == "" ]; then
    echo "compile: missing input file"
    return 1
  fi
  
  ###### finish in file #####################################################
  local final_in_file=$in_file
  
  ###### finish options #####################################################
  local src_opts=($(bld_opts_from_src $final_in_file))
  local impl_opts=($(bld_implicit_opts))
  local all_opts=($(bld_dedup ${impl_opts[@]} ${opts[@]} ${src_opts[@]}))
  
  ###### diagnostics ########################################################
  local diagnostics=$(bld_has_opt diagnostics ${all_opts[@]})
  
  ###### out file name ######################################################
  local dot_ext_obj=$(bld_ext_obj)
  local file_base=${final_in_file##*/}
  local file_base_no_ext=${file_base%.*}
  local out_file="$file_base_no_ext$dot_ext_obj"
  
  ###### get real flags #####################################################
  local flags=$(bld_flags_from_opts $compiler_flags_path ${all_opts[@]})
  
  ###### move to output folder ##############################################
  mkdir -p "$build_path"
  cd $build_path
  if [ "$diagnostics" == "1" ]; then
    echo "build path: $build_path"
  fi
  
  ###### delete existing object file ########################################
  rm -f "$out_file_base.o"
  rm -f "$out_file_base.obj"
  
  ###### final flags ########################################################
  local final_flags="-c -I$src_path ${flags}"
  
  
  ###### compile ############################################################
  if [ "$diagnostics" == "1" ]; then
    echo "cmp $final_in_file -- ${all_opts[@]}"
    echo $compiler "$final_in_file" $final_flags
  fi
  if [ "$compiler" == "clang" ]; then
    echo "$file_base"
  fi
  $compiler "$final_in_file" $final_flags
  
  # return of status from compiler is automatic here.
}

###### Link ###################################################################

function bld_link {
  ###### parse arguments ####################################################
  local out_name=$1
  local in_files=()
  for ((i=2; i<=$#; i+=1)); do
    if [ "${!i}" == "--" ]; then
      break
    fi
    in_files+=(${!i})
  done
  local opts=()
  for ((i+=1; i<=$#; i+=1)); do
    opts+=(${!i})
  done
  if [ "$out_name" == "" ]; then
    echo "link: missing output name"
    return 1
  fi
  if [ "${#in_files}" == "0" ]; then
    echo "link: missing input file(s)"
    return 1
  fi
  
  ###### finish options #####################################################
  local impl_opts=($(bld_implicit_opts))
  local all_opts=($(bld_dedup ${opts[@]} ${impl_opts[@]}))
  
  ###### diagnostics ########################################################
  local diagnostics=$(bld_has_opt diagnostics ${all_opts[@]})
  
  ###### sort in files ######################################################
  local in_src=()
  local in_obj=()
  local in_lib=()
  for ((i=0; i<${#in_files[@]}; i+=1)); do
    local file="${in_files[i]}"
    local ext="${file##*.}"
    if [[ "$ext" == "c" || "$ext" == "cpp" ]]; then
      in_src+=($file)
    elif [[ "$ext" == "o" || "$ext" == "obj" ]]; then
      in_obj+=($file)
    elif [[ "$ext" == "lib" ]]; then
      in_lib+=($file)
    else
      echo "WARNING: ignoring unrecognized file type $file"
    fi
  done
  
  ###### auto correct object files ##########################################
  local dot_ext_obj=$(bld_ext_obj)
  for ((i=0; i<${#in_obj[@]}; i+=1)); do
    local file_name="${in_obj[i]}"
    local base_name="${file_name%.*}"
    in_obj[$i]="$base_name$dot_ext_obj"
  done
  
  ###### compile source files ###############################################
  for ((i=0; i<${#in_src[@]}; i+=1)); do
    bld_compile "${in_src[i]}" ${all_opts[@]}
    local status=$?
    if [ $status -ne 0 ]; then
      return $status
    fi
  done
  
  ###### intermediate object files ##########################################
  local interm_obj=()
  for ((i=0; i<${#in_src[@]}; i+=1)); do
    local file_name="${in_src[i]}"
    local base_name="${file_name##*/}"
    local base_name_no_ext="${base_name%.*}"
    interm_obj+=($base_name_no_ext$dot_ext_obj)
  done
  
  ###### get real flags #####################################################
  local flags=$(bld_flags_from_opts $linker_flags_path ${all_opts[@]})
  
  ###### out file name ######################################################
  local dot_ext_out=""
  local is_dll=$(bld_has_opt dll ${all_opts[@]})
  if [ "$is_dll" == "0" ]; then
    dot_ext_out=$(bld_ext_exe)
  else
    dot_ext_out=$(bld_ext_dll)
  fi
  out_file="$out_name$dot_ext_out"
  
  ###### move to output folder ##############################################
  mkdir -p "$build_path"
  cd $build_path
  if [ "$diagnostics" == "1" ]; then
    echo "build path: $build_path"
  fi
  
  ###### final files to linker ##############################################
  local final_in_files="${interm_obj[@]} ${in_obj[@]} ${in_lib[@]}"
  
  ###### set first diagnostic string ########################################
  local first_diagnostic_string="lnk $final_in_files -- ${all_opts[@]}"
  
  ###### link ###############################################################
  local status=0
  local invokation=""
  if [[ "$linker" == "link" || "$linker" == "lld-link" ]]; then
    invokation="$linker -OUT:$out_file $flags $final_in_files"
  elif [ "$linker" == "clang" ]; then
    invokation="$linker -o \"$out_file\" $flags $final_in_files"
  else
    echo "ERROR: invokation not defined for this linker"
    status=1
  fi
  if [ "$invokation" != "" ]; then
    if [ "$diagnostics" == "1" ]; then
      echo $first_diagnostic_string
      echo $invokation
    fi
    echo "$out_file"
    $invokation
    status=$?
  fi
  return $status
}

###### Library ################################################################

function bld_lib {
  ###### parse arguments ####################################################
  local out_name=$1
  local in_files=()
  for ((i=2; i<=$#; i+=1)); do
    if [ "${!i}" == "--" ]; then
      break
    fi
    in_files+=(${!i})
  done
  local opts=()
  for ((i+=1; i<=$#; i+=1)); do
    opts+=(${!i})
  done
  if [ "$out_name" == "" ]; then
    echo "lib: missing output name"
    return 1
  fi
  if [ "${#in_files}" == "0" ]; then
    echo "lib: missing input file(s)"
    return 1
  fi
  
  ###### finish options #####################################################
  local impl_opts=($(bld_implicit_opts))
  local all_opts=($(bld_dedup ${opts[@]} ${impl_opts[@]}))
  
  ###### diagnostics ########################################################
  local diagnostics=$(bld_has_opt diagnostics ${all_opts[@]})
  
  ###### sort in files ######################################################
  local in_src=()
  local in_obj=()
  for ((i=0; i<${#in_files[@]}; i+=1)); do
    local file="${in_files[i]}"
    local ext="${file##*.}"
    if [[ "$ext" == "c" || "$ext" == "cpp" ]]; then
      in_src+=($file)
    elif [[ "$ext" == "o" || "$ext" == "obj" ]]; then
      in_obj+=($file)
    else
      echo "WARNING: ingnoring unrecgonized file type $file"
    fi
  done
  
  ###### auto correct object files ##########################################
  local dot_ext_obj=$(bld_ext_obj)
  for ((i=0; i<${#in_obj[@]}; i+=1)); do
    local file_name="${in_obj[i]}"
    local base_name="${file_name%.*}"
    in_obj[$i]=$base_name$dot_ext_obj
  done
  
  ###### compile source files ###############################################
  for ((i=0; i<${#in_src[@]}; i+=1)); do
    bld_compile "${in_src[i]}" ${all_opts[@]}
    local status=$?
    if [ $status -ne 0 ]; then
      return $status
    fi
  done
  
  ###### intermediate object files ##########################################
  local interm_obj=()
  for ((i=0; i<${#in_src[@]}; i+=1)); do
    local file_name="${in_src[i]}"
    local base_name="${file_name##*/}"
    local base_name_no_ext="${base_name%.*}"
    interm_obj+=($base_name_no_ext$dot_ext_obj)
  done
  
  ###### out file name ######################################################
  local out_file=""
  if [ "$os" == "windows" ]; then
    out_file="$out_name.lib"
  elif [ "$os" == "linux" || "$os" == "mac" ]; then
    out_file="lib$out_name.a"
  else
    echo "ERROR: static library output not defined for OS: $os"
  fi
  
  ###### final library build input files ####################################
  local final_in_files="${interm_obj[@]} ${in_obj[@]}"
  
  ###### move to output folder ##############################################
  mkdir -p "$build_path"
  cd $build_path
  if [ "$diagnostics" == "1" ]; then
    echo "build path: $build_path"
  fi
  
  ###### set first diagnostic string ########################################
  local first_diagnostic_string="lib $final_in_files -- ${all_opts[@]}"
  
  ###### build library ######################################################
  local status=0
  if [ "$os" == "windows" ]; then
    if [ "$diagnostics" == "1" ]; then
      echo $first_diagnostic_string
      echo lib -nologo -OUT:"$out_file" $final_in_files
    fi
    echo "$out_file"
    lib -nologo -OUT:"$out_file" $final_in_files
    status=$?
  elif [ "$os" == "linux" || "$os" == "mac" ]; then
    # TODO(allen): invoke ar here - make sure to delete the original .a first
    # because ar does not (seem) to replace the output file, just append
    echo "TODO: implement ar path in bld_core.sh:bld_lib"
    status=1
  else
    echo "ERROR: static library invokation not defined for OS: $os"
    status=1
  fi
  
  return $status
}

###### Unit ###################################################################

function bld_unit {
  ###### parse arguments ####################################################
  local main_file=$1
  local opts=()
  for ((i=2; i<=$#; i+=1)); do
    opts+=(${!i})
  done
  if [ "$main_file" == "" ]; then
    echo "unit: missing main file"
    return 1
  fi
  
  ###### set out name #######################################################
  local out_name=""
  if [ "${#opts}" == "0" ]; then
    local file_base=${main_file##*/}
    local file_base_no_ext=${file_base%.*}
    out_name=$file_base_no_ext
  else
    out_name="${opts[0]}"
  fi
  
  ###### finish options #####################################################
  local src_opts=$(bld_opts_from_src $main_file)
  local impl_opts=($(bld_implicit_opts))
  local all_opts=($(bld_dedup $out_name ${opts[@]} ${src_opts[@]} ${impl_opts[@]}))
  
  ###### link ###############################################################
  bld_link $out_name $main_file ${in_files[@]} -- ${all_opts[@]}
}

###### Special Ifs ############################################################

function bld_ext_obj {
  echo $(bld_flags_from_opts $file_extensions_path $compiler obj)
}
function bld_ext_exe {
  echo $(bld_flags_from_opts $file_extensions_path $os exe)
}
function bld_ext_dll {
  echo $(bld_flags_from_opts $file_extensions_path $os dll)
}

###### Get Paths ##############################################################
if [ ! -f "$bld_path/bld_core.sh" ]; then
 echo bld_path set incorrectly
 exit 1
fi
local_path="$bld_path/local"

###### Load Locals ############################################################
bld_load_local_opts


# NOTES - for future iterations of this system
# + The "source" approach creates problems for locating the bld path because
# when a script is "sourced" it inherits the $0 from the "caller". Two ideas:
#   1. There may be a way to fix this issue relating to passing parameters
#      to the "sourced" script
#   2. Setup a locator script that can be called the normal way before sourcing
#      the core to save the bld_path for the core to see
# + It would be nice to include an automatic slash fixer \ -> /

