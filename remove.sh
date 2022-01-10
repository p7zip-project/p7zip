#! /bin/sh

DEST_HOME=/usr/local
DEST_BIN=${DEST_HOME}/bin
DEST_SHARE=${DEST_HOME}/lib/p7zip
DEST_MAN=${DEST_HOME}/man
DEST_SHARE_DOC=${DEST_HOME}/share/doc/p7zip
DEST_DIR=
[ "$1" ] && DEST_BIN="$1"
[ "$2" ] && DEST_SHARE="$2"
[ "$3" ] && DEST_MAN="$3"
[ "$4" ] && DEST_SHARE_DOC="$4"
[ "$5" ] && DEST_DIR="$5"

umask 0022

# cleaning previous install
rm -f  "${DEST_DIR}${DEST_BIN}/7z"
rm -f  "${DEST_DIR}${DEST_BIN}/7za"
rm -f  "${DEST_DIR}${DEST_BIN}/7zr"
rm -f  "${DEST_DIR}${DEST_BIN}/7zG"
rm -f  "${DEST_DIR}${DEST_BIN}/p7zipForFilemanager"
rm -f  "${DEST_DIR}${DEST_BIN}/7zFM"
rm -f  "${DEST_DIR}${DEST_SHARE}/7z"
rm -f  "${DEST_DIR}${DEST_SHARE}/7za"
rm -f  "${DEST_DIR}${DEST_SHARE}/7zCon.sfx"
rm -f  "${DEST_DIR}${DEST_SHARE}/7zr"
rm -f  "${DEST_DIR}${DEST_SHARE}/7zG"
rm -f  "${DEST_DIR}${DEST_SHARE}/7zFM"
rm -f  "${DEST_DIR}${DEST_SHARE}/7z.so"
rm -fr "${DEST_DIR}${DEST_SHARE}/Codecs"
rm -fr "${DEST_DIR}${DEST_SHARE}/Formats"
rm -fr "${DEST_DIR}${DEST_SHARE}/Lang"
rm -f  "${DEST_DIR}${DEST_MAN}/man1/7z.1"
rm -f  "${DEST_DIR}${DEST_MAN}/man1/7z.1.gz"
rm -f  "${DEST_DIR}${DEST_MAN}/man1/7za.1"
rm -f  "${DEST_DIR}${DEST_MAN}/man1/7za.1.gz"
rm -f  "${DEST_DIR}${DEST_MAN}/man1/7zr.1"
rm -f  "${DEST_DIR}${DEST_MAN}/man1/7zr.1.gz"
rm -fr  "${DEST_DIR}${DEST_SHARE_DOC}"

