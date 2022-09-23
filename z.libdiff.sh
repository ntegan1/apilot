#!/bin/bash

_diff_string=""
_diff_array=()
_diff_arrayLength=0
_diff_update() {
  _diff_updateArray
}
_diff_idxs_submodules() {
  submodules="$(git --git-dir="${g}" submodule | awk '{print $2}')"
  for i in $(seq ${_diff_arrayLength}); do
    i=$((i-1))
    name="$(_diff_filenameA ${i})"
    echo "${submodules}" | grep -q "${name}" && echo $i
  done
}
_diff_idx_gitmodules() {
  for i in $(seq ${_diff_arrayLength}); do
    i=$((i-1))
    name="$(_diff_filenameA ${i})"
    if test "${name}" = ".gitmodules"; then
      echo $i
      return
    fi
  done
  echo '-1'
}
_diff_updateArray() {
  a=0
  b=""
  _diff_arrayLength=0
  _diff_array=()
  while read -r line; do
    if grep -q "^diff" <<<"${line}"; then
      if test ${a} -gt 0; then _diff_array+=("${b}"); fi
      a=$((a + 1))
      b=""
    fi
    b="${b}""${line}""
"
  done <<< "${_diff_string}"
  #if test ${a} -gt 0; then _diff_array=(${_diff_array[@]} "${b}"); fi
  if test ${a} -gt 0; then _diff_array+=("${b}"); fi
  _diff_arrayLength=${a}
}
_diff_filenames() {
  idx=$1
  firstline="$(echo "${_diff_array[${idx}]}" | head -n 1)"
  #diff --git a/.gitmodules b/.gitmodules
  #diff --git a/tinygrad_repo b/tinygrad_repo

  a="$(echo "${firstline}" | awk '{print $3}')"
  b="$(echo "${firstline}" | awk '{print $4}')"

  echo "${a/a\//}" "${b/b\//}"
}
_diff_filenameA() {
  idx=$1
  _diff_filenames ${idx} | awk '{print $1}'
}
diff_set() { # usage: diff_set <file_or_string>
  a="$1"
  if test -f "${a}"; then a="$(cat "${a}")"; fi

  # a is now string content, not potentially a filename
  _diff_string="${a}"

  diff_update
}
diff_update() {
  _diff_update
}
diff_num() {
  echo ${_diff_arrayLength}
}
diff_get() {
  a="$1"
  if test $a -ge ${_diff_arrayLength} || test $a -lt 0; then return; fi

  echo "${_diff_array[${a}]}"
}
diff_get_gitmodules() {
  idx="$(_diff_idx_gitmodules)"
  if test $idx -ge ${_diff_arrayLength} || test $idx -lt 0; then return; fi
  echo "${_diff_array[${idx}]}"
}
diff_get_submodules() {
  idxs="$(_diff_idxs_submodules)"
  for i in ${idxs}; do
    echo "${_diff_array[${i}]}"
  done
}
diff_get_nomodules() {
  idxs="$(_diff_idx_gitmodules)"" ""$(_diff_idxs_submodules)"
  for i in $(seq ${_diff_arrayLength}); do
    i=$((i-1))
    if echo "${idxs}" | grep -q $i; then
      continue
    else
      echo "${_diff_array[${i}]}"
    fi
  done
}


