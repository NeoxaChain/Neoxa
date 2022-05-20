VERSION=4.4.4.2
rm -rf ./release-linux
mkdir release-linux

cp ./src/neoxad ./release-linux/
cp ./src/neoxa-cli ./release-linux/
cp ./src/qt/neoxa-qt ./release-linux/
cp ./NEOXACOIN_small.png ./release-linux/

cd ./release-linux/
strip neoxad
strip neoxa-cli
strip neoxa-qt

#==========================================================
# prepare for packaging deb file.

mkdir neoxacoin-$VERSION
cd neoxacoin-$VERSION
mkdir -p DEBIAN
echo 'Package: neoxacoin
Version: '$VERSION'
Section: base 
Priority: optional 
Architecture: all 
Depends:
Maintainer: Neoxa
Description: Neoxa coin wallet and service.
' > ./DEBIAN/control
mkdir -p ./usr/local/bin/
cp ../neoxad ./usr/local/bin/
cp ../neoxa-cli ./usr/local/bin/
cp ../neoxa-qt ./usr/local/bin/

# prepare for desktop shortcut
mkdir -p ./usr/share/icons/
cp ../NEOXACOIN_small.png ./usr/share/icons/
mkdir -p ./usr/share/applications/
echo '
#!/usr/bin/env xdg-open

[Desktop Entry]
Version=1.0
Type=Application
Terminal=false
Exec=/usr/local/bin/neoxa-qt
Name=neoxacoin
Comment= neoxa coin wallet
Icon=/usr/share/icons/NEOXACOIN_small.png
' > ./usr/share/applications/neoxacoin.desktop

cd ../
# build deb file.
dpkg-deb --build neoxacoin-$VERSION

#==========================================================
# build rpm package
rm -rf ~/rpmbuild/
mkdir -p ~/rpmbuild/{RPMS,SRPMS,BUILD,SOURCES,SPECS,tmp}

cat <<EOF >~/.rpmmacros
%_topdir   %(echo $HOME)/rpmbuild
%_tmppath  %{_topdir}/tmp
EOF

#prepare for build rpm package.
rm -rf neoxacoin-$VERSION
mkdir neoxacoin-$VERSION
cd neoxacoin-$VERSION

mkdir -p ./usr/bin/
cp ../neoxad ./usr/bin/
cp ../neoxa-cli ./usr/bin/
cp ../neoxa-qt ./usr/bin/

# prepare for desktop shortcut
mkdir -p ./usr/share/icons/
cp ../NEOXACOIN_small.png ./usr/share/icons/
mkdir -p ./usr/share/applications/
echo '
[Desktop Entry]
Version=1.0
Type=Application
Terminal=false
Exec=/usr/bin/neoxa-qt
Name=neoxacoin
Comment= neoxa coin wallet
Icon=/usr/share/icons/NEOXACOIN_small.png
' > ./usr/share/applications/neoxacoin.desktop
cd ../

# make tar ball to source folder.
tar -zcvf neoxacoin-$VERSION.tar.gz ./neoxacoin-$VERSION
cp neoxacoin-$VERSION.tar.gz ~/rpmbuild/SOURCES/

# build rpm package.
cd ~/rpmbuild

cat <<EOF > SPECS/neoxacoin.spec
# Don't try fancy stuff like debuginfo, which is useless on binary-only
# packages. Don't strip binary too
# Be sure buildpolicy set to do nothing

Summary: Neoxa wallet rpm package
Name: neoxacoin
Version: $VERSION
Release: 1
License: MIT
SOURCE0 : %{name}-%{version}.tar.gz
URL: https://www.neoxacoin.net/

BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

%description
%{summary}

%prep
%setup -q

%build
# Empty section.

%install
rm -rf %{buildroot}
mkdir -p  %{buildroot}

# in builddir
cp -a * %{buildroot}


%clean
rm -rf %{buildroot}


%files
/usr/share/applications/neoxacoin.desktop
/usr/share/icons/NEOXACOIN_small.png
%defattr(-,root,root,-)
%{_bindir}/*

%changelog
* Tue Aug 24 2021  Neoxa Project Team.
- First Build

EOF

rpmbuild -ba SPECS/neoxacoin.spec



