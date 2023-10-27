#
# @(#)$Header: /mm2/home/cvs/bc-src/tgif/scan2ac.awk,v 1.9 2010/09/02 18:56:44 william Exp $
#
BEGIN { triggered=0; }
{   if (/^# Checks for libraries/) { triggered=1; }
    if (/^# Checks for header files/) {
        triggered=0;
        print "# Checks for libraries.";
        print "AC_CHECK_LIB([X11], [XOpenDisplay])";
        print "AC_CHECK_LIB([Xt], [XtInitialize])";
        print "AC_CHECK_LIB([socket], [socket])";
        print "AC_CHECK_LIB([nsl], [inet_addr])";
        print "AC_CHECK_LIB([dl], [dlsym])";
        print "AC_CHECK_LIB([idn], [stringprep_convert])";
        print "AC_CHECK_LIB([m], [pow])";
        print "AC_CHECK_LIB([z], [deflate])";
        print "AC_CHECK_LIB([pthread], [pthread_create])";
        print "";
        print "# Checks for header files.";
        next;
    } else {
        if (triggered) {
            next;
        } else {
            if (/^AC_CONFIG_SUBDIRS\(\[rmcast\]\)/) {
                print "# BC: skipping AC_CONFIG_SUBDIRS([rmcast])";
            } else {
                if (/^AC_PREREQ\(/) {
                    print "AC_PREREQ(2.61)";
                } else {
                    if (/^AC_CONFIG_HEADER\(\[config.h\]\)/) {
                        print "AM_INIT_AUTOMAKE";
                        print "AM_CONFIG_HEADER([config.h])";
                    } else {
                        if (/^AC_CONFIG_HEADERS\(\[config.h\]\)/) {
                            print "AM_INIT_AUTOMAKE";
                            print "AC_CONFIG_HEADERS([config.h])";
                        } else {
                            print;
                        }
                    }
                }
            }
        }
    }
}
