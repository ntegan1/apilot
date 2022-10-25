#!/bin/bash
set -euo pipefail
curdir=$(cd $(dirname ${BASH_SOURCE[0]}) && pwd)
keydir=${curdir}

# get bind ip
curbind=${keydir}/bind_address
prevbind=${keydir}/.prev_bind_address
test ! -f ${prevbind} && echo '192.168.43.1' > ${prevbind}
test ! -f ${curbind} && cat ${prevbind} > ${curbind}

key=${keydir}/server.key.pem
cert=${keydir}/server.cert.pem

function already_exists {
  test -f "${key}" -a -f "${cert}" -a -f "${curbind}"
}
function new_bind {
  test ! "$(cat ${curbind})" = "$(cat ${prevbind})"
}

function generate_keys {
  cat ${curbind} > ${prevbind}
    #-addext basicConstraints=critical,CA:FALSE \
  openssl req -x509 \
    -addext "subjectAltName=DNS:espncs.com,DNS:localhost,IP:$(cat ${curbind})" \
    -nodes -days 365 -newkey rsa:2048 \
    -keyout ${key} -out ${cert} -subj /CN="$(cat ${curbind})"
}

if $(already_exists); then
  echo keys already exist
  if $(new_bind); then
    rm -f ${keydir}/*pem
  else
  exit 0
  fi
fi

rm -f ${keydir}/*pem
generate_keys


