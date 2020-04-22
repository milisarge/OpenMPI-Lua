#!/bin/bash
c_code=$1
filename=$(basename -- "$c_code")
extension="${filename##*.}"
bname="${filename%.*}"
l_code=\"${bname}.lua\"
#echo "${l_code}"
LC_ALL=C mpicc -DLUA="${l_code}" $c_code -llua5.3
