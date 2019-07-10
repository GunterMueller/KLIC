# ---------------------------------------------------------- 
#   (C) 1995 Institute for New Generation Computer Technology 
#	(Read COPYRIGHT for detailed information.) 
#   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
#       (Read COPYRIGHT-JIPDEC for detailed information.)
# ----------------------------------------------------------

if $CROSS; then
cat <<GAZONK >>Makefile

KL1SELFCOMP=

all:	cross

install:	install-basics

GAZONK
else
cat <<GAZONK >>Makefile

KL1SELFCOMP=kl1cmp

all:	self

install:	install-basics

GAZONK
fi
