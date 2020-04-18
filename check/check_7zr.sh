#! /bin/sh

sure()
{
  eval $*
  if [ "$?" != "0" ]
  then
    echo "ERROR during : $*"
    echo "ERROR during : $*" > last_error
    exit 1
  fi
}

P7ZIP="$@"
P7ZIP="${TOOLS} ${P7ZIP}" 


REP=TMP_$$
echo "REP=${REP}"
echo "P7ZIP=${P7ZIP}"
echo "TOOLS=${TOOLS}"

chmod -R 777 ${REP} 2> /dev/null
rm -fr   ${REP}
mkdir -p ${REP}

cd ${REP}

# sure rm -fr 7za433_ref 7za433_7zip_bzip2 7za433_7zip_lzma 7za433_7zip_lzma_crypto 7za433_7zip_ppmd 7za433_tar
# sure rm -fr 7za433_7zip_bzip2.7z 7za433_7zip_lzma.7z 7za433_7zip_lzma_crypto.7z 7za433_7zip_ppmd.7z 7za433_tar.tar

echo ""
echo "# TESTING ..."
echo "#############"

sure ${P7ZIP} t ../test/7za433_7zip_lzma.7z


echo ""
echo "# EXTRACTING ..."
echo "################"

sure tar xf ../test/7za433_tar.tar
sure mv 7za433_tar 7za433_ref

sure ${P7ZIP} x ../test/7za433_7zip_lzma.7z
sure diff -r 7za433_ref 7za433_7zip_lzma

sure ${P7ZIP} x ../test/7za433_7zip_lzma_bcj2.7z
sure diff -r 7za433_ref 7za433_7zip_lzma_bcj2

echo ""
echo "# Archiving ..."
echo "###############"

sure ${P7ZIP} a 7za433_7zip_lzma.7z 7za433_7zip_lzma

echo ""
echo "# EXTRACTING (PASS 2) ..."
echo "#########################"

sure rm -fr 7za433_7zip_lzma

sure ${P7ZIP} x 7za433_7zip_lzma.7z
sure diff -r 7za433_ref 7za433_7zip_lzma

echo ""
echo "# EXTRACTING (LZMA) ..."
echo "#######################"

rm -f 7za.exe

sure ${P7ZIP} x ../test/7za.exe.lzma
sure diff 7za.exe 7za433_ref/bin/7za.exe
sure rm -f 7za.exe

sure ${P7ZIP} x ../test/7za.exe.lzma86
sure diff 7za.exe 7za433_ref/bin/7za.exe
sure rm -f 7za.exe

sure ${P7ZIP} x ../test/7za.exe.lzma_eos
sure diff 7za.exe 7za433_ref/bin/7za.exe
sure rm -f 7za.exe

echo ""
echo "# TESTING (XZ) ..."
echo "#######################"
sure ${P7ZIP} x ../test/7za.exe.xz
sure diff 7za.exe 7za433_ref/bin/7za.exe

chmod +x 7za.exe
sure ${P7ZIP} -txz a 7za.exe.xz 7za.exe
sure rm -f 7za.exe

sure ${P7ZIP} x 7za.exe.xz
sure diff 7za.exe 7za433_ref/bin/7za.exe
sure rm -f 7za.exe

#####################################

cd ..

# ./clean_all.sh
chmod -R 777 ${REP} 2> /dev/null
rm -fr   ${REP}


echo ""
echo "========"
echo "ALL DONE"
echo "========"
echo ""
