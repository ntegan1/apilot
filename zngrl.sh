#!/bin/bash
set -euo pipefail


# vars
a="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"           # script dirname
b="$(basename "${BASH_SOURCE[0]}")"                         # ^^ filename
c="${a}"/"${b/.sh/.diff}"                                   # diff out file
d='https://github.com/commaai/openpilot/compare/master...ngrl.diff'
e="${a}"/"z.libdiff.sh"
f="$1"


# functions
source "${e}"
get_file(){ if test ! -f "${c}"; then curl -L "${d}" -o "${c}"; fi }; cmdeq(){ test "${f}" = "$1"; }
load_diffs(){ diff_set "${c}"; diff_update; }


# main
get_file # if it doesn't already exist

if cmdeq "cat"; then cat "${c}";
elif cmdeq "num"; then load_diffs; diff_num
elif cmdeq "get"; then diff_num=$2; load_diffs; diff_get ${diff_num}
elif cmdeq "dev"; then
  load_diffs
  #gm=($(diff_idxs_gitmodules))
  diff_idxs_gitmodules
  #tg=($(diff_get_submodules))
fi


