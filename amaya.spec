#
# spec file for package amaya (Version 4.3.1)
# 
# Copyright  (c)  1998  S.u.S.E. GmbH  Fuerth, Germany.
#
# please send bugfixes or comments to feedback@suse.de.
#

# neededforbuild  autoconf automake mmcore mmbase mmslib xpm libz libpng libjpeg

%define version 8.3

Vendor:       W3C World Wide Web Consortium
Distribution: W3C
Name:         amaya
Release:      1
Copyright:    Copyright 1995-2003 (MIT) (INRIA), (L)GPL compatible
Group:        X11/Applications/Networking
URL:          http://www.w3.org/Amaya/
Autoreqprov:  on
Packager:     Irene.Vatton@w3.org
BuildRoot:    /var/tmp/%{name}-buildroot
#BuildRoot:     /usr/src/redhat/BUILD/

Summary:      Web Browser/Editor from the World Wide Web Consortium
Version:      %{version}
Source: ftp://ftp.w3.org/pub/amaya/amaya-src-%{version}.tgz
Source1: ftp://ftp.w3.org/pub/amaya/Dutch.tgz
Source2: ftp://ftp.w3.org/pub/amaya/Spanish.tgz
Source3: ftp://ftp.w3.org/pub/amaya/Italian.tgz
Source4: ftp://ftp.w3.org/pub/amaya/Swedish.tgz
Source5: ftp://ftp.w3.org/pub/amaya/German.tgz
%description

Amaya is a complete web browsing and authoring environment and comes
equipped with a WYSIWYG style of interface, similar to that of the
most popular commercial browsers. With such an interface, users do not
need to know the HTML or CSS languages.

Authors:
--------
    Irene.Vatton@inria.fr, Laurent.Carcone@w3.org
    Vincent.Quint@inria.fr

%prep

%setup -n Amaya
export CFLAGS=-Os
mkdir -p $RPM_BUILD_ROOT/usr/share/
autoconf
mkdir linux
cd linux
# Aids compilation with OpenMotif use on some systems <nkadel@merl.com>
ln -s /usr/X11R6/lib/libXm.a
export HOME=`pwd`
../configure \
	--prefix=/usr/share \
	--exec-prefix=/usr/share \
	--with-webdav \
	--enable-redland \
	--enable-bookmarks
%build
cd linux
make all

%install
cd linux
make install \
	prefix=$RPM_BUILD_ROOT/usr/share \
	exec-prefix=$RPM_BUILD_ROOT/usr/share
mkdir -p $RPM_BUILD_ROOT/usr/bin
/bin/ln -s /usr/share/Amaya/applis/bin/amaya  $RPM_BUILD_ROOT/usr/bin/amaya

%files
/usr/share/Amaya/
/usr/bin/amaya

%doc README.amaya amaya/COPYRIGHT

%changelog
* Thu Oct 2 2003 Irene Vatton <Irene.Vatton@inria.fr>
  updated the list of authors
* Mon Sep 29 2003 Nico Kagel-Garcia <nkadel@merl.com>
  updated for RedHat 9 compatibility
  completed update for amaya 8.1b
  synchronized spec files for amaya/amayagl/amayagtk
  enable webdav
  use libXm.a symlink for compilation
  eliminate %post/%postun symlink for /usr/bin/amaya, move to %install
* Thu Aug 21 2003 Irene Vatton <Irene.Vatton@w3.org>
  changed the optimization flags to "" as there is a crash
  due to the version of compiler.
* Mon Jul 7 2003 Jose Kahan <jose@w3.org>
  Updated for Amaya 8.1. Added the bookmark compilation rules.
  changed the optimization flags to Os as there is a crash on
  with O2 on Redhat 8.0, probably due to the version of compiler.
* Fri Apr 23 2003 Irene Vatton <Irene.Vatton@w3.org> 
  adaptation to redhat 8.0
* Fri Jan 30 2003 Nabil Layaida <Nabil.Layaida@inria.fr>
  adaptation to rpmbuild
* Mon Sep 16 2002 Irene Vatton <Irene.Vatton@w3.org>
  GTK options.
* Fri Nov 9 2001  Irene Vatton <Irene.Vatton@w3.org>
  integration of English and French dictionaries
* Mon Feb 28 2001 Irene Vatton <Irene.Vatton@w3.org>
  updated for amaya-4.2.1
* Mon Jan 8 2001 Irene Vatton <Irene.Vatton@w3.org>
  updated for amaya-4.1
* Sat Nov 11 2000 Daniel Veillard <Daniel.Veillard@w3.org>
  updated for amaya-4.0
* Tue Jul 04 2000 Daniel Veillard <Daniel.Veillard@w3.org>
  updated for amaya-3.2
  removed Ramzi Guetari and Daniel Veillard from authors
* Fri Jun 24 1999 Daniel Veillard <Daniel.Veillard@w3.org>
  Updated for amaya-2.1
* Fri Oct 16 1998 Daniel Veillard <Daniel.Veillard@w3.org>
  took the spec file coming from SuSE-1.3 and updated it for RedHat,
  description, license, and version 1.3b
