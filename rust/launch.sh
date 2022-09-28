#!/bin/bash
set -euo pipefail
curdir=$(cd $(dirname ${BASH_SOURCE[0]}) && pwd)
source ${curdir}/env.sh

pids=()

function clean_up {
  for pid in ${pids[@]}; do
    echo pid $pid
    sudo kill $pid
  done
}

trap clean_up SIGHUP SIGINT SIGTERM

${curdir}/http/target/release/http &
pids=(${pids[@]} $!)
${curdir}/client/client &
pids=(${pids[@]} $!)

for pid in ${pids[@]}; do
  wait $pid
done


