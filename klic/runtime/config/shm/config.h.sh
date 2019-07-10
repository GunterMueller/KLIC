# ---------------------------------------------------------- 
#   (C) 1995 Institute for New Generation Computer Technology 
#	(Read COPYRIGHT for detailed information.) 
#   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
#       (Read COPYRIGHT-JIPDEC for detailed information.)
# ----------------------------------------------------------

cat <<GAZONK >>config.h

#undef CPU_TYPE
#define CPU_TYPE $CPU_TYPE
#define $CPU_TYPE

#define DIST_COMPILER_FLAG(name, ext) \
    {	(void)sprintf(bufp, " -DDIST -DSHM_DIST -I%s -I. %s%s", \
		                   klic_incdir, (name), (ext)); }
#define DIST_LINKAGE_FLAG() 

GAZONK
echo " "
