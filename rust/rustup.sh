#!/bin/bash
set -euo pipefail
curdir=$(cd $(dirname ${BASH_SOURCE[0]}) && pwd)
source ${curdir}/env.sh

if $(which cargo); then
  exit 0
else
  echo u need cargo
  echo "export CARGO_HOME=/data/cargo; export RUSTUP_HOME=/data/rustup; curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh"
fi

