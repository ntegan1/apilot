#!/bin/bash
myname=$(basename ${BASH_SOURCE[0]})
mydir=$(cd $(dirname ${BASH_SOURCE[0]}) && pwd)

cd ${mydir}

test "$#" = "1" && vim -c 'tabnew | tabdo execute ":term bash ./launch.sh"'
test "$#" = "1" && exit 0

a='/data/._za'
rm -f "${a}"
sleep 0.2s

myrand="$RANDOM"
echo "$myrand" >> "${a}"
sync
sleep 0.2s

bRunClient=y
firstRand="$(head -n 1 ${a})"
echo $myrand
echo $firstRand
test "${firstRand}" = "$myrand"


test "${firstRand}" = "$myrand" && while true; do sleep 5; ip a | grep -q '192\.168\.43\.1' && break; done; sudo ./_shmServer
test "${firstRand}" = "$myrand" && exit 0
./_newService
exit 0

#vim -c 'helpgrep terminal-info | cnext | cnext'
#vim -c 'tabnew +vert\ term\ bash'

#vim -c "$cmdstring"
#tabdo execute ':term echo hi'

