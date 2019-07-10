	    =============================================
	    KL1 to C compiler system: KLIC (version 3.011)

			     April 21, 2018

		   Kazunori UEDA, Waseda University
		   ueda -at- ueda.info.waseda.ac.jp
	    =============================================

This version of KLIC is essentially a maintenance release of
klic-3.003 (official version previously released from ICOT and JIPDEC)
and its extension, klic-3.005 (developed at Waseda University by
Hiroshi Igarashi, Yusuke Takagi, Ryo Matsumura and Yusuke Kaneki).

It contains a sequential core and a distributed-memory parallel
implementation.  It is known to work on

- Linux (Ubuntu 18.04LTS with GCC 7.3.0, 4.3.0-Gentoo with GCC 4.9.4,
  CentOS release 7.6 with GCC 4.8.5),
- Mac OS X version 10.11.4 (El-Capitan) with GCC 6.1.0 and
  LLVM 7.3.0 (see Note below), and
- Cygwin (32-bit) version 2.5.1-1 with GCC 5.3.0,

all as 32-bit applications.

Despite its name, the distributed-memory parallel implementation runs
perfectly on recent SMP (symmetric multiprocessor) systems as well as
single-processor systems with multi-core CPUs.

Please see README or README.j (Japanese) for the rest of the
introduction, which is still valid except for hardware-specific
information and contact addresses.  Further details can be found in
the KLIC User's Manual (klic.pdf, klicj.pdf) in the "documents"
directory.

INSTALLATION

For convenience of installation, this distribution includes
configuration files containing typical settings for several OSes
(config.sh.*) inclulding Ubunt, CentOS, and Mac OS X.  We'd suggest
copying one of them to config.sh (say by "cp config.sh.CentOS
config.sh"), making changes to library paths if necessary, and run
"./Configure".

Because KLIC runs as a 32-bit application, please make sure that
32-bit C libraries have been prepared.  For Ubuntu and CentOS, they
can be installed as:

  (Ubuntu) sudo apt install gcc-multilib
  (CentOS) sudo yum install glibc-devel
           sudo yum install glibc-devel.i686
           sudo yum install glibc-static
           sudo yum install glibc-static.i686

Further details of the installation procedure can be found in INSTALL
and INSTALL.j (Japanese).

  Note: for Mac OS X Darwin, LLVM 7.0-7.3 with -O2 may not properly
  compile C files generated from KLIC.  In this case, please use -O1
  to compile C files generated from KLIC (see and use the setting of
  DEF_KLICCFLAGS in config.sh.MacOSX).  Alternatviely, you can
  download GNU GCC from http://hpc.sourceforge.net/ or elsewhere and
  use it.

RUNNING KLIC

How to compile KL1 programs and to run compiled binaries can be found
in Chapter 4 of KLIC User's Manual (klic.pdf, klicj.pdf) in the
"documents" directory.  To enable parallel execution, give the "-dp"
option to KLIC and use the "-p N" (N being the number of workers)
to the compiled binary (Sections 4.5.2, 4.5.3 of the Manual).

If you have installed KLIC with shared libraries enabled, you may need
to set the LD_LIBRARY_PATH environment variable to search for the
libraries at run time.

Implementation details of KLIC can be found in the (revived) document
"Inside KLIC" (in documents/inside).  Detailed changelog can be found
in ChangeLog.  Compiler developers may need to consult FOR-DEVELOPERS
or FOR-DEVELOPERS.j.
