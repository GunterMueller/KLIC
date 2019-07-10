		コンパイラ開発者のためへのメモ
					関田 大吾(三菱総合研究所)

KLICシステムでは、KL1言語で記述されたコンパイラを提供しており、このコ
ンパイラはKLICシステムによりコンパイル・リンクすることにより動作する。

よって、KLICのコンパイラを変更する際には若干の注意を要する。というのは、
KLICのコンパイラを変更した際に、そのコンパイラをコンパイルするコンパイ
ラはKLIC自身である。

このように、コンパイラに手を入れる時の注意を以下にまとめる。

・まず、普通にKLICシステムをinstallする。この際、かならずmake install
まで行う。

・make cleanをする。

・Compiler codeに手を入れる。

・引数 builddistを付加して再度Configureを行う。

	% ./Configure builddist

・その際、KLICコンパイラをinstallしたディレクトリを聞かれるので、
これを入力する(INSTALL時のKLIC Library directoryに相当する)。

・できたMakefileにより、makeを行う。

また、以下のようなmakeのターゲットが存在し、開発に有効である(少くとも
われわれには有効であった)。

・make clean: 普通のclean。実行可能ファイル、.o、 .aなどを消す。

・make distclean: 配布時に行うclean。./Configureにより自動生成されるファ
イル群を消去する。

・make realclean: まったく一からKLICシステムを作成する。.kl1から生成さ
れる.cファイルなどを消す。つまり、KLICコンパイラがないともうKLICを再度
installすることはできない。

・make veryfy: 現在の.cファイルと、compiler ディレクトリにあるcompiler
でコンパイルした.cファイルとを比較する。違いがある時には要注意。

							以上
