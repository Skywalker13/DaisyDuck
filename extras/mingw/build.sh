#!/bin/sh

. ./config

export MAKEFLAGS=-j3

export CC=${CROSS_PREFIX}gcc
export CXX=${CROSS_PREFIX}g++
export LD=${CROSS_PREFIX}ld
export AS=${CROSS_PREFIX}as
export AR=${CROSS_PREFIX}ar
export NM=${CROSS_PREFIX}nm
export RANLIB=${CROSS_PREFIX}ranlib
export OBJCOPY=${CROSS_PREFIX}objcopy
export OBJDUMP=${CROSS_PREFIX}objdump
export STRIP=${CROSS_PREFIX}strip
export DLLTOOL=${CROSS_PREFIX}dlltool
export DLLWRAP=${CROSS_PREFIX}dllwrap
export WINDRES=${CROSS_PREFIX}windres
export WINDMC=${CROSS_PREFIX}windmc

export PKG_CONFIG_LIBDIR="$PREFIX/lib"
export PKG_CONFIG_PATH="$PREFIX/lib/pkgconfig"

export CFLAGS="-Os -s -fomit-frame-pointer -Wall -pipe -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE"
export CXXFLAGS="$CFLAGS"
export LDFLAGS="-Wl,-s -L${MINGW_SYSROOT}/lib -L${PREFIX}/lib"
export CPPFLAGS="-I${MINGW_SYSROOT}/include -I${PREFIX}/include"

export PATH="${MINGW_SYSROOT}/bin:$PATH"


if [ ! -d "libiconv-$ICONV_VER" ]; then
  tar -zxf libiconv-$ICONV_VER.tar.gz
  cd libiconv-$ICONV_VER
  ./configure --host=${HOST} \
              --prefix=${PREFIX} \
              --enable-static \
              --disable-shared \
              --disable-nls \
              --disable-rpath \

  make
  make install
  cd -
fi

if [ ! -d "libxml2-$XML2_VER" ]; then
  tar -zxf libxml2-$XML2_VER.tar.gz
  cd libxml2-$XML2_VER
  LDFLAGS="$LDFLAGS -liconv" \
  ./configure --host=${HOST} \
              --prefix=${PREFIX} \
              --enable-static \
              --disable-shared \
              --without-python \
              --without-debug \
              --without-catalog \
              --without-docbook \
              --without-ftp \
              --without-html \
              --without-legacy \
              --without-pattern \
              --without-regexps \
              --without-sax1 \
              --without-schemas \
              --without-schematron \
              --without-tree \
              --without-valid \
              --without-writer \
              --without-xinclude \
              --without-xpath \
              --without-xptr \
              --without-modules \
              --with-reader \
              --with-minimum \
              --with-iconv=${PREFIX} \

  # fix pkgconfig for static linking
  sed -i "s,\(Libs:.*\),\1 -liconv -lwsock32," libxml-2.0.pc

  make
  make install
  cd -
fi

if [ ! -d "libduck-$DUCK_VER" ]; then
  tar -jxf libduck-$DUCK_VER.tar.bz2
  cd libduck-$DUCK_VER
  ./configure --cross-compile \
              --cross-prefix=${CROSS_PREFIX} \
              --prefix=${PREFIX} \
              --enable-static \
              --disable-shared \
              --enable-small \
              --disable-optimize \
              --disable-debug \

  make
  make install
  cd -
fi

if [ ! -d "ffmpeg-$FFMPEG_VER" ]; then
  tar -jxf ffmpeg-$FFMPEG_VER.tar.bz2
  cd ffmpeg-$FFMPEG_VER
  ./configure --enable-cross-compile \
              --cross-prefix=${CROSS_PREFIX} \
              --target-os=mingw32 \
              --prefix=${PREFIX} \
              --extra-cflags="$CFLAGS" \
              --enable-static \
              --disable-shared \
              --disable-debug \
              --disable-doc \
              --enable-small \
              --enable-stripping \
              --enable-runtime-cpudetect \
              --enable-memalign-hack \
              --enable-w32threads \
              --enable-gpl \
              --enable-decoders \
              --enable-demuxers \
              --enable-parsers \
              --enable-protocols \
              --disable-ffplay \
              --disable-ffserver \
              --disable-ffmpeg \
              --disable-postproc \
              --disable-avfilter \
              --disable-avfilter-lavf \
              --disable-bsfs \
              --disable-x11grab \
              --disable-encoders \
              --disable-muxers \
              --disable-indevs \
              --disable-outdevs \
              --disable-devices \
              --disable-filters \
              --disable-zlib \
              --disable-libfaad \

  make
  make install
  cd -
fi

if [ ! -d "vlc-$VLC_VER" ]; then
  tar -jxf vlc-$VLC_VER.tar.bz2

  # fix cross-compile
  cp vlc_10_skip-cache-gen.diff vlc-$VLC_VER/

  cd vlc-$VLC_VER
  patch -p1 < vlc_10_skip-cache-gen.diff
  ./configure --host=${HOST} \
              --prefix=${PREFIX} \
              --disable-static \
              --enable-shared \
              --enable-avcodec \
              --enable-avformat \
              --disable-nls \
              --disable-vlc \
              --disable-vlm \
              --disable-growl \
              --disable-switcher \
              --disable-sout \
              --disable-lua \
              --disable-mad \
              --disable-a52 \
              --disable-postproc \
              --disable-fribidi \
              --disable-freetype \
              --disable-directx \
              --disable-caca \
              --disable-wingdi \
              --disable-postproc \
              --disable-swscale \
              --disable-faad \
              --disable-twolame \
              --disable-libtar \
              --disable-a52 \
              --disable-dca \
              --disable-flac \
              --disable-libmpeg2 \
              --disable-vorbis \
              --disable-speex \
              --disable-theora \
              --disable-dirac \
              --disable-schroedinger \
              --disable-png \
              --disable-vcd \
              --disable-libcddb \
              --disable-dvb \
              --disable-ogg \
              --disable-mux_ogg \
              --disable-shout \
              --disable-mkv \
              --disable-mod \
              --disable-mpc \
              --disable-gme \
              --disable-dvbpsi \
              --disable-telepathy \
              --disable-sdl \
              --disable-qt4 \
              --disable-skins2 \
              --disable-libgcrypt \
              --disable-xosd \
              --disable-fbosd \
              --disable-osso_screensaver \
              --disable-httpd \
              --disable-libproxy \
              --disable-notify \
              --disable-taglib \
              --disable-sqlite \
              --disable-mozilla \
              --disable-remoteosd \
              --disable-smb \
              --disable-libass \
              --disable-x264 \
              --disable-fluidsynth \
              --disable-zvbi \
              --disable-telx \
              --disable-libass \
              --disable-asademux \
              --disable-kate \
              --disable-tiger \
              --disable-id3tag \
              --disable-activex \
              --disable-screen \
              --disable-dvdread \
              --disable-dvdnav \
              --disable-atmo \
              --disable-libxml2 \
              --disable-svg \
              --disable-visual \
              --disable-goom \
              --disable-bonjour \
              --disable-mtp \
              --disable-live555 \
              --disable-dc1394 \
              --disable-dv \
              --disable-dshow \
              --disable-bda \
              --without-kde-solid \
              --without-x \

  make
  make install
  cd -
fi
