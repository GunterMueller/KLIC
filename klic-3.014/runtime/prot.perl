#!/usr/local/bin/perl
# ---------------------------------------------------------- 
#   (C)1995 Institute for New Generation Computer Technology 
#       (Read COPYRIGHT for detailed information.) 
#-----------------------------------------------------------
while (<>) {
    if (/^\w.*\(.*\)\s*\{?\s*$/) {
	$_ = join('', @buf, $_);
	print;
	@buf = ();
    } else {
	chop;
	/^\w/ && push(@buf, "$_ ");
    }
    if (/;/) {
	@buf=();
    }
}
