#
# spec file for package amaya (Version 2.0)
# 
# Copyright  (c)  1998  S.u.S.E. GmbH  Fuerth, Germany.
#
# please send bugfixes or comments to feedback@suse.de.
#

# neededforbuild  autoconf automake mmcore mmbase mmslib xpm libz libpng libjpeg

%define version 2.1

Vendor:       W3C World Wide Web Consortium
Distribution: W3C
Name:         amaya
Release:      1
Copyright:    Copyright 1995 (MIT) (INRIA), (L)GPL compatible
Group:        X11/Applications/Networking
URL:          http://www.w3.org/Amaya/
Autoreqprov:  on
Packager:     Daniel.Veillard@w3.org

Summary:      Web Browser/Editor from the World Wide Web Consortium
Version:      %{version}
Source: ftp://ftp.w3.org/pub/amaya/amaya-src-%{version}.tar.gz
Source1: ftp://ftp.w3.org/pub/amaya/English.tar.gz
Source2: ftp://ftp.w3.org/pub/amaya/French.tar.gz
Patch: amaya-src-%{version}.diff
%description

Amaya is a complete web browsing and authoring environment and comes
equipped with a WYSIWYG style of interface, similar to that of the
most popular commercial browsers. With such an interface, users do not
need to know the HTML or CSS languages.

Authors:
--------
    Irene.Vatton@w3.org,Ramzi.Guetari@w3.org,Jose.Kahan@w3.org,
    Vincent.Quint@w3.org,Daniel.Veillard@w3.org

%changelog
* Fri Jun 24 1999 Daniel Veillard <Daniel.Veillard@w3.org>
- Updated for amaya-2.1
* Fri Oct 16 1998 Daniel Veillard <Daniel.Veillard@w3.org>
- took the spec file coming from SuSE-1.3 and updated it for RedHat,
  description, license, and version 1.3b

%prep
%setup -n Amaya
%patch
%build
export CFLAGS=-O2
rm -R libjpeg
# rm -R libpng
autoconf
mkdir linux
cd linux
export HOME=`pwd`
../configure --with-amaya --prefix=/usr --exec-prefix=/usr
#cp Options.orig Options
make all
%install
#touch /.timestamp
touch config/en-javamsg
touch config/fr-javamsg
touch config/java.properties
#mkdir dicopar
#touch dicopar/alphabet
#touch dicopar/clavier
#mkdir fonts
#touch fonts/Families.list
#touch fonts/fonts.dir
#touch fonts/icones1
#touch amaya/COPYRIGHT
#touch amaya/COPYRIGHT.html
#touch amaya/HTML.trans
#touch amaya/HTML.en
#touch amaya/HTML.fr
if [ -e /usr/bin/amaya ] ; then
  rm -f /usr/bin/amaya
fi
cd linux
make install
cd ..
cp -a amaya/AmayaPage.html /usr/share/thot/amaya
%files
%doc COPYRIGHT README.amaya
/usr/bin/amaya
/usr/share/thot

