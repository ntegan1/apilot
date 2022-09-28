#!/bin/bash
set -euo pipefail
curdir=$(cd $(dirname ${BASH_SOURCE[0]}) && pwd)

# this information is in inc/config.h also
_filename='server.pem'
server_pem_file=${curdir}/${_filename}
function already_exists() { test -f "${server_pem_file}"; }

function need_openssl() { which openssl 2>/dev/null 1>&2 || \
  (echo needopenssl; exit 1)
}
function generate_pem() {
  outfile="${server_pem_file}"
  need_openssl
  echo generating
  (! yes '' | openssl req -new -x509 \
    -keyout ${outfile} \
    -out ${outfile} \
    -days 365 -nodes) 2>/dev/null 1>&2
  # make sure it exists now
  already_exists
}

already_exists || generate_pem


