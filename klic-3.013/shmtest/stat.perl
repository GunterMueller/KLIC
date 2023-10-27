push(@INC, "/home/kumon/perl.src");
require 'statistics.pl';
while (<>) {
    if (/^\./) {
#	s/^\.\//\$(CMD)/;
#	s/-e -S -I10000/\$(RUNOPT)/;
	chop($file = $_);
	$on = 1;
    } elsif (/^Response time is (\d+) msec/ && $on) {
	$on = 0;
	$total{$file}.= "$1:";
    }
}
foreach (sort (keys %total)) {
    @a = split(':', $total{$_});
    ($ave, $std) = &statistics'ave_std(*a); #';
        ($min, $max) = &statistics'min_max(*a); #';
#    printf "%-40s%7d%s%5d %6.2f <%6.2f\n", $_, $ave, '+-', $std, $min/1000, $max/1000;
    printf "\t@echo Reference time is %d+-%d msec\n\t%s\n", $ave, $std, $_;
}
