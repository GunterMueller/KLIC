#
# @(#)$Header: /mm2/home/cvs/bc-src/tgif/Makefile.am,v 1.21 2011/01/18 05:17:27 william Exp $
#
AUTOMAKE_OPTIONS = foreign

TGIFDIR	= $(libdir)/X11/tgif

CFLAGS=-DVERSION=\"$(VERSION)\" -DPROG="\"$(PACKAGE)\"" \
	-Wall -I/usr/X11R6/include -DHAVE_CONFIG_H -DTGIF_PATH=\"$(TGIFDIR)\" -DPSFILE_MOD=0664 -DLOCALEDIR=\"/usr/share/locale\" -DTELEPORT_ATTR=\"warp_to=\" -DTMP_DIR=\"/tmp/\" -DLAUNCH_ATTR=\"launch=\" -DEXEC_ATTR=\"exec=\" -DEPSF_FILE_EXT=\"eps\" -DPS_FILE_EXT=\"ps\" -DXBM_FILE_EXT=\"xbm\" -DXPM_FILE_EXT=\"xpm\" -DOBJ_FILE_EXT=\"obj\" -DSYM_FILE_EXT=\"sym\" -DTEXT_FILE_EXT=\"txt\" -DPIN_FILE_EXT=\"pin\" -DNETLIST_FILE_EXT=\"net\" -DCOMP_FILE_EXT=\"cmp\"  -D_BACKGROUND_DONT_FORK -D_USE_XDRAWPOINT_TO_PUT_A_POINT -D_USE_PS_ADOBE_STRING=\"3.0/3.0\" -D_DONT_USE_MKTEMP -D_DONT_REENCODE=\"FFDingbests:ZapfDingbats\" -DDEFATTRGROUP=\"TANGRAM-II:Declaration:Events:Messages:Rewards:Initialization:Watches\" -D_NO_NKF -D_NO_CHINPUT -D_NO_XCIN -DUSE_XT_INITIALIZE -D_NO_LOCALE_SUPPORT


bin_PROGRAMS	= tgif

tgif_SOURCES	= \
	align.c animate.c arc.c attr.c auxtext.c \
	box.c button.c \
	chat.c chinput.c choice.c choose.c cli_xcin.c \
		convkinput.c convxim.c cmd.c color.c cutpaste.c \
		cursor.c \
	dialog.c drawing.c dup.c \
	edit.c eps.c exec.c expr.c \
	file.c font.c ftp.c \
	grid.c group.c \
	hash.c help.c http.c \
	imgproc.c import.c ini.c inmethod.c \
	list.c \
	mainloop.c mainmenu.c mark.c markup.c menu.c \
		menuinfo.c miniline.c move.c msg.c \
	names.c navigate.c nkf.c \
	obj.c oval.c \
	page.c pattern.c pin.c pngtrans.c poly.c polygon.c ps.c \
	raster.c rcbox.c rect.c remote.c rm_intrf.c ruler.c \
	scroll.c select.c setup.c shape.c shortcut.c \
		special.c spline.c stk.c stream.c stretch.c \
		strtbl.c \
	tangram2.c tdgtbase.c tdgtbmpl.c tdgtbrow.c tdgtbtn.c tdgtdraw.c \
		tdgtlist.c tdgtmsg.c tdgtsedt.c tgif_dbg.c tgcwheel.c \
		tgcwdl.c tginssym.c tgisdl.c tgtwb5.c tgif.c tgtwb5dl.c \
		tgtwb5xl.c tidget.c tcp.c text.c \
	util.c \
	version.c vms_comp.c \
	wb.c wb1.c wb2.c wb3.c wb_buff.c wb_mcast.c \
		wb_seg.c \
	xbitmap.c xpixmap.c xprtfltr.c \
	z_intrf.c

tgif_LDADD = -L/usr/X11R6/lib -lXext -lX11 -lXt -lSM -lICE -lXmu -lm -lz

tgifdir = $(TGIFDIR)
tgif_DATA = tgif.Xdefaults tgificon.eps tgificon.obj tgificon.xbm tgificon.xpm tgif.gif tangram.sym eq4.sym eq4-2x.sym eq4-ps2epsi.sym eq4-epstool.sym eq4xpm.sym eq4-lyx-ps2epsi.sym eq4xpm-land.sym eq4png.sym eq4jpg.sym eq4-2x-jpg.sym eq4-2x-nowin-jpg.sym keys.obj Copyright License
