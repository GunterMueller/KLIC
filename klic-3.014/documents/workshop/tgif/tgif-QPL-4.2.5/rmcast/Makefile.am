SUBDIRS = . src rmchat mcastproxy
EXTRA_DIST = LICENSE.QPL README.tgwb doc tools tgif etc
dist-hook:
	rm -rf `find $(distdir)/doc -type d -name CVS`
	rm -rf `find $(distdir)/tools -type d -name CVS`
	rm -rf `find $(distdir)/tgif -type d -name CVS`
	rm -rf `find $(distdir)/etc -type d -name CVS`

deb: all
	tools/scripts/makedeb.sh `pwd` $(PACKAGE) $(PACKAGE_VERSION) $(libdir) $(bindir) $(includedir) $(datadir)
