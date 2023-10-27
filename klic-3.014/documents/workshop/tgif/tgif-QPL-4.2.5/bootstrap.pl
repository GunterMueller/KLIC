#!/usr/bin/perl
#
# @(#)$Header: /mm2/home/cvs/bc-src/tgif/bootstrap.pl,v 1.16 2011/05/23 16:35:57 william Exp $
#
# bootstrap.pl version

$skip_autoscan = 0;

$num_cmd_args = $#ARGV +1;

if ($num_cmd_args != 1) {
    print "usage: bootstrap.pl version\n";
    exit 0;
}

$version = $ARGV[0];

$autodir = "/usr/share/automake-1.10";
$email = "bill.cheng\@acm.org";

print "clean up files...\n";
system("rm -rf autom4te.cache");
system("rm -f Makefile Makefile.distr Makefile.in aclocal.m4 autoscan.log");
system("rm -f configure configure.ac config.h.in config.h");

if (!$skip_autoscan) {
    #
    # These files will be generated, delete them to make sure that we start
    #         from scratch
    #
    system("rm -f mkinstalldirs install-sh missing depcomp");
    system("rm -f config.status configure.scan");

    #
    # Need these from for the script to run
    #
    print "copy files from $autodir...\n";
    system("cp $autodir/mkinstalldirs $autodir/install-sh .");
    system("cp $autodir/missing $autodir/depcomp .");

    print "running autoscan...\n";
    system("autoscan");
}

print "generating configure.ac...\n";

$init_pat = "AC_INIT(FULL-PACKAGE-NAME, VERSION, BUG-REPORT-ADDRESS)";
$init_dat = "AC_INIT(tgif, $version, $email)";
$cmd = "sed -e '1,\$s/$init_pat/$init_dat/' configure.scan > configure.1.$$";
system($cmd);

$init_pat = "AC_INIT(\\\[FULL-PACKAGE-NAME\\\], \\\[VERSION\\\], \\\[BUG-REPORT-ADDRESS\\\])";
$init_dat = "AC_INIT([tgif], [$version], [$email])";
$cmd = "sed -e '1,\$s/$init_pat/$init_dat/' configure.1.$$ > configure.$$";
system($cmd);

system("awk -f scan2ac.awk configure.$$ > configure.ac");
system("rm -f configure.1.$$ configure.$$");

print "running aclocal...\n";
system("aclocal");

print "running autoheader...\n";
system("autoheader");

print "running automake -ac...\n";
system("automake -ac");

print "running autoconf...\n";
system("autoconf");

print "running configure...\n";
system("./configure; cp Makefile Makefile.distr; make clean");
