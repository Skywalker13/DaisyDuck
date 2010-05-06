#!/bin/sh

PRL="$1"  # foobar.prl
REQ="$2"  # QtCore
SYSROOT="$3"
VERSION="$4"

BASENAME=$(echo $PRL | sed "s,\(.*\)\.prl,\\1,")

QMAKE_PRL_BUILD_DIR=$(grep QMAKE_PRL_BUILD_DIR $PRL | sed "s,.*= \(.*\),\\1,")
QMAKE_PRL_CONFIG=$(grep QMAKE_PRL_CONFIG $PRL | sed "s,.*= \(.*\),\\1,")
QMAKE_PRL_LIBS=$(grep QMAKE_PRL_LIBS $PRL | sed "s,.*= \(.*\),\\1,")

defines=$(grep QMAKE_PRL_DEFINES $PRL | sed "s,.*= \(.*\),\\1,g")
QMAKE_PRL_DEFINES=""
for i in $defines; do
  QMAKE_PRL_DEFINES="-D$i ${QMAKE_PRL_DEFINES}"
done

cat <<EOF >$(echo $PRL | sed "s,\(.*\)prl$,\\1pc,")
prefix=${SYSROOT}
exec_prefix=\${prefix}
libdir=\${prefix}/lib
includedir=\${prefix}/include/${BASENAME}
qt_config=${QMAKE_PRL_CONFIG}

Name: ${BASENAME}
Description: ${BASENAME} Library
Version: ${VERSION}
Libs: -L\${libdir} -l${BASENAME} ${QMAKE_PRL_LIBS}
Cflags: -I\${includedir} -DQT_STATIC ${QMAKE_PRL_DEFINES} \${prefix}/include
Requires: ${REQ}
EOF
