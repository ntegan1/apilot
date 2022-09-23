#!/bin/bash

_diff_string=""
_diff_array=()
_diff_arrayLength=0
_diff_update() {
  _diff_updateArray
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
diff_idxs_gitmodules() {
  for i in $(seq ${_diff_arrayLength}); do
    diff="${_diff_array[$((i - 1))]}"
  done

}
