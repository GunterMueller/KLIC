========================================================
(C)1993, 1994, 1995 Institute for New Generation Computer Technology
($BG[I[$=$NB>$O(B COPYRIGHT $B%U%!%$%k$r;2>H$7$F2<$5$$(B.)
(C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
($BG[I[$=$NB>$O(B COPYRIGHT-JIPDEC $B%U%!%$%k$r;2>H$7$F$/$@$5$$(B)
========================================================

KL1 $B$+$i(B C $B$X$N%3%s%Q%$%i%7%9%F%`(B: KLIC (3.002$BHG(B)
====================================================
1996$BG/(B9$B7n(B17$BF|2~D{(B
$BEl5~Bg3X(B                            $B6a;3(B $BN4(B
($B3t(B) $B;0I)Am9g8&5f=j(B                 $BF#@%(B $BE/O/(B
                                    $B4XED(B $BBg8c(B

[1] KLIC $B$H$O(B

KLIC $B$OJB9TO@M}7?8@8l(B KL1 $B$N=hM}7O$G$9(B.  KL1 $B$O(B Guarded Horn Clauses
(GHC) $B$N%U%i%C%HHG$K4p$E$$$?8@8l$G$9(B.  GHC $B$O$$$o$f$k(B committed-choice 
$B7?$NJB9TO@M}7?8@8lB2$N0l0w$G(B, Concurrent Prolog, Parlog, Janus, Fleng 
$B$J$I$N7;Do$K$"$?$j$^$9(B.

KLIC $B=hM}7O$O(B, KL1 $B%W%m%0%i%`$r(B C $B%W%m%0%i%`$K%3%s%Q%$%k$7(B, $B$=$l$r%[%9(B
$B%H%7%9%F%`$N(B C $B%3%s%Q%$%i$G%3%s%Q%$%k$7(B, $B<B9T;~%i%$%V%i%j(B (UNIX $B$,Ds6!(B
$B$9$k$b$N$r4^$`(B) $B$H%j%s%/$7$F<B9T$9$k(B, $B$H$$$&J}<0$r$H$C$F$$$^$9(B.

$B:#2s$NG[I[$G$OFs<oN`$NJBNs<BAu$,IU$12C$o$j$^$7$?(B.  $B$R$H$D$OJ,;6%a%b%j<B(B
$BAu$H8F$P$l(B, $B8=%P!<%8%g%s$G$O(B PVM(Parallel Virtual Machine) $B$H$$$&%W%m(B
$B%;%94VDL?.%i%$%V%i%j$rI,MW$H$7$F$$$^$9(B.  $B$b$&0lJ}$O(B, SparcCenter, DEC
Alpha $B>e$G$N6&M-%a%b%j<BAu$G$9!#(B


	KLIC --+-- $BC`<!<BAu(B
	       |
	       +-- $BJBNs<BAu(B ---+--- $BJ,;6%a%b%j(B
	                       |
	                       +--- $B6&M-%a%b%j(B

$BCm0U(B:

$B8=:_!"6&M-%a%b%j<BAu$G$$$/$D$+$N?<9o$J%P%0$,$_$D$+$C$F$*$j!">-Mh$NHG$G(B
$B=$@5$5$l$^$9!#(B2.00x$BHG$N6&M-%a%b%j<BAu$r8=:_MxMQ$5$l$F$$$k>l9g$K$O!"$3(B
$B$NHG$HCV$-$+$($k$3$H$O$d$a$F$/$@$5$$!#(B

[2] $B<B9T4D6-(B

KL1 $B%W%m%0%i%`$r(B C $B%W%m%0%i%`$KK]Lu$9$k%3%s%Q%$%i$O(B KL1 $B<+?H$G=q$$$F$"(B
$B$j$^$9(B.  $BG[I[$K$O(B C $B$K%3%s%Q%$%k:Q$_$N%3%s%Q%$%i$H(B KL1 $B%=!<%9$NN>J}$,(B
$BF~$C$F$$$^$9(B.

KLIC $B%3%s%Q%$%i$G%3%s%Q%$%k$7$?(B KL1 $B%W%m%0%i%`$O(B, $B$5$i$K(B C $B%3%s%Q%$%i(B
$B$G%3%s%Q%$%k$7(B, KLIC $B<B9T;~%i%$%V%i%j$*$h$S(B UNIX $B$,Ds6!$9$k%i%$%V%i%j(B
$B$H%j%s%/$9$kI,MW$,$"$j$^$9(B.

KLIC $B%7%9%F%`$OB?$/$N(B UNIX $B%7%9%F%`$K0\?"$G$-$k$h$&$K$7$?$D$b$j$G$9$,(B,
$B8=HG$K$O<c43$N%7%9%F%`0MB8@-$,;D$C$F$$$k$h$&$G$9(B.

$B8=HG$O0J2<$N(B UNIX $B%7%9%F%`$G$OF0:n$r3NG'$7$F$$$^$9(B.

$BC`<!<BAu!'(B

	IBM PC/AT compatible running Linux 2.0.x
		with gcc-2.7.x
	IBM PC/AT compatible running FreeBSD 2.2.1-RELEASE
		with gcc-2.7.2
	IBM PC/AT compatible running Solaris 2.5.x
		with Sun SPARCompiler
	Sun SparcStation 10 running SunOS 4.1.3
		with gcc-2.7.x and cc
	Sun SparcStation 20 running SunOS 4.1.4
		with gcc-2.7.2
	Sun SparcStation 20 running SunOS 5.4
		with gcc-2.7.2 and SPARCompiler
	SparcCenter 2000 running SunOS 5.3 (Solaris 2.3)
		with gcc-2.7 and SunC 2.0.1
	Ultra Enterprise 3000 running SunOS 5.5.1
		with gcc-2.7.2.2
	SGI Indigo2 IMPACT running IRIX 5.3 *
		with cc
	IBM RS6000 running AIX *
		with cc

	$B$=$NB>(B

*: $B8=:_$NHG$G$OL$3NG'$G$9$,!"2a5n$NHG$GF0:n$7$F$$$k$3$H$O3NG'$5$l$F$*(B
$B$j!"$^$:LdBj$J$/F0:n$9$k$H9M$($i$l$^$9!#(B

$BJBNs<BAu!'(B

NOTE: $B:#2s$NG[I[$G$O!"CWL?E*$J%P%0$,6&M-%a%b%jHG$G$OH/8+$5$l$F$$$^$9!#(B
2.x$BHG$N6&M-%a%b%j<BAu$rMxMQ$5$l$F$$$k>l9g$K$OCV$-$+$($k$3$H$OHr$1$F$/(B
$B$@$5$$!#(B

$B!&J,;6%a%b%j<BAu$K$O!"$5$i$K0J2<$N$h$&$J%P%j%(!<%7%g%s$,$"$j$^$9!#(B

	a) "pvm" $BHG(B: $B$3$NHG$O0JA0$+$i$NJ,;6%P!<%8%g%s$H8_49$N$b$N$G$9!#(B
	$BA4$F$NDL?.$O(BPVM$B$r7PM3$7$F9T$J$o$l$^$9!#(B

	b) "pvm-tcp" $BHG(B: $B$3$NHG$G$ODL?.$O(BTCP-IP$B$N(Bsocket$B$r7PM3$7$F9T$J(B
	$B$o$l$^$9!#%7%9%F%`$N=i4|2=$N$?$a$K(BPVM$B$rMxMQ$7$F$$$k$?$a!"0JA0(B
	$B$H$7$F(BPVM$B$,I,MW$G$9!#(B

	c) "shm" $BHG(B: $B$3$NHG$G$OA4$F$N%W%m%;%C%54VDL?.$O6&M-%a%b%j$rMQ(B
	$B$$$F9T$J$o$l$^$9!#$?$@$7!"$3$NHG$O!V6&M-%a%b%j!WHG$H$OJL$NHG$G(B
	$B$"$j!"(BKL1$B%G!<%?$r$=$N$^$^6&M-%a%b%j$KCV$-DL?.$r$9$k$N$G$O$"$j(B
	$B$^$;$s!#$h$C$F!"??$N!V6&M-%a%b%jHG!W$h$j$b8zN($OMn$A$^$9!#$3$N(B
	$BHG$K$O(BPVM$B$OI,MW$"$j$^$;$s!#(B

$B8zN(E*$K$O(Bshm$BHG$,:G$bNI$$$H9M$($i$l!"(Bpvm$BHG$,$b$C$H$b0-$$$H9M$($i$l$^$9!#(B
$B0lJ}0\?"@-$K4X$7$F$O(Bshm$B$,0-$/!"0J2<$N%W%i%C%H%U%)!<%`$G$7$+F0:n3NG'$5(B
$B$l$F$*$i$:!"B>$N7W;;5!(B($BFC$K%W%m%;%C%5$N0[$J$k7W;;5!(B)$B$X$N0\?"$O<j4V$,$+(B
$B$+$k$b$N$H;W$o$l$^$9!#(B

	SparcCenter 2000 running SunOS 5.3 (Solaris 2.3)
		with gcc-2.7.0
	Ultra-Enterprise running SunOS 5.5.1 (Solaris 2.5.1)
		with gcc-2.7.2.1
	DEC 7000 AXP running OSF1
		with gcc-2.7
	IBM PC/AT compatible multi processor machine running Linux 2.0.x
		with gcc-2.7

"pvm"$BHG$O0\?"@-$ONI$$$H9M$($i$l$k$N$G!"$b$7$b!"$3$3$K5s$2$i$l$?$h$&$J(B
$B%W%i%C%H%U%)!<%`0J30$G<B9T$r$3$3$m$_$k$J$i$P!":G=i$K(B"pvm"$BHG$r;n$_$F$/(B
$B$@$5$$!#(B

$B$b$7>e5-$N%7%9%F%`0J30$G$&$^$/%$%s%9%H!<%k$G$-$?$i(B, $B$"$k$$$O%$%s%9%H!<(B
$B%k$,$&$^$/$$$+$J$+$C$?$i(B, $B0J2<$N%"%I%l%9$K8fO"Mm$$$?$@$1$l$P9,$$$G$9!#(B

	klic@icot.or.jp

[3] $B%I%-%e%a%s%H(B

$B%7%9%F%`$N(B texinfo $B7A<0$NF|K\8l$H1Q8l$N4JC1$J%^%K%e%"%k$,G[I[$K4^$^$l(B
$B$F$$$^$9(B.  $B$?$@$7F|K\8l$N%^%K%e%"%k$O(B klic-1.600 $BHG$KBP1~$7$?$b$N$G(B, 
$B:G?7HG$G$O$J$$$3$H$K$4Cm0U2<$5$$(B.  $B%^%K%e%"%k$N%=!<%9$H%U%)!<%^%C%H:Q(B
$B$_$N(B info $B$H(B dvi $B$N7A<0$N%U%!%$%k$,%G%#%l%/%H%j(B "documents" $B$N2<$K$"$j(B
$B$^$9(B.

[4] $B%U%!%$%k(B

$B%U%!%$%k$K$D$$$F$O(B MANIFEST $B$H8fMw2<$5$$(B.

[5] $B%$%s%9%H!<%k(B

$B<B9T;~%7%9%F%`$N%$%s%9%H!<%k$K$O(B KLIC $B%7%9%F%`$N%k!<%H$K$"$k(B Configure 
$B%9%/%j%W%H$rAv$i$;(B, $B<!$$$G(B "make" $B$rAv$i$;$k$@$1$GNI$$$O$:$G$9(B.  $B>\:Y(B
$B$K$D$$$F$O(B INSTALL $B$r8fMw2<$5$$(B.

[6] $BDs6!5!G=(B

KLIC $B$G$O(B KL1 $B%W%m%0%i%`$N4pK\E*$J<B9T$,$G$-$^$9(B.

$B4pK\ItJ,0J30$K8=:_Ds6!$7$F$$$k5!G=$O35N,0J2<$NDL$j$G$9(B.

  - $B<c43$NAH9~=R8l(B ($B@0?t(B, $BIbF0>.?tE@?t$K$D$$$F$N;;=Q1i;;$J$I(B)
  - $BJ8;zNs(B, $BG[Ns$J$I$N%G!<%?9=B$$H4pK\A`:n(B
  - $B%H%l!<%5(B ($B%9%F%C%W<B9T(B, $B%9%Q%$5!G=$J$I(B)
  - $B%=%1%C%H$NHsF14|F~=PNO$r4^$`F~=PNO(B ($B%[%9%H$K$h$C$F$OL$Ds6!(B)
  - $BM%@hEY@)8f5!G=(B
  - $BJBNs<B9T5!G=(B

[7] $B%P%0$K$D$$$F(B

  - $BF|K\8l%^%K%e%"%k$O(B klic-2.0 $BHG$KBP1~$7$F$^$;$s(B.

  - $B%^%K%e%"%k$O$^$@IT40A4$G$9(B.

  - $BJBNsHG$O$^$@IT40A4$J$b$N$G$9(B.

  - $BEvA3I,MW$J5!G=$GL$Ds6!$N$b$N$,?tB?$/$"$j$^$9(B.

  - $BB>$K$bB??t$N%P%0$,$"$k$K0c$$$"$j$^$;$s(B.

$B%P%0Js9p$d%3%a%s%H$O2<5-$N%"%I%l%9$K$*Aw$j2<$5$$(B.

	klic-bugs@icot.or.jp

$B8+$D$1$?%P%0$dIT6q9g$O@'Hs$*CN$i$;2<$5$$(B.  $B$9$Y$F$N%f!<%6$N$?$a$N<!HG(B
$B0J9_$N2~NI$N$?$a$K$?$$$X$sBg;v$J>pJs$G$9(B.

[8] $BDLCN$H>pJs8r49(B

$B%P%0(B, $B%P%0=$@5(B, $B?7HG$NDs6!$J$I$NDLCN$O2<5-$N%a%$%j%s%0%j%9%H$rDL$8$F9T(B
$B$J$$$^$9(B.

	klic-users@icot.or.jp

$B$3$N%a%$%j%s%0%j%9%H$O%f!<%64V$N>pJs8r49$K$b;H$($^$9(B.

$B%a%$%j%s%0%j%9%H$X$NEPO?$dKu>C$N$?$a$K$O0J2<$N%"%I%l%9$K%a%$%k$r$*Aw$j(B
$B2<$5$$(B.

	klic-requests@icot.or.jp
