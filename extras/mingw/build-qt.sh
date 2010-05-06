#!/bin/sh

LIBPREFIX=$1

QT_VER="4.6.2"

export MAKEFLAGS=-j3

# Qt4 can not be easily cross-compiled without many patches in the scripts.
# Use MSYS/MinGW with Wine or Windows for the compilation.

PRL2PC="$(pwd)/prl2pc.sh"

if [ ! -d "qt-everywhere-opensource-src-$QT_VER" ]; then
  tar -zxf qt-everywhere-opensource-src-$QT_VER.tar.gz

  # fix compile
  cp qt4_10_optimize-small.diff qt-everywhere-opensource-src-$QT_VER

  cd qt-everywhere-opensource-src-$QT_VER
  patch -p1 < qt4_10_optimize-small.diff
  [ -z "$LIBPREFIX" ] && PREFIX="$(pwd)" || PREFIX=$LIBPREFIX
  ./configure.exe -release \
                  -opensource \
                  -confirm-license \
                  -static \
                  -platform win32-g++ \
                  -no-exceptions \
                  -no-qt3support \
                  -no-xmlpatterns \
                  -no-multimedia \
                  -no-audio-backend \
                  -no-phonon \
                  -no-phonon-backend \
                  -no-opengl \
                  -no-webkit \
                  -no-script \
                  -no-scripttools \
                  -no-gif \
                  -no-libtiff \
                  -no-libmng \
                  -no-openssl \
                  -no-dsp \
                  -no-vcproj \
                  -no-stl \
                  -no-libjpeg \
                  -no-style-plastique \
                  -no-style-cleanlooks \
                  -no-style-motif \
                  -no-style-cde \

  make sub-src

  # Generate pkg-config files with the .prl files.
  mkdir -p lib/pkgconfig
  cp -f lib/QtCore.prl \
        lib/QtGui.prl \
        lib/QtXml.prl \
        lib/QtNetwork.prl \
        lib/pkgconfig
  cd lib/pkgconfig
  $PRL2PC QtCore.prl "" "$PREFIX" $QT_VER
  $PRL2PC QtGui.prl QtCore "$PREFIX" $QT_VER
  $PRL2PC QtXml.prl QtCore "$PREFIX" $QT_VER
  $PRL2PC QtNetwork.prl QtCore "$PREFIX" $QT_VER
  rm -f *.prl
  cd -

  cd -
fi
