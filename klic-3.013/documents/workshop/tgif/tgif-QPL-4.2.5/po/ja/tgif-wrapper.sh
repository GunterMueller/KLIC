#!/bin/bash

if echo $LANG | grep -q ja_JP ; then
	export LANG=ja_JP.eucJP
fi

exec /usr/libexec/tgif $*

