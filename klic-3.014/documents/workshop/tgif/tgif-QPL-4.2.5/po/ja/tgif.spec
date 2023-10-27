%define		Xresource_ja_ver	4.1ja9

Name:		tgif
Version:	4.1.45
Release:	5%{?dist}
Summary:	2-D drawing tool
Group:		Applications/Multimedia

License:	QPL
URL:		http://bourbon.usc.edu/tgif/
Source0:	http://downloads.sourceforge.net/%{name}/%{name}-QPL-%{version}.tar.gz
# Borrowed from Vine Linux
Source1:	tgif-%{Xresource_ja_ver}.tar.gz
Source2:	tgif-4.1.41.ja.po
Source3:	tgif.desktop
Source4:	http://bourbon.usc.edu/tgif/images/%{name}.gif
Source5:	tgif-wrapper.sh
Patch1:		tgif-4.1.45-fontcheck.patch
Patch3:		tgif-4.1.35-overthespot_fix.patch
# http://tyche.pu-toyama.ac.jp/~a-urasim/tgif/
Patch10:	tgif-textcursor-a-urasim.patch
Patch20:	tgif-4.1.44-xim-bad-focus.patch
Patch100:	tgif-QPL-4.1.45-open-with-mode.patch
Buildroot:	%{_tmppath}/%{name}-%{version}-%{release}-%(%{__id_u} -n)

BuildRequires:	ImageMagick
BuildRequires:	imake
BuildRequires:	desktop-file-utils
BuildRequires:	gettext
BuildRequires:	libXmu-devel
Requires:	ghostscript
Requires:	netpbm-progs

%description
Tgif  -  Xlib based interactive 2-D drawing facility under
X11.  Supports hierarchical construction of  drawings  and
easy  navigation  between  sets  of drawings.  It's also a
hyper-graphics (or hyper-structured-graphics)  browser  on
the World-Wide-Web.

%prep
%setup -q -n %{name}-QPL-%{version} -a 1
%patch1 -p1 -b .fontcheck
%patch3 -p1 -b .overthespot
%patch10 -p0 -b textcursor
%patch20 -p1 -b .xim-bad-focus
%patch100 -p1 -b .opmode

%{__cp} -pf %{SOURCE2} po/ja/ja.po
pushd po/ja
%{__perl} -pi \
	-e 's,JISX-0208-1983-0,EUC-JP,g' \
	ja.po
popd

# use scalable bitmap font
for i in C ja ; do
  %{__sed} -e s,alias\-mincho,misc\-mincho,g \
           -e s,alias\-gothic,jis\-fixed,g \
           -i tgif-%{Xresource_ja_ver}/${i}/Tgif.ad
done

%build
%{__cp} -pf Tgif.tmpl-linux Tgif.tmpl
%{__sed} -i.mode -e 's|0664|0644|' Tgif.tmpl

xmkmf
%{__sed} -i.mode -e 's|0444|0644|' Makefile
DEFOPTS='-DOVERTHESPOT -DUSE_XT_INITIALIZE -D_ENABLE_NLS -DPRINT_CMD=\"lpr\" -DA4PAPER'
%{__make} %{?_smp_mflags} \
	CC="%{__cc} %{optflags}" \
	MOREDEFINES="$DEFOPTS" \
	TGIFDIR=%{_datadir}/tgif/ \
	LOCAL_LIBRARIES="-lXmu -lXt -lX11" \
	tgif

pushd po
xmkmf 
%{__sed} -i.mode -e 's|0444|0644|' Makefile
%{__make} \
	Makefile \
	Makefiles \
	depend \
	all
popd

%install
%{__rm} -rf $RPM_BUILD_ROOT/
# wrap xgif
%{__make} \
	DESTDIR=$RPM_BUILD_ROOT/ \
	BINDIR=%{_libexecdir}/ \
	TGIFDIR=%{_datadir}/tgif/ \
	INSTALLFLAGS="-cp" \
	install \
	install.man
%{__mkdir_p} $RPM_BUILD_ROOT%{_bindir}/
%{__install} -cpm 0755 %{SOURCE5} $RPM_BUILD_ROOT%{_bindir}/%{name}

%{__rm} -f $RPM_BUILD_ROOT%{_datadir}/tgif/*.obj
%{__install} -cpm 0644 *.obj \
	$RPM_BUILD_ROOT%{_datadir}/tgif/

%{__mkdir_p} $RPM_BUILD_ROOT%{_datadir}/X11/{,ja/}app-defaults/
%{__install} -cpm 0644 tgif-%{Xresource_ja_ver}/C/Tgif.ad \
	$RPM_BUILD_ROOT%{_datadir}/X11/app-defaults/Tgif
%{__install} -cpm 0644 tgif-%{Xresource_ja_ver}/ja/Tgif.ad \
	$RPM_BUILD_ROOT%{_datadir}/X11/ja/app-defaults/Tgif

pushd po
%{__make} \
	DESTDIR=$RPM_BUILD_ROOT/ \
	INSTALLFLAGS="-cp" \
	install
popd

# desktop file & icon
%{__mkdir_p} $RPM_BUILD_ROOT%{_datadir}/applications/
desktop-file-install \
	--remove-category 'Application' \
	--remove-category 'X-Fedora' \
	--vendor 'fedora' \
	--dir $RPM_BUILD_ROOT%{_datadir}/applications/ \
	%{SOURCE3}

%{__mkdir_p} %{buildroot}%{_datadir}/pixmaps/
convert -geometry 64x64! %{SOURCE4} %{name}.png
%{__install} -cpm644 %{name}.png \
	$RPM_BUILD_ROOT/%{_datadir}/pixmaps/

%{find_lang} tgif

%clean
%{__rm} -rf $RPM_BUILD_ROOT/

%post
%{_bindir}/update-desktop-database -q %{_datadir}/applications/ ||:

%postun
%{_bindir}/update-desktop-database -q %{_datadir}/applications/ ||:

%files -f %{name}.lang
%defattr(-,root,root,-)
%doc Copyright HISTORY LICENSE.QPL
%doc README* 
%doc VMS_MAKE_TGIF.COM 
%doc example.tex 
%doc %{name}-%{Xresource_ja_ver}/README.jp

%{_bindir}/%{name}
%{_libexecdir}/%{name}
%{_mandir}/man1/%{name}.1x*

%{_datadir}/%{name}/
%{_datadir}/X11/app-defaults/Tgif
# Currently no package owns the following directories
%dir %{_datadir}/X11/ja/
%dir %{_datadir}/X11/ja/app-defaults/
%{_datadir}/X11/ja/app-defaults/Tgif

%{_datadir}/pixmaps/%{name}.png
%{_datadir}/applications/*-%{name}.desktop

%changelog
* Wed Aug 22 2007 TASAKA Mamoru <mtasaka@ioa.s.u-tokyo.ac.jp> - 4.1.45-5
- Set mode explicitly when open(2) is used with O_CREAT
  due to recent glibc change

* Wed Aug 22 2007 TASAKA Mamoru <mtasaka@ioa.s.u-tokyo.ac.jp> - 4.1.45-4.dist.1
- Mass rebuild (buildID or binutils issue)

* Wed Jul 11 2007 MATSUURA Takanori <t.matsuu at gmail.com> - 4.1.45-4
- based on tgif-4.1.44-0vl6.src.rpm from VineSeed main
- use scalable bitmap font

* Mon Jul  9 2007 TASAKA Mamoru <mtasaka@ioa.s.u-tokyo.ac.jp> - 4.1.45-3
- Clean up BuildRequires

* Thu Jul  5 2007 TASAKA Mamoru <mtasaka@ioa.s.u-tokyo.ac.jp> - 4.1.45-2
- Explicitly set LANG=ja_JP.eucJP on ja_JP locale
- Add needed Requires

* Thu Jul  5 2007 TASAKA Mamoru <mtasaka@ioa.s.u-tokyo.ac.jp> - 4.1.45-1
- Clean up for Fedora

* Sat Apr 07 2007 MATSUBAYASHI Kohji <shaolin@vinelinux.org> 4.1.44-0vl6
- add Patch20 to fix problems with {scim,uim}-anthy (<BTS:512>)

* Thu Jan  4 2007 MATSUURA Takanori <t.matsuu at gmail.com> - 4.1.45-0vl5.1
- based on tgif-4.1.44-0vl5.src.rpm from VineSeed main
- update to 4.1.45
- some ajustments for Fedora Core

* Wed Dec 27 2006 KAZUKI SHIMURA <kazuki@ma.ccnw.ne.jp> 4.1.44-0vl5
- add tgificon.png (source4)
- update tgif.desktop (source3)

* Wed Oct 11 2006 Daisuke SUZUKI <daisuke@linux.or.jp> 4.1.44-0vl4
- add Patch10 to fix text cursor problem (<BTS:250>)
  http://tyche.pu-toyama.ac.jp/~a-urasim/tgif/

* Fri Sep 08 2006 KAZUKI SHIMURA <kazuki@ma.ccnw.ne.jp> 4.1.44-0vl3
- add and update desktop file (source3)
- move desktop file to %%{_datadir}/applications
- exec update-desktop-database at %%post,%%postun
- add Requires(post,postun): desktop-file-utils

* Sat Sep 02 2006 KAZUKI SHIMURA <kazuki@ma.ccnw.ne.jp> 4.1.44-0vl2
- add BuildRequires: XOrg-devel

* Mon Jan 17 2005 Daisuke SUZUKI <daisuke@linux.or.jp> 4.1.44-0vl1
- switch to QPL version

* Thu Jun 10 2004 KOBAYASHI R. Taizo <tkoba@vinelinux.org> 4.1.43-0vl1
- source update

* Thu Jun 12 2003 Ryoichi INAGAKI <ryo1@bc.wakwak.com> 4.1.42-0vl2
- rebuild with new toolchains

* Fri Mar 29 2002 Jun Nishii <jun@vinelinux.org> 4.1.41-0vl3
- add Tate-gaki entry for Ricoh fonts

* Sun Mar 17 2002 Shoji Matsumoto <shom@vinelinux.org> 4.1.41-0vl2
- refine Tgif.ad for Vine 2.5

* Fri Oct 19 2001 Jun Nishii <jun@vinelinux.org> 4.1.41-0vl1
- ver.up 

* Thu Jul 28 2001 Shoji Matsumoto <shom@vinelinux.org>
- 4.1.40-0vl2
- tgif-4.1ja6 (-aliastt-{mincho,gothic}- -> -alias-{mincho,gothic}-)

* Thu Dec  7 2000 Jun Nishii <jun@vinelinux.org>
- 4.1.40-0vl1

* Tue Oct  3 2000 Jun Nishii <jun@vinelinux.org>
- 4.1.39-0vl2
- added documents

* Sun Sep 10 2000 Jun Nishii <jun@vinelinux.org>
- 4.1.39-0vl1

* Thu Aug 17 2000 Yasuyuki Furukawa <furukawa@vinelinux.org>
- added overthespot_fix patch for XIM with OverTheSpot style.

* Fri Aug 11 2000 Jun Nishii <jun@vinelinux.org>
- 4.1.36-0vl1

* Mon Aug  7 2000 Jun Nishii <jun@vinelinux.org>
- 4.1.35-0vl1

* Sat Jul 15 2000 MATSUBAYASHI 'Shaolin' Kohji <shaolin@rhythmaning.org>
- 4.1.34-0vl2
- modified %%files section to handle compressed man page

* Mon May  8 2000 Jun Nishii <jun@vinelinux.org>
- updated 4.0.33

* Thu Apr 20 2000 Yasuyuki Furukawa <furukawa@vinelinux.org>
- updated 4.0.29
- modified fontcheck patch to check signgle byte font, too.

* Thu Mar  9 2000 Yasuyuki Furukawa <furukawa@vinelinux.org>
- updated 4.0.28

* Mon Feb 28 2000 Yasuyuki Furukawa <furukawa@vinelinux.org>
- updated 4.0.27

* Thu Feb 24 2000 Yasuyuki Furukawa <furukawa@vinelinux.org>
- added tgif wmconfig, desktop file

* Fri Feb 18 2000 Yasuyuki Furukawa <furukawa@vinelinux.org>
- added xim unofficial patch from fj.sources to fix a bug about XIM.

* Wed Feb 16 2000 Jun Nishii <jun@vinelinux.org>
- 4.1.26-0vl3
- bug fix in tgif-4.1.26-fontcheck.patch 

* Mon Feb 14 2000 Jun Nishii <jun@vinelinux.org>
- 4.1.26-0vl2
- merge tgif-4.1.26-fontcheck.patch by Mr. Yasuyuki Furukawa 
  which obsoletes trigger for Dynafonts and TrueTypeFonts !

* Thu Jan 20 2000 Jun Nishii <jun@vinelinux.org>
- 4.1.26-0vl2
- added trigger for Dynafonts and TrueTypeFonts

* Thu Jan 20 2000 Yasuyuki Furukawa <furukawa@vinelinux.org>
- updated to 4.1.26
- change ja resource from ja_JP.ujis/app-defaults to ja/app-defaults
- modified font setting

* Wed Nov 17 1999 Jun Nishii <jun@flatout.org>
- updated to 4.1.25

* Thu Nov 4 1999 Jun Nishii <jun@flatout.org>
- updated to 4.1.23

* Thu Oct 28 1999 Jun Nishii <jun@flatout.org>
- rel.4
- update ja.po
- more gettextize in choice.c and menu.c

* Wed Oct 27 1999 Jun Nishii <jun@flatout.org>
- rel.3 
- merge messages in strtbl.c and added japanese catalog

* Tue Oct 26 1999 Jun Nishii <jun@flatout.org>
- rel.2
- enable nls in status buffer and added japanese catalog

* Tue Oct 26 1999 Jun Nishii <jun@flatout.org>
- updated to 4.1.22

* Sun Aug 8 1999 Norihito Ohmori <ohmori@flatout.org>
- archive format change to bzip2
- rebuild for glibc-2.1.x

* Wed Jun 30 1999 Jun Nishii <jun@flatout.org>
- updated to 4.1.16

* Tue Apr 15 1999 Jun Nishii <jun@flatout.org>
- updated to 4.1.7

* Tue Apr 8 1999 Jun Nishii <jun@flatout.org>
- updated to 4.1.6
- Our menufontset-nls patch and xim patch were merged in original source!

* Tue Mar  9 1999 MATSUMOTO Shoji <vine@flatout.org>
- vertical font indicator bug fix
- modify resource and tgif.sh

* Mon Mar 8 1999 Jun Nishii <jun@flatout.org>
- updated to 4.1

* Mon Mar 8 1999 Jun Nishii <jun@flatout.org>
- bug fix in showing shortcut key in menu
- modify document

* Wed Mar  4 1999 MATSUMOTO Shoji <vine@flatout.org>
- set Tgif.InitialFont Ryumin

* Wed Mar  3 1999 MATSUMOTO Shoji <vine@flatout.org>
- add XIM OverTheSpot patch
- modify Tgif-ja.ad

* Mon Mar 2 1999 Jun Nishii <jun@flatout.org>
- updated to 4.0.18

* Mon Mar 1 1999 Jun Nishii <jun@flatout.org>
- make patch to support fontset and nls
- change version name as 4.0.17_jp 

* Sat Feb 27 1999 Jun Nishii <jun@flatout.org>
- modify Tgif-ja.ad (use A4,cm,color-icon,etc...)
- correct document

* Wed Feb 24 1999 Jun Nishii <jun@flatout.org>
- updated to ver. 4.0.17 
- make wrapper to read Tgif-ja

* Sat Feb 20 1999 Jun Nishii <jun@flatout.org>
- updated to ver. 4.0.16

* Tue Feb 16 1999 Jun Nishii <jun@flatout.org>
- build ver. 4.0.14 for Vine Linux
