$!
$! @(#)$Header: /mm2/home/cvs/bc-src/tgif/VMS_MAKE_TGIF.COM,v 1.1 2004/06/18 23:20:49 william Exp $
$!
$!	VAX/VMS command file to compile and link TGIF.
$!
$!	NOTE:	This command file is only to be viewed as an EXAMPLE
$!		on how to build TGIF on a VAX/VMS system.
$!		THIS COMMAND FILE MAY NOT WORK, NOR DO WHAT
$!		YOU THINK IT DOES.
$!
$!	Created by Mats Bergstr"om, University of Lund.
$!
$!	The TGIF default extension for the object files is ".obj",
$!	since this collides with the default VMS ".OBJ" extension
$!	for compiled files, the name used for TGIF object files is
$!	changed to ".tgif".
$!	Feel free to change it at any time.
$!	Mind the quotes below though ;-)
$!
$!	Have a look at the document to change the defines BEFORE
$!	You try to run this...
$!
$!	After unpacking TGIF there are problably some files with
$!	extension ".OBJ". Use the extension ".o" for all compiled
$!	object files created to avoid confusion!
$!
$	define sys sys$library
$	define x11 decw$include
$!
$!
$	compile := cc/object=*.o
$!
$	defs := 	"(	""TGIF_DIR=""""TGIF$DIR"""""" ,"-
			"	""TGIF_PATH=""""TGIF$DIR"""""" ,"-
			"	""PSFILE_MOD=0664"","-
			"	""OBJ_FILE_EXT=""""tgif"""""" ,"-
			"	""SYM_FILE_EXT=""""sym"""""" ,"-
			"	""EPSF_FILE_EXT=""""eps"""""" ,"-
			"	""PS_FILE_EXT=""""ps"""""" ,"-
			"	""XBM_FILE_EXT=""""xbm"""""" ,"-
			"	""XPM_FILE_EXT=""""xpm"""""" ,"-
			"	""TEXT_FILE_EXT=""""txt"""""" ,"-
			"	""TELEPORT_ATTR=""""warp_to="""""" ,"-
			"	""LAUNCH_ATTR=""""launch="""""" ,"-
			"	""EXEC_ATTR=""""exec="""""" ,"-
			"	""TMP_DIR=""""SYS$SCRATCH:"""""" ,"-
			"	""KEEP_WHEN_PRINT"","-
			"	""PRINT_CMD=""""TGIF$PRINT"""""" ,"-
			"	""NOTR4MODE"","-
			"	""_NO_LSTAT"","-
			"	""_NO_GETTIMEOFDAY"","-
			"	""A4PAPER""			)"
$!
$!
$! Compile the stuff.
$	Write sys$output "Go have a cup of coffee, this will take a while..."	
$!
$	compile/defi='defs'	ALIGN
$	compile/defi='defs'	ANIMATE
$	compile/defi='defs'	ARC
$	compile/defi='defs'	ATTR
$	compile/defi='defs'	AUXTEXT
$	compile/defi='defs'	BOX
$	compile/defi='defs'	BUTTON
$	compile/defi='defs'	CHOICE
$	compile/defi='defs'	CHOOSE
$	compile/defi='defs'	CMD
$	compile/defi='defs'	COLOR
$	compile/defi='defs'	CUTPASTE
$	compile/defi='defs'	CURSOR
$	compile/defi='defs'	DIALOG
$	compile/defi='defs'	DRAWING
$	compile/defi='defs'	DUP
$	compile/defi='defs'	EDIT
$	compile/defi='defs'	EPS
$	compile/defi='defs'	EXEC
$	compile/defi='defs'	EXPR
$	compile/defi='defs'	FILE
$	compile/defi='defs'	FONT
$	compile/defi='defs'	FTP
$	compile/defi='defs'	GRID
$	compile/defi='defs'	GROUP
$	compile/defi='defs'	HELP
$	compile/defi='defs'	HTTP
$	compile/defi='defs'	IMGPROC
$	compile/defi='defs'	IMPORT
$	compile/defi='defs'	MAINLOOP
$	compile/defi='defs'	MAINMENU
$	compile/defi='defs'	MARK
$	compile/defi='defs'	MENU
$	compile/defi='defs'	MOVE
$	compile/defi='defs'	MSG
$	compile/defi='defs'	NAMES
$	compile/defi='defs'	NAVIGATE
$	compile/defi='defs'	OBJ
$	compile/defi='defs'	OVAL
$	compile/defi='defs'	PAGE
$	compile/defi='defs'	PATTERN
$	compile/defi='defs'	POLY
$	compile/defi='defs'	POLYGON
$	compile/defi='defs'	PS
$	compile/defi='defs'	RASTER
$	compile/defi='defs'	RCBOX
$	compile/defi='defs'	RECT
$	compile/defi='defs'	REMOTE
$	compile/defi='defs'	RULER
$	compile/defi='defs'	SCROLL
$	compile/defi='defs'	SELECT
$	compile/defi='defs'	SETUP
$	compile/defi='defs'	SHAPE
$	compile/defi='defs'	SHORTCUT
$	compile/defi='defs'	SPECIAL
$	compile/defi='defs'	SPLINE
$	compile/defi='defs'	STK
$	compile/defi='defs'	STRETCH
$	compile/defi='defs'	TGIF_DBG
$	compile/defi='defs'	TCP
$	compile/defi='defs'	TEXT
$	compile/defi='defs'	TGIF
$	compile/defi='defs'	UTIL
$	compile/defi='defs'	VERSION
$	compile/defi='defs'	VMS_COMPAT
$	compile/defi='defs'	WB1
$	compile/defi='defs'	WB2
$	compile/defi='defs'	WB3
$	compile/defi='defs'	XBITMAP
$	compile/defi='defs'	XPIXMAP
$!
$!
$! 
$!
$	delete/noconfirm tgif.olb;
$	library/create tgif -
ALIGN.o,ANIMATE.o,ARC.o,ATTR.o,AUXTEXT.o,BOX.o,BUTTON.o,CHOICE.o,-
CHOOSE.o,CMD.o,COLOR.o,CUTPASTE.o,CURSOR.o,DIALOG.o,DRAWING.o,DUP.o,-
EDIT.o,EPS.o,EXEC.o,EXPR.o,FILE.o,FONT.o,FTP.o,GRID.o,GROUP.o,HELP.o,HTTP.o,-
IMGPROC.o,IMPORT.o,MAINLOOP.o,MAINMENU.o,MARK.o,MENU.o,MOVE.o,MSG.o,-
NAMES.o,NAVIGATE.o,OBJ.o,OVAL.o,PAGE.o,PATTERN.o,POLY.o,POLYGON.o,-
PS.o,RASTER.o,RCBOX.o,RECT.o,REMOTE.o,RULER.o,SCROLL.o,SELECT.o,-
SETUP.o,SHAPE.o,SHORTCUT.o,SPECIAL.o,SPLINE.o,STK.o,STRETCH.o,TGIF_DBG.o,-
TCP.o,TEXT.o,UTIL.o,VMS_COMP.o,WB1.o,WB2.o,WB3.o,-
XBITMAP.o,XPIXMAP.o
$!
$	delete/noconfirm -
ALIGN.o;,ANIMATE.o;,ARC.o;,ATTR.o;,AUXTEXT.o;,BOX.o;,BUTTON.o;,CHOICE.o;,-
CHOOSE.o;,CMD.o;,COLOR.o;,CUTPASTE.o;,CURSOR.o;,DIALOG.o;,DRAWING.o;,DUP.o;,-
EDIT.o;,EPS.o;,EXEC.o;,EXPR.o;,FILE.o;,FONT.o;,FTP.o;,GRID.o;,GROUP.o;,-
HELP.o;,HTTP.o;,IMGPROC.o;,IMPORT.o;,MAINLOOP.o;,MAINMENU.o;,MARK.o;,-
MENU.o;,MOVE.o;,MSG.o;,NAMES.o;,NAVIGATE.o;,OBJ.o;,OVAL.o;,PAGE.o;,-
PATTERN.o;,POLY.o;,POLYGON.o;,PS.o;,RASTER.o;,RCBOX.o;,RECT.o;,REMOTE.o;,-
RULER.o;,SCROLL.o;,SELECT.o;,SETUP.o;,SHAPE.o;,SHORTCUT.o;,SPECIAL.o;,-
SPLINE.o;,STK.o;,STRETCH.o;,TGIF_DBG.o;,TCP.o;,TEXT.o;,UTIL.o;,VMS_COMP.o;,-
WB1.o;,WB2.o;,WB3.o;,XBITMAP.o;,XPIXMAP.o;
$!
$! Link the stuff. NOTE "version.o" MUST be linked explicitly!!!
$!
$huuj:
$	link tgif.o,version.o,tgif/lib,sys$input/opt
sys$share:decw$xlibshr/share
sys$share:vaxcrtl/share
$!
