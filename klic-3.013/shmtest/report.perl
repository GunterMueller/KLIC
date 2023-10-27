#! /usr/local/bin/perl
# This program summalize the output of test programs.
# If you want to summalize all test and measurement programs,
# do the following commands.
# make all >& log; perl report.perl log
#

printf("%-39s\t%6s\t%6s\t%-5s\n",
       "Program", "Ref", "Mesured", "Ratio");

while (<>) {
    if (/^Reference time is (\d+) / || /^(error)/) {
	$ref = $1;
	chop($cmd = <>);
	$cmd =~	s/\s*[\>\|\&].*//;
	$cmd =~ s/\s+/ /g;
    }
    if (/^Response time is (\d+) msec/) {
	$res = $1;
	if ($ref eq "error") {
	    $ref = " error";
	    $rat = "  ---";
	} else {
	    $ref = sprintf("%6d", $ref);
	    $rat = sprintf("%5.2f", $ref/$res);
	}
	$res = sprintf("%6d", $res);
	printf("%-39s\t%s\t%6d\t%s\n",
	       $cmd, $ref, $res, $rat);
    }
}
