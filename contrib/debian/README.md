
Debian
====================
This directory contains files used to package neoxad/neoxa-qt
for Debian-based Linux systems. If you compile neoxad/neoxa-qt yourself, there are some useful files here.

## neoxa: URI support ##


neoxa-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install neoxa-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your neoxa-qt binary to `/usr/bin`
and the `../../share/pixmaps/neoxa128.png` to `/usr/share/pixmaps`

neoxa-qt.protocol (KDE)

