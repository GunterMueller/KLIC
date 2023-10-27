/*
 *    hzfont.h    by Dr. Susie W. chu,  March 18,1991
 *
 * @(#)$Header: /mm2/home/cvs/bc-src/tgif/big5.h,v 1.1 2004/06/18 23:19:08 william Exp $
 */
#include <stdio.h>

#ifdef undef
typedef unsigned short ushort;
typedef unsigned char uchar;
#endif
/* The following  *.n are .15 means 16x15 fonts, .24 means 24x24 fonts
   .40 means 40x40 fonts, .48 means 48x48 fonts, .64 means 64x64 fonts
   For the screen display we use 16x15 font as default. (also in ROM).
   For the printout (laserjet,or ps) we use 24x24 font for m,l and k style.
   For the printout (laserjet,or ps) we use 48x48 font for s style.
   Big 5 code is a double byte encoding chinese scheme.
   The low byte ranges are 40-7e ( 63 of them) and a1-fe (94 of them).
    Font name       range                # of chars
    ---------       -----                ----------
   spcfont.n      Special symbol 
		  a140 - a3bf              408
   stdfont.n     (a)Frequently used
		  a440 - c67e              5401
			   <--- gap
		 (b)Infrequently used
		  c940 - f9d5              7652
		 (c)ETen words
		  f9d6 - f9fe                41
					 --------
				 total 13,094
    spcfsupp.n    Suppliment  
                   c6a1 - c8d3              365 
    ascfont.n     AscII half width
		   00 - ff                  256
*/
#define NSPC    408
#define NSTD1  5401
#define NSTD2  7652
#define NSTD3    41
#define NSUPP   365
#define NASCII  256
#define HISPCS   0xa1
#define HISPCE   0xa3
#define HISTD1S  0xa4
#define HISTD1E  0xc6
#define HISTD23S 0xc9
#define HISTD23E 0xf9
#define HISUPPS  0xc6
#define HISUPPE  0xc8
#define NLOWB1  63
#define NLOWB2  94
#define NLOWB  157
#define LO1S  0x40
#define LO1E  0x7e
#define LO2S  0xa1
#define LO2E  0xfe
#define LOSPCE  0xbf
#define LOSTD1E  0x7e
#define LOSTD23E  0xfe
#define LOSUPPS  0xa1
#define LOSUPPE  0xd3
/* high order byte */
#define HB(code) ((code)>>8)
/* low order byte */
#define LB(code) ((code)&0xff)
