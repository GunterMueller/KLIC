# ---------------------------------------------------------- 
#   (C)1995 Institute for New Generation Computer Technology 
#       (Read COPYRIGHT for detailed information.) 
#   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
#       (Read COPYRIGHT-JIPDEC for detailed information.)
#-----------------------------------------------------------
cat <<GAZONK

DISTCFLAGS = \$(CFLAGS) -DDEBUGLIB -DDIST -DSHM_DIST -I../include

EXT_OTHER_HEADERS =

DIST_OTHER_SRCS = distpkt.c distio.c distproc.c distrmon.c lock.s

DIST_OTHER_OBJS = distpkt.o distio.o distproc.o distrmon.o lock.o

RMON_SERVER_SRCS = rmon_server.c

RMON_SERVER_OBJS = rmon_server.o

install-rmon_server: rmon_server
	- \$(KLICLIB)
	/bin/cp -p rmon_server \$(KLICLIB)/klic/rmon_server

EXTENSION = rmon_server

INSTALL_EXTENSION = install-rmon_server

rmon_server: rmon_server.c
	\$(CC) -o rmon_server \$(DISTCFLAGS) rmon_server.c \
			\$(LDFLAGS)

distpkt.o: distpkt.c
	\$(CC) \$(DISTCFLAGS) -c distpkt.c
distio.o: distio.c ../include/klic/distpkt.h
	\$(CC) \$(DISTCFLAGS) -c distio.c
distproc.o: distproc.c
	\$(CC) \$(DISTCFLAGS) -c distproc.c
distrmon.o: distrmon.c
	\$(CC) \$(DISTCFLAGS) -c distrmon.c
lock.o:	lock.s
	\$(CC) -c lock.s
GAZONK
