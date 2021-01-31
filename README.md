# An unintended Fork

Attention, this is a kind of fork from the [original project elokab-fm](https://github.com/zakariakov/elokab-files-manager).

It was tried to propose patches back via [Pullrequest](https://github.com/zakariakov/elokab-files-manager/pull/1)
but that was not answered so far.

I keep going to improve things here for the moment, especially with improvements for ownClouds Virtual Filesystem on Linux.


# elokab-files-manager

A lightweight  file manager for Linux desktops built in Qt.

# مدير ملفات العقاب

New look in Icons mode

![Screenshots](https://github.com/zakariakov/screenshots/blob/master/elokabfm-iconmode.png)

New look in compact mode

![Screenshots](https://github.com/zakariakov/screenshots/blob/master/elokabfm-compact.png)

Image and pdf and video thumbnails


![Screenshots](https://github.com/zakariakov/screenshots/blob/master/elokabFm-thumbnails.png)



Depends :Qt5-svg , file ,udisks2

Optional dependencies: imagemagick , ffmpeg

# Install

git clone https://gitlab.com/zakariakov/elokab-files-manager.git

cd elokab-files-manager

qmake

make

sudo make install
