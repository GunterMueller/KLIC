##---------------------------------------------------------- 
##   (C)1993,1994,1995 Institute for New Generation Computer Technology 
##       (Read COPYRIGHT for detailed information.) 
##----------------------------------------------------------
@mon=(Jan, Feb, Mar, Apr, Jun, Jul, Aug, Sep, Oct, Nov, Dec);
while (<DATA>) {
    if (/^RCS file: (\S+),v/) {
	$file = $1;
    } elsif (m|^date: (\d+)/(\d+)/(\d+) (\d{2}):(\d{2}):(\d{2});|) {
	$date="$1/$2/$3 $4:$5:$6";
	$on++;
	@a = ($1,$2,$3,$4,$5,$6);
	{
	    local($y,$m,$d,$h,$min,$s)=@a;
	    if (($h += 9) >= 24) {
		$h -= 24;
		$d++;
	    }
	    $m = $2; $d = $3;
	    $y += 1900 if ($y < 100);
	    $m <= 2 && ($y --, $m += 12);
	    $week=('Sun','Mon','Tue','Wed','Thr','Fri','Sat')
	      [((0,3,5,1,3,6,2,4,0,2,5,1)[$m-3]+$d+$y+2) % 7];
	}
	($y,$m,$d,$h,$min,$s)=@a;
	$y += 1900 if ($y < 100);
	if (($h += 9) >= 24) {
	    $h -= 24;
	    $d++;
	}
	$head = "\n$week $mon[$m-1] $d $h:$min:$s $y  Kouichi Kumon  (kumon@icot.or.jp)\n";
	push(@line, $head);
    } elsif (/^-------------------/||/^=================/) {
	if ($on) {
	    $on = 0;
	    push(@buffer, join('', @line));
	    push(@date, $date);
	}
	@line = ();
    } elsif (/^revision \d+\.\d+/) {
    } else {
	chop;
	push(@line, "$_($file)\n");
    }
}
@key = sort {$date[$b] cmp $date[$a]} (0..$#date);
print @buffer[@key];
__END__
