#! /bin/sh

cd ..

DEST_HOME=${HOME}/INSTALL_FOR_P7ZIP/local
DEST_HOME_SPACE="${HOME}/INSTALL_FOR_P7ZIP/lo  cal"

##########################################################################################"

echo "## PASS 1 - no files "

chmod -R 777 ${HOME}/INSTALL_FOR_P7ZIP
rm -fr       ${HOME}/INSTALL_FOR_P7ZIP

./install.sh "${DEST_HOME}/usr/bin" "${DEST_HOME}/usr/lib/p7zip" "${DEST_HOME}/usr/man" "${DEST_HOME}/usr/share/doc/p7zip"


echo "## PASS 2 - overwrite files "
./install.sh "${DEST_HOME}/usr/bin" "${DEST_HOME}/usr/lib/p7zip" "${DEST_HOME}/usr/man" "${DEST_HOME}/usr/share/doc/p7zip"

mv "${DEST_HOME}" "${DEST_HOME}_REF"

echo "## PASS 3 - with DEST_DIR"

./install.sh "/usr/bin" "/usr/lib/p7zip" "/usr/man" "/usr/share/doc/p7zip"  "${DEST_HOME}/"

diff -r  "${DEST_HOME}/usr/bin" "${DEST_HOME}_REF/usr/bin" > /dev/null
   if [ "$?" = "0" ]
   then
     echo "#####"
     echo "ERROR (bin)"
     echo "#####"
     exit 1
   fi

diff -r  "${DEST_HOME}/usr/man" "${DEST_HOME}_REF/usr/man" > /dev/null
   if [ "$?" = "0" ]
   then
     echo "#####"
     echo "ERROR (man)"
     echo "#####"
     exit 1
   fi

diff -r  "${DEST_HOME}/usr/lib" "${DEST_HOME}_REF/usr/lib" > /dev/null
   if [ "$?" != "0" ]
   then
     echo "#####"
     echo "ERROR (lib)"
     echo "#####"
     exit 1
   fi

diff -r  "${DEST_HOME}/usr/share" "${DEST_HOME}_REF/usr/share" > /dev/null
   if [ "$?" != "0" ]
   then
     echo "#####"
     echo "ERROR (share)"
     echo "#####"
     exit 1
   fi

##########################################################################################"

echo "## PASS 4 - no files "

chmod -R 777 ${HOME}/INSTALL_FOR_P7ZIP
rm -fr       ${HOME}/INSTALL_FOR_P7ZIP

./install.sh "${DEST_HOME_SPACE}/usr/bin" "${DEST_HOME_SPACE}/usr/lib/p7zip" "${DEST_HOME_SPACE}/usr/man" "${DEST_HOME_SPACE}/usr/share/doc/p7zip"


echo "## PASS 5 - overwrite files "
./install.sh "${DEST_HOME_SPACE}/usr/bin" "${DEST_HOME_SPACE}/usr/lib/p7zip" "${DEST_HOME_SPACE}/usr/man" "${DEST_HOME_SPACE}/usr/share/doc/p7zip"

# test each programs
echo "## PASS 6 - programs testing"

"${DEST_HOME_SPACE}/usr/bin/7za" t check/test/7za433_7zip_lzma.7z > /dev/null
  if [ "$?" != "0" ]
  then
    echo "#####"
    echo "ERROR 7za"
    echo "#####"
    exit 1
  fi

"${DEST_HOME_SPACE}/usr/bin/7z" t check/test/7za433_7zip_lzma.7z > /dev/null
  if [ "$?" != "0" ]
  then
    echo "#####"
    echo "ERROR 7z"
    echo "#####"
    exit 1
  fi

"${DEST_HOME_SPACE}/usr/bin/7zr" t check/test/7za433_7zip_lzma.7z > /dev/null
  if [ "$?" != "0" ]
  then
    echo "#####"
    echo "ERROR 7zr"
    echo "#####"
    exit 1
  fi


mv "${DEST_HOME_SPACE}" "${DEST_HOME_SPACE}_REF"

echo "## PASS 7 - with DEST_DIR"

./install.sh "/usr/bin" "/usr/lib/p7zip" "/usr/man" "/usr/share/doc/p7zip"  "${DEST_HOME_SPACE}/"

diff -r  "${DEST_HOME_SPACE}/usr/bin" "${DEST_HOME_SPACE}_REF/usr/bin" > /dev/null
   if [ "$?" = "0" ]
   then
     echo "#####"
     echo "ERROR (bin)"
     echo "#####"
     exit 1
   fi

diff -r  "${DEST_HOME_SPACE}/usr/man" "${DEST_HOME_SPACE}_REF/usr/man" > /dev/null
   if [ "$?" = "0" ]
   then
     echo "#####"
     echo "ERROR (man)"
     echo "#####"
     exit 1
   fi

diff -r  "${DEST_HOME_SPACE}/usr/lib" "${DEST_HOME_SPACE}_REF/usr/lib" > /dev/null
   if [ "$?" != "0" ]
   then
     echo "#####"
     echo "ERROR (lib)"
     echo "#####"
     exit 1
   fi

diff -r  "${DEST_HOME_SPACE}/usr/share" "${DEST_HOME_SPACE}_REF/usr/share" > /dev/null
   if [ "$?" != "0" ]
   then
     echo "#####"
     echo "ERROR (share)"
     echo "#####"
     exit 1
   fi


##########################################################################################"

chmod -R 777 ${HOME}/INSTALL_FOR_P7ZIP
rm -fr       ${HOME}/INSTALL_FOR_P7ZIP

    echo "########"
    echo "All Done"
    echo "########"

