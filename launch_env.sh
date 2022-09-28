#!/usr/bin/bash

source /data/openpilot/rust/env.sh

sudo mount -o remount,suid /data
sudo setcap 'cap_net_bind_service=+ep' /data/openpilot/rust/http/target/release/http

export OMP_NUM_THREADS=1
export MKL_NUM_THREADS=1
export NUMEXPR_NUM_THREADS=1
export OPENBLAS_NUM_THREADS=1
export VECLIB_MAXIMUM_THREADS=1

if [ -z "$AGNOS_VERSION" ]; then
  export AGNOS_VERSION="6.1"
fi

if [ -z "$PASSIVE" ]; then
  export PASSIVE="1"
fi

export STAGING_ROOT="/data/safe_staging"
