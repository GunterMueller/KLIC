#
# Copyright 1995 Institute for New Generation Computer Technology
# Read COPYRIGHT for detailed information
# (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
# (Read COPYRIGHT-JIPDEC for detailed information.)
#
# Makefile for KLIC runtime monitor system

SHELL = /bin/sh

all: klic-rmonitor

klic-rmonitor:
	./make-com
	chmod +x klic-rmonitor

install: all
	- $(INSTDIR) $(KLICBIN)
	- $(INSTDIR) $(KLICLIB)/klic
	$(INSTALL) klic-rmonitor.tcl $(KLICLIB)/klic
	$(INSTALL) klic-rmonitor $(KLICBIN)

clean:
	/bin/rm -f klic-rmonitor  *~ core

distclean: clean

realclean: clean
