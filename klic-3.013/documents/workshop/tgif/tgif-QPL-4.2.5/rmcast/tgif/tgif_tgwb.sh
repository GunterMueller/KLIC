#!/bin/bash
################################################################################
#  File: tgif_tgwb.sh
#  Author: {allyson}@land.ufrj.br
#  Date: 2006-03-30
#
#  Description: compiles TGWB tgif source code
#
#  Copyright 2006 LAND/UFRJ
#  http://www.land.ufrj.br
#
################################################################################

if [ $# != 1 ]; then
    echo
    echo "Usage:"
    echo
    echo "    $0 <tgif source dir>"
    echo
    exit 1
fi

if [ ! -d $1 ]; then
    echo
    echo "[tgif_tgwb] $1 is not a directory"
    echo
    exit 1
fi

cd $1
if [ ! -f Tgif.tmpl ]; then
    echo
    echo "[tgif_tgwb] Tgif.tmpl not found"
    echo
    exit 1
fi

if [ ! -f Target.tmpl ]; then
    echo
    echo "[tgif_tgwb] Target.tmpl not found"
    echo
    exit 1
fi

if [ ! -w Tgif.tmpl ]; then
    chmod u+w Tgif.tmpl
fi

if [ ! -w Target.tmpl ]; then
    chmod u+w Target.tmpl
fi

mv Target.tmpl Target.tmpl.orig
sed -e 's/TARGET_1 = tgif/TARGET_1 = tgwb-bin/' Target.tmpl.orig > Target.tmpl.tgwb
if [ $? = 0 ]; then
    echo
    echo "[tgif_tgwb] File Target.tmpl successfully modified"
    echo
else
    echo
    echo "[tgif_tgwb] Failed trying to modify Target.tmpl"
    echo
    exit 1
fi
mv Target.tmpl.tgwb Target.tmpl

mv Tgif.tmpl Tgif.tmpl.orig
sed '/^SYS_LIBRARIES/ a\EXTRA_LIBRARIES = -L./rmcast/src -lpthread -lrmcast' Tgif.tmpl.orig | sed -e 's/= tgif/= tgwb-bin/' -e 's/^MOREDEFINES.*$/MOREDEFINES = -D_TGIF_WB2 -D_TANGRAM_II -DDEFATTRGROUP=\\\"TANGRAM-II:Declaration:Initialization:Events:Messages:Rewards:Watches\\"/' > Tgif.tmpl.tgwb
if [ $? = 0 ]; then
    echo
    echo "[tgif_tgwb] File Tgif.tmpl successfully modified"
    echo
else
    echo
    echo "[tgif_tgwb] Failed trying to modify Tgif.tmpl"
    echo
    exit 1
fi

mv Tgif.tmpl.tgwb Tgif.tmpl

xmkmf 
if [ $? = 0 ]; then
    echo
    echo "[tgif_tgwb] Makefile successfully created by xmkmf" 
    echo
else
    echo
    echo "[tgif_tgwb] Makefile creation by xmkmf failed"
    echo
    exit 1
fi

ln -sf tgif.man tgwb-bin.man

cd rmcast
./configure
if [ $? = 0 ]; then
    echo
    echo "[tgif_tgwb] rmcast's Makefile successfully created by configure"
    echo
else
    echo
    echo "[tgif_tgwb] rmcast's Makefile creation by configure failed"
    echo
    cd ..
    exit 1
fi

make clean all
if [ $? = 0 ]; then
    echo
    echo "[tgif_tgwb] rmcast library successfully compiled"
    echo
else
    echo
    echo "[tgif_tgwb] rmcast library compilation failed"
    echo
    cd ..
    exit 1
fi

cd ..
make clean all 
if [ $? = 0 ]; then
    echo
    echo "[tgif_tgwb] TGWB successfully compiled"
    echo
else
    echo
    echo "[tgif_tgwb] TGWB compilation failed"
    echo
    exit 1
fi

echo
echo "[tgif_tgwb] To install TGWB in your system, run as root: make install-tgwb"
echo
