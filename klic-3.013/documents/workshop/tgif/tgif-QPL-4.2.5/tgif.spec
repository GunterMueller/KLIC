%define ver	4.2.5
#
# @(#)$Header: /mm2/home/cvs/bc-src/tgif/tgif.spec.in,v 1.5 2010/09/09 02:28:12 william Exp $
#
%define rel	1
%define prefix	/usr/X11R6
%define name	tgif
Summary:	tgif 2-D drawing package
Summary(pl):	tgif - pakiet do tworzenia grafiki 2D
Name:		%{name}
Version:	%{ver}
Release:	%{rel}
License:	custom (please see Copyright file in distribution)
Group:		X11/Applications/Graphics
Group(pl):	X11/Aplikacje/Grafika
Source:		ftp://bourbon.usc.edu/pub/tgif/%{name}-%{ver}.tar.gz
URL:		http://bourbon.usc.edu/tgif/
BuildRoot:	/tmp/%{name}-%{version}-root
Prefix:		%prefix
Packager:	Roberto A. Oppedisano <rao@penguinpowered.com>

Requires:	xorg-x11-fonts-ISO8859-1-75dpi
Requires:	ghostscript
Requires:	netpbm-progs
Requires:	zlib

%description
Xlib based interactive 2-D drawing facility under X11.
Supports hierarchical construction of drawings and easy navigation between
sets of drawings.  It's also a hyper-graphics
(or hyper-structured-graphics) browser on the World-Wide-Web.
     
Please see the README file for various information including copyright notices.

%prep
%setup -n tgif-%{ver}

%build
xmkmf
make

%install
[ -n "$RPM_BUILD_ROOT" -a "$RPM_BUILD_ROOT" != / ] && rm -rf $RPM_BUILD_ROOT

mkdir -p $RPM_BUILD_ROOT%{prefix}/bin
mkdir -p $RPM_BUILD_ROOT%{prefix}/lib/X11/tgif
mkdir -p $RPM_BUILD_ROOT%{prefix}/man/man1

install -s tgif $RPM_BUILD_ROOT%{prefix}/bin/tgif
install -c -m 0444 tgif.Xdefaults $RPM_BUILD_ROOT%{prefix}/lib/X11/tgif
install -c -m 0444 tgificon.eps $RPM_BUILD_ROOT%{prefix}/lib/X11/tgif
install -c -m 0444 tgificon.obj $RPM_BUILD_ROOT%{prefix}/lib/X11/tgif
install -c -m 0444 tgificon.xbm $RPM_BUILD_ROOT%{prefix}/lib/X11/tgif
install -c -m 0444 tgificon.xpm $RPM_BUILD_ROOT%{prefix}/lib/X11/tgif
install -c -m 0444 tangram.sym $RPM_BUILD_ROOT%{prefix}/lib/X11/tgif
install -c -m 0444 eq4.sym $RPM_BUILD_ROOT%{prefix}/lib/X11/tgif
install -c -m 0444 eq4-2x.sym $RPM_BUILD_ROOT%{prefix}/lib/X11/tgif
install -c -m 0444 eq4-ps2epsi.sym $RPM_BUILD_ROOT%{prefix}/lib/X11/tgif
install -c -m 0444 eq4-epstool.sym $RPM_BUILD_ROOT%{prefix}/lib/X11/tgif
install -c -m 0444 eq4xpm.sym $RPM_BUILD_ROOT%{prefix}/lib/X11/tgif
install -c -m 0444 eq4-lyx-ps2epsi.sym $RPM_BUILD_ROOT%{prefix}/lib/X11/tgif
install -c -m 0444 eq4-2x-nowin-jpg.sym $RPM_BUILD_ROOT%{prefix}/lib/X11/tgif
install -c -m 0444 keys.obj $RPM_BUILD_ROOT%{prefix}/lib/X11/tgif
install -c -m 0444 tgif.man $RPM_BUILD_ROOT%{prefix}/man/man1/tgif.1

%clean
[ -n "$RPM_BUILD_ROOT" -a "$RPM_BUILD_ROOT" != / ] && rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc README Copyright HISTORY
%{prefix}/bin/tgif
%{prefix}/lib/X11/tgif
################################################################
# Note: For RedHat 7.0 or above, use the value below.  If you  #
#       have RedHat 6.x, you should delete the .gz at the end. #
################################################################
%{prefix}/man/man1/tgif.1.gz
