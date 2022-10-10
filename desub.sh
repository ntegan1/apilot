#!/bin/bash
set -euo pipefail
# ntegan@nhost ~/openpilot.master (git)-[master] % git submodule
#  35122285b97ba467b715e749f5e99834ea91c0fe cereal (3512228)
#  485ab4a176ea66426549e79a403e7e657fff6534 laika_repo (heads/master)
#  80e7b94ae9f7451235bd802fec04d2f5e601083b opendbc (heads/master)
#  1b301600eea35092d1bfd6fde01184907d7d2354 panda (remotes/origin/boardesp-archive
# -82-g1b30160)
#  7fff192e234cd91f50deed12adb3616d96f12d3e rednose_repo (heads/master)
test "$(git submodule)" = "" && echo alraedy de-submodulized && exit 0
git submodule deinit -f --all
git submodule update --init --recursive
for f in cereal opendbc panda tinygrad_repo ; do
	git rm --cached $f
	rm -rf $f/.git
	git add $f
  if test "$f" = "panda"; then
    git add -f $f/board/obj/.placeholder
  fi
	git commit -m "de-submodulize $f"
done
for f in laika_repo rednose_repo; do
	git rm --cached $f
	rm -rf $f/.git
	new=${f/_repo/}
	rm -rf $new
	mv $f/$new $new
	git add $new
	rm -rf $f
	git commit -m "de-submodulize $f to $new"
done
