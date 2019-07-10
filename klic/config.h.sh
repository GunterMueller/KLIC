# ---------------------------------------------------------- 
#   (C) 1995 Institute for New Generation Computer Technology 
#	(Read COPYRIGHT for detailed information.) 
#   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
#       (配布その他は COPYRIGHT-JIPDEC ファイルを参照してください)
# ----------------------------------------------------------

cat <<GAZONK >>config.h

#define DIST_COMPILER_FLAG(name, ext) \
    {(void)sprintf(bufp, " -DDIST -I%s -I. %s%s", \
    klic_incdir, (name), (ext)); }
#define DIST_LINKAGE_FLAG() 

GAZONK
echo " "
