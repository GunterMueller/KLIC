lib_LTLIBRARIES = librmcast.la
nobase_include_HEADERS = rmcast.h

librmcast_la_SOURCES= rmcache.c rmcache.h rmcast.c rmcast.h rmevent.c rmevent.h \
                     rminternals.c rminternals.h rmmsg.c rmmsg.h rmmsgpckt.c \
                     rmmsgpckt.h rmstruct.h rmtcp.c rmtcp.h rmwinmask.c \
                     rmwinmask.h
librmcast_la_CFLAGS = -Wall -fno-strict-aliasing \
                      -DSTR_VERSION=\"$(PACKAGE_VERSION)\" \
                      -DRANDOM_TIMERS -DREFRESH -DSINGLE_NACK \
                      -DSUPPORT_FOR_NEW_MEMBERS -D_REENTRANT -DMALLOC_CHECK_ \
                      -D_POSIX_PTHREAD_SEMANTICS
