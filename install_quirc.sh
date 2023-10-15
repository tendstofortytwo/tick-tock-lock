#!/usr/bin/env bash

pushd quirc
	PORTLIBS_PREFIX=$(${DEVKITPRO}/portlibs_prefix.sh 3ds)
	source "${DEVKITPRO}/3dsvars.sh"
	make libquirc.a
	sudo cp libquirc.a "${PORTLIBS_PREFIX}/lib/"
	sudo cp lib/quirc.h "${PORTLIBS_PREFIX}/include/"
popd
