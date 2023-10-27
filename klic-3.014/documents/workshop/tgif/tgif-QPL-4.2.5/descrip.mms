#
# @(#)$Header: /mm2/home/cvs/bc-src/tgif/descrip.mms,v 1.1 2004/06/18 23:20:49 william Exp $
#
# VMS MAKEFILE (DESCRIP.MSS)
# 25-MAR-91 GJC@MITECH.COM
#
# Before running this define these logical names:
# $ DEFINE X11 DECW$INCLUDE
# $ DEFINE SYS SYS$LIBRARY
#
# 24-NOV-97 Rolf Niepraschk, niepraschk@ptb.de
#

O = .O
E = .EXE
XBM = [.XBM]

.SUFFIXES :   ! clear the suffix list first
.SUFFIXES : $(X) $(O) .C
.C$(O) :
	$(CC) /OBJ=$@ $(CFLAGS) $<

TGIFEXE = TGIF$(E)

TGIFLIB = TGIF.OLB

MISCLIB =,DESCRIP.OPT/OPT

# CFLAGS = $(DEFINES)/debug/nooptimize/list/show=(nosource)/object=$(mms$target_name).obj
CFLAGS = /prefix=(ALL,EXCEPT=(delete,select))\
/WARN=(DISABLE=(LONGEXTERN,EMPTYFILE,IMPLICITFUNC))$(DEFINES)

# LFLAGS = /DEBUG/MAP

DEFINES = /DEFINE=(TGIF_PATH="""""", \
	VMS=1, PSFILE_MOD="0664",\
	EPSF_FILE_EXT="""eps""", PS_FILE_EXT="""ps""", \
	XBM_FILE_EXT="""xbm""", XPM_FILE_EXT="""xpm""", \
	OBJ_FILE_EXT="""obj""", SYM_FILE_EXT="""sym""", \
	TEXT_FILE_EXT="""txt""", PIN_FILE_EXT="""pin""", \
	TELEPORT_ATTR="""warp_to=""", LAUNCH_ATTR="""launch=""", \
	EXEC_ATTR="""exec=""", \
	TMP_DIR="""/sys$scratch/"""
        $(D10)$(D11)$(D12)$(D13)$(D14)$(D15)$(D16)$(D17)$(D18)$(D19)\
        $(D20)$(D21)$(D22)$(D23)$(D24)$(D25)$(D26)$(D27)$(D28)$(D29)\
        $(D30)$(D31)$(D32)$(D33)$(D34)$(D35)$(D36)$(D37)$(D38)$(D39)\
        $(D40)$(D41)$(D42)$(D43)$(D44)$(D45)$(D46)$(D47)$(D48)$(D49))

D10=,"dumpOneFilePerPage=_dumpOneFilePerPage"
D11=,"execNavigateBack=_execNavigateBack"
D12=,"navigateRefresh=_navigateRefresh"
D13=,F_SETFL=0,F_GETFL=1 # ???
D14=,"unlink=delete"
#
# D42=,METRIC_PIX_PER_INCH=1
#   Include the above line if you want to use 127 pixels per inch
#   when printing in the metric system.  If this line is not
#   included, 128 pixels per inch is used.
D41=,_NO_GETTIMEOFDAY
#   Include the above line if you do not have the gettimeofday()
#   function but have the ftime() function.
# D40=,BSD_COMP
#   Include the above line if you are compiling on a Solaris (or
#   possibly an SGI) machine which requires it in <sys/ioctl.h>.
#   (If you see the error that FIONBIO is not defined when compiling
#   tcp.c, then you probably need this.)
# D39=,_HANDLE_SIGCHLD
#   Include the above line if there are defunct tgif child processes.
# D38=,_NO_LSTAT
#   Include the above line if lstat() is not available.  In this case,
#   using BrowseXBitmap and BrowseXPixmap under the File Menu may cause
#   infinite loop if the directory structure is infinitely recursive.
# D37=,_USE_XDRAWPOINT_TO_PUT_A_POINT
#   Include the above line if the grid points are not visible.  This can
#   be used to bypass some X server bugs (such as certain Linux X server
#   using VGA16 screen) where the XDrawLine() Xlib call does not work
#   properly.
# D36=,_NO_EXTERN (obsoleted)
#   Include the above line in the DEFINES if you don't want to include
#   the extern definition for functions defined in a module.  For example,
#   if this option is set, then file "foo.c" will not include "foo.e",
#   and certain other extern definition in "foo.c" will be omitted.
# D35=,_NO_PROTO
#   Include the above line in the DEFINES if checking for function
#   prototypes is not desired.  Checking for function prototypes
#   are automatically enabled if you are using an ANSI or C++ compilor.
#   However, not all ANSI/C++ compilors are created equal.
# D34=,NO_STRSTR
#   Include the above line in the DEFINES if you do not have the strstr()
#   function in your system's string library.
# D33=,INVERT_CTM_BUG
#   Include the above line in the DEFINES if you are using PostScript
#   utilities which fails when trying to invert non-invertible CTM
#   (e.g., muluti.ps from ftp.ucc.su.oz.au:/pub/ps_printing/multi).
# D32=,HP_LINE_BUG
#   Include the above line in the DEFINES if two-vertices horizontal and
#   verticle line segments don't draw properly on a buggy HP server.
# D31=,DONTFREECOLORS
#   Include the above line in the DEFINES if you don't want the black and
#   the white colors to be freed when tgif initializes itself.  You should
#   not need to do this unless to get around some server bugs.
# D30=,USE_XAPPLRESDIR
#   Include the above line in the DEFINES if $XAPPLRESDIR/Tgif is used to
#   specify the resource file.  If your system does not have putenv()
#   (such as IBM-RT), then this won't work.  I'm not sure if VMS systems
#   have putenv(), so this might be useless for VMS systems.
# D29=,XAPPLOADDIR_DEF
#   If you define USE_XAPPLRESDIR, define this to a default directory
#   to use in case $XAPPLRESDIR isn't set.
# D28=,USE_XT_INITIALIZE
#   This will cause the XToolkit to be initialized so that the X defaults
#   are obtained from the Xt directories.  This should only be used when
#   USE_XAPPLRESDIR option is NOT used.
# D27=,NO_THIN_LINE
#   Include the above line in the DEFINES if a line width of 0 causes
#   your server to crash.  This is to get around a bug in some X servers.
# D26=,THIN_OVAL_AND_ARC
#   Include the above line in the DEFINES if you want 0 width arcs to
#   be used when the width of an arc is less than or equal to 1.
#   (Sun3 server has a bug that makes drawing ovals and arcs with
#   width other than 0 VERY slow).
# D25=,KEEP_WHEN_PRINT
#   Include the above line in the DEFINES if you don't want the temp
#   file to be deleted after sending the print job to the printer.
#   (Some VMS systems might require this flag to be used.)
# D24=,PRINT_CMD="""print"""
#   Include the above line in the DEFINES if you want to configure tgif
#   to use a default print command.
# D23=,NOTR4MODE
#   Include the above line in the DEFINES if you are running X11R3.
#   This will cause tgif NOT to use some X11R4 window manager related
#   functions that's not present in R3.
# D22=,MAPBEFORESELECT
#   Include the above line in the DEFINES to call XMapWindow() before
#   calling XSelectInput().  This might be needed to get around some
#   server bugs.
# D21=,DONT_FREE_COLORMAP="""true"""
#   Include the above line in the DEFINES if you don't want the colormap
#   to be freed when tgif exits.
D20=,A4PAPER
#   Include the above line in the DEFINES if you want to print on A4 paper
#   (8.25in by 11.7in or about 21cm by 29.7cm).

.FIRST
	@ DEFINE XMB [.XBM]
	@ DEFINE X11 DECW$INCLUDE

ALL : $(TGIFEXE) $(TGIFLIB)
	@ !

INCLUDE make.common

$(TGIFEXE) : $(TGIFLIB)
        LINK/EXE=$@ $</LIB/INCLUDE=TGIF $(MISCLIB)

$(TGIFLIB) : $(OBJS)
        IF F$SEARCH("$@") .EQS. "" THEN LIBRARY/CREATE $@
        LIBRARY/obj $@ $+

CLEAN :
        IF F$SEARCH("*$(O)",).NES."" THEN DEL *$(O);*
        IF F$SEARCH("$(TGIFLIB)",).NES."" THEN DEL $(TGIFLIB);*

VERYCLEAN : CLEAN
        IF F$SEARCH("$(TGIFEXE)",).NES."" THEN DEL $(TGIFEXE);*

INCLUDE make.comdep

