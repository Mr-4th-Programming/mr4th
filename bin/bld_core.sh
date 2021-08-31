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


###### Testing ################################################################
echo "implicit opts: ${implicit_opts[@]}"

###### Restore Path ###########################################################
cd $og_path
