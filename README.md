# ownCloud File Manager

A lightweight file manager for Linux desktops with ownCloud capabilities.

## An Unintended Fork

This is a kind of fork from the [original project elokab-fm](https://github.com/zakariakov/elokab-files-manager).

It was tried to propose patches back via [pullrequest](https://github.com/zakariakov/elokab-files-manager/pull/1) but reach out was never answered.

We decided to copy the project and continue to improve it. Special attention is given to make it work with ownCloud, utilizing ownClouds virtual file system on the Linux desktop, and other ownCloud specific things.

# مدير ملفات العقاب

New look in Icons mode

![Screenshots](https://github.com/zakariakov/screenshots/blob/master/elokabfm-iconmode.png)

New look in compact mode

![Screenshots](https://github.com/zakariakov/screenshots/blob/master/elokabfm-compact.png)

Image and pdf and video thumbnails

![Screenshots](https://github.com/zakariakov/screenshots/blob/master/elokabFm-thumbnails.png)


# Build and Install

## Depends :Qt5 libraries

Optional dependencies: imagemagick , ffmpeg

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
