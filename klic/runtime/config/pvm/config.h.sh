# ---------------------------------------------------------- 
#   (C) 1995 Institute for New Generation Computer Technology 
#	(Read COPYRIGHT for detailed information.) 
#   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
#       (Read COPYRIGHT-JIPDEC for detailed information.)
# ----------------------------------------------------------

if test "$PVM_ARCH" = "VPP300" -o "$PVM_ARCH" = "VPP700"; then
DEFINEVPP="#define"
else
DEFINEVPP="#undef"
fi

if test "$PVM_ARCH" = "AP3000"; then
DEFINEAP="#define"
else
DEFINEAP="#undef"
fi

if ( test "$PVM_ARCH" = "VPP300" || test "$PVM_ARCH" = "VPP700" ); then
  PVMLIBDIR=$PVM_ROOT/lib/$PVM_ARCH
  DISTCOMPILERFLAG="{(void)sprintf(bufp, \" -DDIST -I%s -I. -I%s %s%s -Wl,-P -dy -J\", klic_incdir, PVMINC, (name), (ext)); }"
  DISTLINKAGEFLAG="{ (void)sprintf(bufp, \" -Wl,-P,-dy -J -Wv,-sc,-ad -Oe,-P -L%s \",PVMLIBDIR);     bufp += strlen(bufp);     (void)sprintf(bufp, \" -l%s /usr/lang/pvm/lib/VPP300/libmp.a -lgen -lelf -lsocket -lpx -lm -lc\",PVMLIB);     bufp += strlen(bufp); }"

elif test "$PVM_ARCH" = "AP3000"; then
  PVMLIBDIR=$PVM_ROOT/lib
  DISTCOMPILERFLAG="{(void)sprintf(bufp, \" -DDIST -I%s -I. -I%s %s%s\", klic_incdir, PVMINC, (name), (ext)); }"
  DISTLINKAGEFLAG="{(void)sprintf(bufp, \" -L%s\",PVMLIBDIR); bufp += strlen(bufp); (void)sprintf(bufp, \" -l%s -lmpl -lemi -lthread\",PVMLIB); bufp += strlen(bufp); }"

else
   PVMLIBDIR=$PVM_ROOT/lib/$PVM_ARCH
   DISTCOMPILERFLAG="{(void)sprintf(bufp, \" -DDIST -I%s -I. -I%s %s%s\", klic_incdir, PVMINC, (name), (ext)); }"
   DISTLINKAGEFLAG="{(void)sprintf(bufp, \" -L%s\",PVMLIBDIR); bufp += strlen(bufp); (void)sprintf(bufp, \" -l%s\",PVMLIB); bufp += strlen(bufp); }"
fi

cat <<GAZONK >>config.h

$DEFINEVPP VPP
$DEFINEAP AP3000

#define PVMINC  "$PVM_ROOT/include"
#define PVMLIBDIR  "$PVMLIBDIR"
#define PVMLIB  "$PVMLIB"

#define DIST_COMPILER_FLAG(name, ext) $DISTCOMPILERFLAG
#define DIST_LINKAGE_FLAG() $DISTLINKAGEFLAG

GAZONK
echo " "

if test $USE_REDUCTION_COUNT = yes;
then
echo "Disabling using interval timer ..."
cat <<GAZONK >>config.h

#undef USETIMER /* disabling use interval timer */

#define USE_REDUCTION_COUNT
#define REDUCTION_COUNTS $REDUCTION_COUNTS
GAZONK
echo " "
fi
