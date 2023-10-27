package statistics;
# Copyright 1993, 1994, 1995 Kouichi Kumon. kumon@icot.or.jp
# perl$B$G=q$$$?E}7WMQ%Q%C%1!<%8$G$9!#(B
# 1$B0z?tMQ$N$b$N$H(B2$B0z?tMQ$N$b$N$rMQ0U$7$^$7$?!#(B
# $B8F$S=P$9;~$K$O!"$^$:85%G!<%?$rG[Ns$KF~$l!"$=$NG[Ns$r;2>H$H$7$F(B
# $B%5%V%k!<%A%s$K0z$-EO$7$^$9!#(B
# $B7k2L$OG[Ns$G5"$C$F$-$^$9$,!"$=$NCM$O0J2<$K<($9$h$&$J$b$N$K$J$C$F$$$^$9!#(B
# $B$b$7!"%(%i!<$,5/$3$C$?;~$K$O7k2L$H$7$F(Bundef$B$,JV$k!#(B
# $B$3$N>l9g$N%A%'%C%/J}K!$H$7$F!"Nc$($P<!$N$h$&$K$9$k!#(B
# ($ave,$ste)=&ave_std(*x);
# if (!defined($ave)) { error...; }
#
# $BDj5A$5$l$?4X?t$O0J2<$NDL$j!#(B
# usage statistics'min_max(*x) = ($min, $max);
# usage statistics'ave_std(*x) = ($ave, $std);
# usage statistics'ave_std2(*x, *y)=
#                        ($ave_x, $ave_y, $std_x, $std_y, $correlation);
# $B2s5"D>@~$O<!$N<0$K$J$k!#(B
# (y-ave_y)/std_y = (x-ave_x)/std_x * correlation;
# y = (x-ave_x)/std_x*std_y*correlation+ave_y;
# y = x/std_x*std_y*correlation + ave_y - ave_x/std_x*std_y*correlation;
# $B$3$N2s5"<0$H85%G!<%?$H$N8m:9$NI8=`JP:9$O(B
# std_y * sqrt(1-correlation^2)$B$GI=$5$l$k!#(B
#
# $B$3$N7W;;$ONI$/9T$J$&$?$a!"(B
#$B%i%$%V%i%j$rMQ0U$7$?!#(B
# usage statistics'regression(statistics'ave_std2(*x, *y))=
#			(x_coef, y_const, y_err)
sub regression {
    ($ave_x, $ave_y, $std_x, $std_y, $correlation) = @_;
    local($x_coef, $y_const, $y_err);
    $x_coef = $std_y/$std_x*$correlation;
    $y_const= $ave_y - $ave_x/$std_x*$std_y*$correlation;
    $y_err  = $std_y * sqrt(1-$correlation*$correlation);
    return ($x_coef, $y_const, $y_err);
}
    

sub ave_std2 {
    local(*x, *y)=@_;
    local($sumx, $sumy, $sumxy, $sumx2, $sumy2);
    local($avex, $avey, $avexy, $avex2, $avey2);
    local($i);
    if (@x != @y || @x == 0) {
	warn "x(".scalar(@x).") and y(".scalar(@y).
	    ") have different number of items or no item\n";
	return undef;
    }
    foreach (@x) { $sumx += $_; }
    foreach (@y) { $sumy += $_; }
    $avex = $sumx/@x;
    $avey = $sumy/@y;
    for ($i = 0; $i < @x; $i++) {
	$sumx2 += ($x[$i]-$avex)*($x[$i]-$avex);
	$sumy2 += ($y[$i]-$avey)*($y[$i]-$avey);
	$sumxy += ($x[$i]-$avex)*($y[$i]-$avey);
    }
    return ($avex, $avey, sqrt($sumx2/@x), sqrt($sumy2/@y),
	    $sumxy/sqrt($sumx2*$sumy2));
}
sub ave_std {
    local(*x)=@_;
    local($sum, $sum2, $ave, $std);
    if (@x < 1) {
	warn "number of elements too few(".scalar(@x).")\n";
	return undef;
    }
    foreach (@x) {
	$sum += $_;
    }
    $ave = $sum/@x;
    foreach (@x) {
	$sum2 += ($_-$ave)*($_-$ave);
    }
    $std = sqrt($sum2/@x);
    return ($ave, $std);
}
sub min_max {
    local(*x)=@_;
    return undef if (@x == 0);
    local($min, $max)=($x[0], $x[0]);
    foreach (@x) {
	$min = $_ if ($min > $_);
	$max = $_ if ($max < $_);
    }
    return ($min, $max);
}
1;
