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

# set -x

LD_LIBRARY_PATH=`cd ../bin ; pwd`:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH
echo "LD_LIBRARY_PATH=${LD_LIBRARY_PATH}"

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

# for Beos or MacOSX
ln -s ../../bin/7z.so .

echo ""
echo "# TESTING ..."
echo "#############"

sure ${P7ZIP} l ../test/7za433_7zip_lzma.7z
# sure ${P7ZIP} t -pqwerty ../test/7za433_7zip_lzma_crypto.7z
sure ${P7ZIP} l ../test/7za433_7zip_ppmd.7z
sure ${P7ZIP} l ../test/7za433_7zip_bzip2.7z



echo ""
echo "# EXTRACTING ..."
echo "################"

sure tar xf ../test/7za433_tar.tar
sure mv 7za433_tar 7za433_ref

sure ${P7ZIP} x ../test/7za433_7zip_lzma.7z
sure diff -r 7za433_ref 7za433_7zip_lzma

# sure ${P7ZIP} x -pqwerty ../test/7za433_7zip_lzma_crypto.7z
# sure diff -r 7za433_ref 7za433_7zip_lzma_crypto

sure ${P7ZIP} x ../test/7za433_7zip_ppmd.7z
sure diff -r 7za433_ref 7za433_7zip_ppmd

sure ${P7ZIP} x ../test/7za433_7zip_bzip2.7z
sure diff -r 7za433_ref 7za433_7zip_bzip2

echo ""
echo "# Archiving ..."
echo "###############"

sure ${P7ZIP} a 7za433_7zip_lzma.7z 7za433_7zip_lzma/bin/7za.exe 7za433_7zip_lzma/readme.txt 7za433_7zip_lzma/doc/copying.txt

echo ""
echo "# EXTRACTING (PASS 2) ..."
echo "#########################"

sure rm -fr 7za433_7zip_bzip2 7za433_7zip_lzma 7za433_7zip_lzma_crypto 7za433_7zip_ppmd 7za433_tar

sure ${P7ZIP} x 7za433_7zip_lzma.7z
sure diff -r 7za433_ref 7za433_7zip_lzma

cd ..

# ./clean_all.sh
chmod -R 777 ${REP} 2> /dev/null
rm -fr   ${REP}

echo ""
echo "========"
echo "ALL DONE"
echo "========"
echo ""
