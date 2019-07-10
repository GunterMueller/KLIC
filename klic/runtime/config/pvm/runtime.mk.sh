# ---------------------------------------------------------- 
#   (C)1995 Institute for New Generation Computer Technology 
#       (Read COPYRIGHT for detailed information.) 
#   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
#       (Read COPYRIGHT-JIPDEC for detailed information.)
#-----------------------------------------------------------

if test "$PVM_ARCH" = "AP3000"; then
DISTLDFLAGS="-lmpl -lemi -lthread"
else
if test "$PVM_ARCH" = "VPP300" -o "$PVM_ARCH" = "VPP700"; then
DISTLDFLAGS="-Wl,-P,-dy -Wv,-sc,-ad,-Oe,-P /usr/lang/pvm/lib/VPP300/libmp.a -lgen -lelf -lsocket -lpx -lm -lc"
else
DISTLDFLAGS=
fi
fi

cat <<GAZONK

PVMINC =  $PVM_ROOT/include
PVMLIBDIR = $PVM_ROOT/lib/$PVM_ARCH
PVMLIB  = $PVMLIB

DISTCFLAGS = \$(CFLAGS) -DDEBUGLIB -DDIST -I\$(PVMINC) -I../include

DISTLDFLAGS = $DISTLDFLAGS

EXT_OTHER_HEADERS =

DIST_OTHER_SRCS = distpkt.c distio.c distproc.c distrmon.c

DIST_OTHER_OBJS = distpkt.o distio.o distproc.o distrmon.o

RMON_SERVER_SRCS = rmon_server.c

RMON_SERVER_OBJS = rmon_server.o

install-rmon_server: rmon_server
	- \$(KLICLIB)
	/bin/cp -p rmon_server \$(KLICLIB)/klic/rmon_server

EXTENSION = rmon_server

INSTALL_EXTENSION = install-rmon_server

rmon_server: rmon_server.c
	\$(CC) -o rmon_server \$(DISTCFLAGS) rmon_server.c \\
			-L\$(PVMLIBDIR) -l\$(PVMLIB) \$(LDFLAGS) \$(DISTLDFLAGS)

distpkt.o: distpkt.c
	\$(CC) \$(DISTCFLAGS) -c distpkt.c
distio.o: distio.c ../include/klic/distpkt.h
	\$(CC) \$(DISTCFLAGS) -c distio.c
distproc.o: distproc.c
	\$(CC) \$(DISTCFLAGS) -c distproc.c
distrmon.o: distrmon.c
	\$(CC) \$(DISTCFLAGS) -c distrmon.c
GAZONK
