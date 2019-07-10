#!/usr/local/share/bin/wish -f
# $Id: klic-rmonitor.tcl,v 1.1.1.1 2000/07/28 02:01:40 igarashi Exp $
# C)1995 Institute for New Generation Computer Technology 
# (Read COPYRIGHT for detailed information.) 
#(C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
# (Read COPYRIGHT-JIPDEC for detailed information.)
#

set defintr 500
set penum -1

if { $argc != 0 } {
    if [regexp {^[0-9]+$} $argv] {
	set penum $argv
    }
}

set in file0

set line -1

# If the number of processor is unknown, 
# read input data set and count the lines
# This routeing depends on the output of "Sun Solaris 2.x's mpstat"
# command.

if { $penum < 0 } {
    if { [ gets $in line ] < 0 } {
	puts "unexpected EOF"
	exit 1
    }
    if { [ regexp {^CPU} $line ] == 1 } {
	set retval [ gets $in line ]
    }
    set penum 0
    while { $retval > 0 } {
	if { [ regexp {^CPU} $line ] == 1 } {
	    break
	}
	incr penum
	set retval [ gets $in line ]
    }
}

#puts "penum = $penum"

#button .quit -text quit -command "destroy ."
#pack .quit -side right

#puts "Button initiated"

# Preparatation of drawing.

# width of one unit.
set x 18
# height of one unit.
set y 18
# horizontal distance between units
set xdist [expr $x + 2]
# vertinal distance between units
set ydist [expr $y]
set xbase 0
set ybase 0
# initial value of the view point for auto scrolling.
set xview 0

# whole height
set cheight [expr $x * $penum ]

set cwidth [expr $cheight * 2]


canvas .c -height $cheight -width $cwidth

pack .c -side top -fill x
.c create line 0 0 1 1

proc setPeColor { pe num } {
    set color [ selColor $num ]
    $pe configure -bg $color
}

# calcurate color from idling rate

proc selColor { num } {
	set r [ expr (100 - $num) * 255 / 100 ]
	set g 0
	set b [ expr $num * 255 / 100 ]
	set color [ format \#%02x%02x%02x $r $g $b ]
	return $color
}

proc calc {} {
    global in
    global penum
    global x y xbase ybase xdist ydist
    global line
# if this is a first-time data read, read one line.
    if { $line == -1 } {
	if { [ gets $in line ] < 0 } {
	    puts "unexpected EOF"
	    exit 1
	}
    }
# Anyway, if $line == "CPU ..." reread one more line
    if { [ regexp {^CPU} $line ] == 1 } {
	if { [ gets $in line ] < 0 } {
	    puts "unexpected EOF"
	    exit 1
	}
    }
# debugging aids
#    puts "entry $line"
    while { [ regexp {^CPU} $line ] != 1 } {
	set l [ llength $line ]
	set ucpu [ lindex $line [ expr $l - 4 ]]
	set scpu [ lindex $line [ expr $l - 3 ]]
	set uidle [ lindex $line [ expr $l - 2 ]]
	set sidle [ lindex $line [ expr $l - 1 ]]
	set cpu [ expr $ucpu + $scpu ]
	set idle [ expr $uidle + $sidle ]
	set color [selColor [ expr $idle * 100 / ($cpu + $idle)]]
	set x1 [expr $xbase + $x]
	set y1 [expr $ybase + $y]
	.c create rectangle $xbase $ybase $x1 $y1 -fill "$color"
	set ybase [expr $ybase + $ydist]
	set rend [expr $xbase + $x + 2]
	global cwidth
	set lend [expr $rend - $cwidth]
	if {$lend > 0} {
	    .c xview [expr $lend / 10]
	}
	if { [ gets $in line ] < 0} {
	    puts "unexpected EOF"
	    exit 1
	}
    }
    set ybase 0
    set xbase [expr $xbase + $xdist]
    global defintr
    after $defintr calc
}

#bind . <Control-c> {destroy .}

proc setXview {ind} {
    global xview
    set xview $ind
}

proc scrollview {ind} {
    global cwidth
    global xbase
    global x
    set rend [expr $xbase + $x + 2]
    set lend [expr $rend - $cwidth]
    set pind [expr $lend / 10]
    if { $pind > $ind } {
	.c xview $pind
    }
}

after $defintr calc


