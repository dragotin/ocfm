# elokab-files-manager

A lightweight  file manager for Linux desktops built in Qt.

## An Unintended Fork

Attention, this is a kind of fork from the [original project elokab-fm](https://github.com/zakariakov/elokab-files-manager).

It was tried to propose patches back via [pullrequest](https://github.com/zakariakov/elokab-files-manager/pull/1)
but that was not answered so far.

I keep going to improve things here for the moment, especially with improvements for ownClouds Virtual Filesystem on Linux.


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

Other than the original project, this unintended fork uses cmake to build.

Build instructions:

```
    git clone git@github.com:dragotin/elokab-files-manager.git

    cd elokab-files-manager

    mkdir build

    cd build

    cmake ..

    make

    sudo make install
```
