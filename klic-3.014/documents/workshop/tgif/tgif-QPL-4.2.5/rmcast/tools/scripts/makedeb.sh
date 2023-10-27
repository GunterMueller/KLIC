#!/bin/bash

# Variaveis que cujos valores sao passados como parametros
SONUMBER=0
SOURCEDIR=$1
DEBIANDIR=$SOURCEDIR/debian
PACKAGENAME=$2
VERSION=$3
LIBDIR=$4
BINDIR=$5
INCLUDEDIR=$6
DOCDIR=$7/doc/lib$PACKAGENAME$SONUMBER

# Outras variaveis
RELEASE=1land
ARCHITECTURE="i386"
DEBFILENAME="lib${PACKAGENAME}${SONUMBER}_${VERSION}-${RELEASE}_${ARCHITECTURE}.deb"
DEBFILENAME_DEV="lib${PACKAGENAME}${SONUMBER}-dev_${VERSION}-${RELEASE}_${ARCHITECTURE}.deb"

echo "Copiando os arquivos"
rm -rf ${DEBIANDIR}
mkdir -p ${DEBIANDIR}/DEBIAN
make DESTDIR=${DEBIANDIR} install
rm -rf $DEBIANDIR/$BINDIR
strip $DEBIANDIR/$LIBDIR/librmcast.so.0.0.0
chmod -x $DEBIANDIR/$LIBDIR/librmcast.so.0.0.0
mv $DEBIANDIR/$LIBDIR/librmcast.so.0.0.0 $DEBIANDIR/$LIBDIR/librmcast.so.0 $SOURCEDIR

# pacote librmcast0-dev
echo "Gerando o pacote ${DEBFILENAME_DEV}"
chmod -x $DEBIANDIR/$LIBDIR/librmcast.la

echo "Gerando o arquivo md5sums"
DDIR=`echo ${DEBIANDIR} | sed 's:\/:\\\/:g'`
find ${DEBIANDIR} -wholename "${DEBIANDIR}/DEBIAN" -prune -o -type f -exec md5sum '{}' \; | sed -e s/$DDIR// -e 's/\///' > ${DEBIANDIR}/DEBIAN/md5sums

echo "Gerando o arquivo control"
cat > ${DEBIANDIR}/DEBIAN/control <<EOF
Package: lib${PACKAGENAME}${SONUMBER}-dev
Version: ${VERSION}-${RELEASE}
Section: libdevel
Priority: optional
Architecture: ${ARCHITECTURE}
Provides: lib${PACKAGENAME}-dev
Conflicts: lib${PACKAGENAME}-dev
Replaces: lib${PACKAGENAME}-dev
Depends: lib${PACKAGENAME}${SONUMBER}
Maintainer: Support LAND <support@land.ufrj.br>
Description: Reliable multicast library - development headers
 rmcast is a library for data transmission through multicast in a reliable way.
EOF

fakeroot dpkg-deb --build ${DEBIANDIR} ${DEBFILENAME_DEV}

# pacote librmcast0
echo "Gerando o pacote ${DEBFILENAME}"

rm -f $DEBIANDIR/$LIBDIR/librmcast.so $DEBIANDIR/$LIBDIR/librmcast.la $DEBIANDIR/$LIBDIR/librmcast.a
rm -rf $DEBIANDIR/$INCLUDEDIR
mv $SOURCEDIR/librmcast.so.0.0.0 $SOURCEDIR/librmcast.so.0 $DEBIANDIR/$LIBDIR
mkdir -p $DEBIANDIR/$DOCDIR
cp $SOURCEDIR/LICENSE.QPL $DEBIANDIR/$DOCDIR/copyright
chmod 0644 $DEBIANDIR/$DOCDIR/copyright

echo "Gerando o arquivo shlibs ..."
echo "librmcast 0 librmcast0 (>= ${VERSION}-${RELEASE})" > ${DEBIANDIR}/DEBIAN/shlibs

echo "Gerando o arquivo postinst"
cat > ${DEBIANDIR}/DEBIAN/postinst <<EOF
#!/bin/sh
set -e
# Automatically added by dh_makeshlibs
if [ "$1" = "configure" ]; then
    ldconfig
fi
# End automatically added section
EOF
chmod +x ${DEBIANDIR}/DEBIAN/postinst

echo "Gerando o arquivo prerm"
cat > ${DEBIANDIR}/DEBIAN/postrm <<EOF
#!/bin/sh
set -e
# Automatically added by dh_makeshlibs
if [ "$1" = "remove" ]; then
    ldconfig
fi
# End automatically added section
EOF
chmod +x ${DEBIANDIR}/DEBIAN/postrm

echo "Gerando o arquivo md5sums"
DDIR=`echo ${DEBIANDIR} | sed 's:\/:\\\/:g'`
find ${DEBIANDIR} -wholename "${DEBIANDIR}/DEBIAN" -prune -o -type f -exec md5sum '{}' \; | sed -e s/$DDIR// -e 's/\///' > ${DEBIANDIR}/DEBIAN/md5sums
echo "Gerando o arquivo control"
cat > ${DEBIANDIR}/DEBIAN/control <<EOF
Package: lib${PACKAGENAME}${SONUMBER}
Version: ${VERSION}-${RELEASE}
Section: libs
Priority: optional
Architecture: ${ARCHITECTURE}
EOF

touch ${DEBIANDIR}/control
dpkg-shlibdeps -O ${DEBIANDIR}/$LIBDIR/librmcast.so.0.0.0 | sed "s/shlibs:Depends=\(.*\)/Depends: \1/g" >> ${DEBIANDIR}/DEBIAN/control
rm -f ${DEBIANDIR}/control
cat >> ${DEBIANDIR}/DEBIAN/control <<EOF
Maintainer: Support LAND <support@land.ufrj.br>
Description: Reliable multicast library - runtime library
 rmcast is a library for data transmission through multicast in a reliable way.
EOF

fakeroot dpkg-deb --build ${DEBIANDIR} ${DEBFILENAME}

rm -rf ${DEBIANDIR}

exit 0
