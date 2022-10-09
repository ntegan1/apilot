#!/bin/bash
set -euo pipefail
curdir=$(cd $(dirname ${BASH_SOURCE[0]}) && pwd)

# get bind ip
source ${curdir}/env.sh

keydir=${curdir}/keys

curbind=${keydir}/bind_address
key=${keydir}/server.key.pem
cert=${keydir}/server.cert.pem

function already_exists {
  test -f "${key}" -a -f "${cert}" -a -f "${curbind}"
}
function new_bind {
  test ! "$(cat ${curbind})" = "${BIND}"
}

function generate_keys {
  echo "${BIND}" > ${curbind}
    #-addext basicConstraints=critical,CA:FALSE \
  openssl req -x509 \
    -addext "subjectAltName=DNS:espncs.com,DNS:localhost,IP:${BIND}" \
    -nodes -days 365 -newkey rsa:2048 \
    -keyout ${key} -out ${cert} -subj /CN="${BIND}"
}


if $(already_exists); then
  echo keys already exist
  if $(new_bind); then
    rm -f ${keydir}/*
  else
  exit 0
  fi
fi

rm -f ${keydir}/*
generate_keys
