========================================================
(C) 1995 Institute for New Generation Computer Technology
(配布その他は COPYRIGHT ファイルを参照して下さい.)
(C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
(配布その他は COPYRIGHT-JIPDEC ファイルを参照してください)
========================================================

		SparcCenter 上でのランタイムモニタの使い方。

(1)  PVM と PVMコンソールを立ちあげておく

(2)  このディレクトリで、

      % klic-rmonitor  '表示プロセッサ数'

     を実行する。

(3)  PVM コンソールで ps コマンドを実行することによりランタイムモニタ用
     サーバプロセスのタスクid（以下 rmon_tid と呼ぶ）を得る。

(4)  ユーザプログラムの実行時オプションとして、

      % a.out  -rmon rmon_tid ...

      を指定する。
