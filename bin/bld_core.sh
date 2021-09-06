#!/bin/bash

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
  local final_in_file=$root_path/$in_file
  
  ###### finish options #####################################################
  local src_opts=($(bld_opts_from_src $final_in_file))
  # TODO(allen): implicit options here?
  local all_opts=($(bld_dedup ${opts[@]} ${src_opts[@]}))
  
  ###### out file name ######################################################
  local file_base=${final_in_file##*/}
  local file_base_no_ext=${file_base%.*}
  local out_file="$file_base_no_ext$dot_ext_obj"
  
  ###### get real flags #####################################################
  local flags_file=$bin_path/compiler_flags.txt
  local flags=$(bld_flags_from_opts $flags_file ${all_opts[@]})
  
  ###### move to output folder ##############################################
  mkdir -p "$build_path"
  cd $build_path
  
  ###### delete existing object file ########################################
  rm -f "$out_file_base.o"
  rm -f "$out_file_base.obj"
  
  ###### final flags ########################################################
  local final_flags="-c -I$src_path ${flags}"
  
  ###### compile ############################################################
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
  # TODO(allen): implicit options here?
  local all_opts=($(bld_dedup ${opts[@]}))
  
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
      exit $status
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
  local flags_file=$bin_path/linker_flags.txt
  local flags=$(bld_flags_from_opts $flags_file ${all_opts[@]})
  
  ###### out file name ######################################################
  local bin_kind="exe"
  for ((i=0; i<${#all_opts[@]}; i+=1)); do
    if [[ "${all_opts[i]}" == "dll" ]]; then
      bin_kind="dll"
      break
    fi
  done
  
  dot_ext_out="$dot_ext_exe"
  if [ "$bin_kind" == "dll" ]; then
    dot_ext_out="$dot_ext_dll"
  fi
  out_file="$out_name$dot_ext_out"
  
  ###### move to output folder ##############################################
  mkdir -p "$build_path"
  cd $build_path
  
  ###### Final Files to Linker ##############################################
  local final_in_files="${interm_obj[@]} ${in_obj[@]} ${in_lib[@]}"
  
  ###### link ###############################################################
  local status=0
  if [ "$linker_kind" == "link" ]; then
    echo "$out_file"
    $linker -OUT:"$out_file" $flags $final_in_files
    status=$?
  elif [ "$linker_kind" == "clang" ]; then
    echo "$out_file"
    $linker -o "$out_file" $flags $final_in_files
    status=$?
  else
    echo "ERROR: invokation not defined for this linker"
    status=1
  fi
  
  return $status
}

###### OS Cracking ############################################################
os="undefined"
if [ "$OSTYPE" == "win32" ] ||
   [ "$OSTYPE" == "msys"  ]; then
  os="windows"
elif [ "$OSTYPE" == "linux-gnu" ]; then
  os="linux"
elif [ "$OSTYPE" == "darwin" ]; then
  os="mac"
fi


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


###### Implicit Options #######################################################
implicit_opts=($compiler $compile_mode $os $arch $linker)


###### Object File Extension ##################################################
# TODO(allen): isolating special ifs?
dot_ext_obj=".o"
if [[ "$compiler" == "cl" ]]; then
  dot_ext_obj=".obj"
fi


###### Binary File Extension ##################################################
# TODO(allen): isolating special ifs?
dot_ext_exe=""
dot_ext_dll=""
if [ "$os" == "windows" ]; then
  dot_ext_exe=".exe"
  dot_ext_dll=".dll"
elif [ "$os" == "linux" ] || [ "$os" == "mac" ]; then
  dot_ext_exe=""
  dot_ext_dll=".so"
else
  echo "ERROR: binary extension not defined for OS: $os"
fi


###### Linker Kind ############################################################
# TODO(allen): isolating special ifs?
linker_kind="link"
if [ "$linker" == "clang" ]; then
  linker_kind="clang"
fi


###### Testing ################################################################
echo "implicit opts: ${implicit_opts[@]}"

bld_link test src/main.cpp -- ${implicit_opts[@]}

###### Restore Path ###########################################################
cd $og_path
