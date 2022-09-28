#!/bin/bash
set -euo pipefail
curdir=$(cd $(dirname ${BASH_SOURCE[0]}) && pwd)

# get bind ip
source ${curdir}/env.sh

keydir=${curdir}/keys

curbind=${keydir}/bind_address
key=${keydir}/server.key.pem
cert=${keydir}/server.cert.pem
pfx=${keydir}/server.pfx

function need_openssl { which openssl 2>/dev/null 1>&2 || \
  (echo needopenssl; exit 1)
}
function already_exists {
  test -f "${key}" -a -f "${cert}" -a -f "${pfx}" -a -f "${curbind}"
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

  openssl pkcs12 -export \
    -out ${pfx} -inkey ${key} -in ${cert} \
    -password pass:'test'
}



need_openssl

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

