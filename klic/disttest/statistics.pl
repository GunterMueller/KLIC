package statistics;
# Copyright 1993, 1994, 1995 Kouichi Kumon. kumon@icot.or.jp
# perlで書いた統計用パッケージです。
# 1引数用のものと2引数用のものを用意しました。
# 呼び出す時には、まず元データを配列に入れ、その配列を参照として
# サブルーチンに引き渡します。
# 結果は配列で帰ってきますが、その値は以下に示すようなものになっています。
# もし、エラーが起こった時には結果としてundefが返る。
# この場合のチェック方法として、例えば次のようにする。
# ($ave,$ste)=&ave_std(*x);
# if (!defined($ave)) { error...; }
#
# 定義された関数は以下の通り。
# usage statistics'min_max(*x) = ($min, $max);
# usage statistics'ave_std(*x) = ($ave, $std);
# usage statistics'ave_std2(*x, *y)=
#                        ($ave_x, $ave_y, $std_x, $std_y, $correlation);
# 回帰直線は次の式になる。
# (y-ave_y)/std_y = (x-ave_x)/std_x * correlation;
# y = (x-ave_x)/std_x*std_y*correlation+ave_y;
# y = x/std_x*std_y*correlation + ave_y - ave_x/std_x*std_y*correlation;
# この回帰式と元データとの誤差の標準偏差は
# std_y * sqrt(1-correlation^2)で表される。
#
# この計算は良く行なうため、
#ライブラリを用意した。
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
