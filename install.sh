#! /bin/sh

installShared()
{
  prg=$1
  if [ -x bin/${prg} ]
  then
    echo "- installing ${DEST_DIR}${DEST_BIN}/${prg}"
    cp bin/${prg} "${DEST_DIR}${DEST_SHARE}/${prg}"
    chmod 755 "${DEST_DIR}${DEST_SHARE}/${prg}"
    strip     "${DEST_DIR}${DEST_SHARE}/${prg}"
    echo "#! /bin/sh" > "${DEST_DIR}${DEST_BIN}/${prg}"
    echo "\"${DEST_SHARE}/${prg}\" \"\$@\"" >> "${DEST_DIR}${DEST_BIN}/${prg}"
    chmod 755 "${DEST_DIR}${DEST_BIN}/${prg}"
  fi
}

# global install
DEST_HOME=/usr/local
# for testing 
# DEST_HOME=${HOME}/INSTALL/usr/local
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

use_share="n"

if [ -x bin/7zCon.sfx ]
then
  use_share="o"
fi

if [ -x bin/7z ]
then
  use_share="o"
fi

if [ -x bin/7zG ]
then
  use_share="o"
fi

if [ -x bin/7zFM ]
then
  use_share="o"
fi

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
chmod -R 777 "${DEST_DIR}${DEST_SHARE}/Codecs" 2> /dev/null
chmod -R 777 "${DEST_DIR}${DEST_SHARE}/Formats" 2> /dev/null
rm -fr "${DEST_DIR}${DEST_SHARE}/Codecs"
rm -fr "${DEST_DIR}${DEST_SHARE}/Formats"
rm -fr "${DEST_DIR}${DEST_SHARE}/Lang"
rm -f  "${DEST_DIR}${DEST_MAN}/man1/7z.1"
rm -f  "${DEST_DIR}${DEST_MAN}/man1/7za.1"
rm -f  "${DEST_DIR}${DEST_MAN}/man1/7zr.1"
chmod -R 777 "${DEST_DIR}${DEST_SHARE_DOC}" 2> /dev/null
rm -fr  "${DEST_DIR}${DEST_SHARE_DOC}"

if [ "${use_share}" = "o" ]
then
  mkdir -p "${DEST_DIR}${DEST_BIN}"
  mkdir -p "${DEST_DIR}${DEST_SHARE}"
  installShared 7za
  installShared 7zr
  installShared 7z
  installShared 7zG
  installShared 7zFM

  if [ -x bin/7zCon.sfx ]
  then
    echo "- installing ${DEST_DIR}${DEST_SHARE}/7zCon.sfx"
    cp bin/7zCon.sfx "${DEST_DIR}${DEST_SHARE}/7zCon.sfx"
    chmod 755 "${DEST_DIR}${DEST_SHARE}/7zCon.sfx"
    strip     "${DEST_DIR}${DEST_SHARE}/7zCon.sfx"
  fi

  if [ -x bin/7z.so ]
  then
    echo "- installing ${DEST_DIR}${DEST_SHARE}/7z.so"
    cp bin/7z.so "${DEST_DIR}${DEST_SHARE}/7z.so"
    chmod 755 "${DEST_DIR}${DEST_SHARE}/7z.so"
  fi

  if [ -d bin/Codecs ]
  then
    echo "- installing ${DEST_DIR}${DEST_SHARE}/Codecs"
    cp -r bin/Codecs "${DEST_DIR}${DEST_SHARE}/"
    chmod 755 "${DEST_DIR}${DEST_SHARE}"/*/*
  fi

else
  if [ -x bin/7za ]
  then
    echo "- installing ${DEST_DIR}${DEST_BIN}/7za"
    mkdir -p "${DEST_DIR}${DEST_BIN}"
    cp bin/7za "${DEST_DIR}${DEST_BIN}/7za"
    chmod 755  "${DEST_DIR}${DEST_BIN}/7za"
    strip      "${DEST_DIR}${DEST_BIN}/7za"
  fi

  if [ -x bin/7zr ]
  then
    echo "- installing ${DEST_DIR}${DEST_BIN}/7zr"
    mkdir -p "${DEST_DIR}${DEST_BIN}"
    cp bin/7zr "${DEST_DIR}${DEST_BIN}/7zr"
    chmod 755  "${DEST_DIR}${DEST_BIN}/7zr"
    strip      "${DEST_DIR}${DEST_BIN}/7zr"
  fi
fi

mkdir -p "${DEST_DIR}${DEST_MAN}/man1"
if [ -d DOC ]
then
   echo "- installing ${DEST_DIR}${DEST_MAN}/man1/7z.1"
   sed -e s?"{DEST_SHARE_DOC}"?"${DEST_SHARE_DOC}/DOC"?g man1/7z.1 > "${DEST_DIR}${DEST_MAN}/man1/7z.1"
   chmod 644 "${DEST_DIR}${DEST_MAN}/man1/7z.1"

   echo "- installing ${DEST_DIR}${DEST_MAN}/man1/7za.1"
   sed -e s?"{DEST_SHARE_DOC}"?"${DEST_SHARE_DOC}/DOC"?g man1/7za.1 > "${DEST_DIR}${DEST_MAN}/man1/7za.1"
   chmod 644 "${DEST_DIR}${DEST_MAN}/man1/7za.1"

   echo "- installing ${DEST_DIR}${DEST_MAN}/man1/7zr.1"
   sed -e s?"{DEST_SHARE_DOC}"?"${DEST_SHARE_DOC}/DOC"?g man1/7zr.1 > "${DEST_DIR}${DEST_MAN}/man1/7zr.1"
   chmod 644 "${DEST_DIR}${DEST_MAN}/man1/7zr.1"
else
   echo "- installing ${DEST_DIR}${DEST_MAN}/man1/7z.1"
   grep -v "{DEST_SHARE_DOC}" man1/7z.1 > "${DEST_DIR}${DEST_MAN}/man1/7z.1"
   chmod 644 "${DEST_DIR}${DEST_MAN}/man1/7z.1"

   echo "- installing ${DEST_DIR}${DEST_MAN}/man1/7za.1"
   grep -v "{DEST_SHARE_DOC}" man1/7za.1 > "${DEST_DIR}${DEST_MAN}/man1/7za.1"
   chmod 644 "${DEST_DIR}${DEST_MAN}/man1/7za.1"

   echo "- installing ${DEST_DIR}${DEST_MAN}/man1/7zr.1"
   grep -v "{DEST_SHARE_DOC}" man1/7zr.1 > "${DEST_DIR}${DEST_MAN}/man1/7zr.1"
   chmod 644 "${DEST_DIR}${DEST_MAN}/man1/7zr.1"
fi

if [ -f README ]
then
  echo "- installing ${DEST_DIR}${DEST_SHARE_DOC}/README"
  mkdir -p  "${DEST_DIR}${DEST_SHARE_DOC}"
  cp README "${DEST_DIR}${DEST_SHARE_DOC}/README"
  chmod 644 "${DEST_DIR}${DEST_SHARE_DOC}/README"
fi

if [ -f ChangeLog ]
then
  echo "- installing ${DEST_DIR}${DEST_SHARE_DOC}/ChangeLog"
  mkdir -p     "${DEST_DIR}${DEST_SHARE_DOC}"
  cp ChangeLog "${DEST_DIR}${DEST_SHARE_DOC}/ChangeLog"
  chmod 644    "${DEST_DIR}${DEST_SHARE_DOC}/ChangeLog"
fi

if [ -d DOC ]
then
  echo "- installing HTML help in ${DEST_DIR}${DEST_SHARE_DOC}/DOC"
  mkdir -p "${DEST_DIR}${DEST_SHARE_DOC}"
  cp -r DOC "${DEST_DIR}${DEST_SHARE_DOC}/DOC"
  find "${DEST_DIR}${DEST_SHARE_DOC}/DOC" -type d -exec chmod 755 {} \;
  find "${DEST_DIR}${DEST_SHARE_DOC}/DOC" -type f -exec chmod 644 {} \;
fi

use_lang="n"

if [ -x bin/7zG ]
then
  use_lang="o"
  cp GUI/p7zipForFilemanager "${DEST_DIR}${DEST_BIN}/"
  chmod 755 "${DEST_DIR}${DEST_BIN}/"
fi

if [ -x bin/7zFM ]
then
  use_lang="o"
fi

if [ "${use_lang}" = "o" ]
then
  echo "- installing Lang in ${DEST_DIR}${DEST_SHARE}"
  cp -r GUI/Lang "${DEST_DIR}${DEST_SHARE}/"
  find "${DEST_DIR}${DEST_SHARE}/Lang" -type d -exec chmod 755 {} \;
  find "${DEST_DIR}${DEST_SHARE}/Lang" -type f -exec chmod 644 {} \;
fi

