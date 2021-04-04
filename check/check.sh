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

#set -x

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

sure ${P7ZIP} t ../test/7za433_tar.tar
sure ${P7ZIP} t ../test/7za433_7zip_lzma.7z
sure ${P7ZIP} t -pqwerty ../test/7za433_7zip_lzma_crypto.7z
sure ${P7ZIP} t ../test/7za433_7zip_ppmd.7z
sure ${P7ZIP} t ../test/7za433_7zip_bzip2.7z
sure ${P7ZIP} t ../test/7za433_rar.rar
sure ${P7ZIP} t ../test/7za433_rar4.rar

echo ""
echo "# EXTRACTING ..."
echo "################"

sure tar xf ../test/7za433_tar.tar
sure mv 7za433_tar 7za433_ref

sure ${P7ZIP} x ../test/7za433_tar.tar
sure diff -r 7za433_ref 7za433_tar

sure ${P7ZIP} x ../test/7za433_7zip_lzma.7z
sure diff -r 7za433_ref 7za433_7zip_lzma

sure ${P7ZIP} x ../test/7za433_7zip_lzma_bcj2.7z
sure diff -r 7za433_ref 7za433_7zip_lzma_bcj2

sure ${P7ZIP} x -pqwerty ../test/7za433_7zip_lzma_crypto.7z
sure diff -r 7za433_ref 7za433_7zip_lzma_crypto

sure ${P7ZIP} x ../test/7za433_7zip_ppmd.7z
sure diff -r 7za433_ref 7za433_7zip_ppmd

sure ${P7ZIP} x ../test/7za433_7zip_ppmd_bcj2.7z
sure diff -r 7za433_ref 7za433_7zip_ppmd_bcj2

sure ${P7ZIP} x ../test/7za433_7zip_bzip2.7z
sure diff -r 7za433_ref 7za433_7zip_bzip2

sure ${P7ZIP} x ../test/7za433_rar.rar -y
sure diff -r 7za433_ref 7za433_rar

sure ${P7ZIP} x ../test/7za433_rar4.rar -y
sure diff -r 7za433_ref 7za433_rar4

sure ${P7ZIP} x ../test/7za433_7zip_lzma2.7z
sure diff -r 7za433_ref 7za433_7zip_lzma2

sure ${P7ZIP} x ../test/7za433_7zip_lzma2_bcj2.7z
sure diff -r 7za433_ref 7za433_7zip_lzma2_bcj2

sure ${P7ZIP} x -pqwerty ../test/7za433_7zip_lzma2_crypto.7z
sure diff -r 7za433_ref 7za433_7zip_lzma2_crypto

echo ""
echo "# Archiving ..."
echo "###############"

sure ${P7ZIP} a -ttar 7za433_tar.tar 7za433_tar
sure tar tvf 7za433_tar.tar

sure ${P7ZIP} a 7za433_7zip_lzma.7z 7za433_7zip_lzma

sure ${P7ZIP} a -sfx7zCon.sfx 7za433_7zip_lzma.x 7za433_7zip_lzma

sure ${P7ZIP} a -pqwerty -mhc=on -mhe=on 7za433_7zip_lzma_crypto.7z 7za433_7zip_lzma_crypto # -p = password

sure ${P7ZIP} a -mx=9 -m0=ppmd:mem=64m:o=32 7za433_7zip_ppmd.7z 7za433_7zip_ppmd

sure ${P7ZIP} a -m0=bzip2 7za433_7zip_bzip2.7z 7za433_7zip_bzip2

sure ${P7ZIP} a -t7z 7za433_7zip_lzma.7z 7za433_7zip_lzma -m0=LZMA # m0=methodID

sure ${P7ZIP} a -t7z 7za433_7zip_lzma2.7z 7za433_7zip_lzma -m0=LZMA2

sure ${P7ZIP} a -t7z 7za433_7zip_Deflate.7z 7za433_7zip_lzma -m0=Deflate

sure ${P7ZIP} a -t7z 7za433_7zip_Delta.7z 7za433_7zip_lzma -m0=Delta

sure ${P7ZIP} a -t7z 7za433_7zip_BCJ.7z 7za433_7zip_lzma -m0=BCJ

sure ${P7ZIP} a -t7z 7za433_7zip_BCJ2.7z 7za433_7zip_lzma -m0=BCJ2

sure ${P7ZIP} a -t7z 7za433_7zip_Copy.7z 7za433_7zip_lzma -m0=Copy

sure ${P7ZIP} a -t7z 7za433_7zip_zstd.7z 7za433_7zip_lzma -m0=zstd -mx=22 # mx=level

sure ${P7ZIP} a -t7z 7za433_7zip_lz4.7z 7za433_7zip_lzma -m0=lz4 -mmt=on # mmt=multithreading mode

sure ${P7ZIP} a -t7z 7za433_7zip_lz5.7z 7za433_7zip_lzma -m0=lz5 -mmt=on # mmt=multithreading mode

sure ${P7ZIP} a -t7z 7za433_7zip_brotli.7z 7za433_7zip_lzma -m0=brotli -mmt=on # mmt=multithreading mode

sure ${P7ZIP} a -t7z 7za433_7zip_lizard.7z 7za433_7zip_lzma -m0=lizard -mmt=on # mmt=multithreading mode

sure ${P7ZIP} a -t7z 7za433_7zip_flzma2.7z 7za433_7zip_lzma -m0=flzma2 -mmt=on # mmt=multithreading mode

sure ${P7ZIP} a -t7z 7za433_7zip_lzma3.7z 7za433_7zip_lzma -m0=LZMA -mf=BCJ # mf=FilterID

sure ${P7ZIP} a -t7z 7za433_7zip_lzma4.7z 7za433_7zip_lzma -m0=LZMA -mf=ARM

sure ${P7ZIP} a -t7z 7za433_7zip_lzma5.7z 7za433_7zip_lzma -m0=LZMA -mf=ARMT -mtm=on # mt=multithreading mode

sure ${P7ZIP} a -t7z 7za433_7zip_lzma6.7z 7za433_7zip_lzma -m0=LZMA -mf=BCJ2 -mtc=on # tc=Creation timestamps

sure ${P7ZIP} a -t7z 7za433_7zip_lzma7.7z 7za433_7zip_lzma -m0=LZMA -mf=SPARC -mta=on # mta=last Access timestamps

sure ${P7ZIP} a -t7z 7za433_7zip.7z 7za433_7zip_lzma -m0=bcj -m1=zstd -mx=22

sure ${P7ZIP} a -tzip 7za433_7zip_lzma.zip 7za433_7zip_lzma

sure ${P7ZIP} a -tzip -pqazwsx 7za433_7zip_ZipCrypto.zip 7za433_7zip_lzma -mem=ZipCrypto # mem=Encryption MethodID

sure ${P7ZIP} a -tzip -pqazwsx 7za433_7zip_AES128.zip 7za433_7zip_lzma -mem=AES128 

sure ${P7ZIP} a -tzip -pqazwsx 7za433_7zip_AES192.zip 7za433_7zip_lzma -mem=AES192

sure ${P7ZIP} a -tzip -pqazwsx 7za433_7zip_AES256.zip 7za433_7zip_lzma -mem=AES256

sure ${P7ZIP} a -tzip 7za433_7zip_Copy.zip 7za433_7zip_lzma -mm=Copy -mmt=on # mm=MethodID

sure ${P7ZIP} a -tzip 7za433_7zip_Deflate.zip 7za433_7zip_lzma -mm=Deflate

sure ${P7ZIP} a -tzip 7za433_7zip_Deflate64.zip 7za433_7zip_lzma -mm=Deflate64

sure ${P7ZIP} a -tzip 7za433_7zip_BZip2.zip 7za433_7zip_lzma -mm=BZip2 -md=32m # md={Size}[b|k|m]

sure ${P7ZIP} a -tzip 7za433_7zip_PPMd.zip 7za433_7zip_lzma -mm=PPMd -mmem=24m -mo=8 # mo=model order for PPMd & mmem={Size}[b|k|m]

sure ${P7ZIP} a -tzip 7za433_7zip_mc.zip 7za433_7zip_lzma -mcl=on -mcp=2 # mcl=local code page & mcp=code page

sure ${P7ZIP} a -tzip 7za433_7zip_zstd.zip 7za433_7zip_lzma -mm=zstd

sure ${P7ZIP} a -ttar 7za433_7zip_lzma.tar 7za433_7zip_lzma

sure ${P7ZIP} a -txz 7za433_7zip_lzma.tar.xz 7za433_7zip_lzma.tar

sure ${P7ZIP} a -txz -mcrc0 7za433_7zip_lzma_CRCNone.tar.xz 7za433_7zip_lzma.tar

sure ${P7ZIP} a -txz -mcrc4 7za433_7zip_lzma_CRC32.tar.xz 7za433_7zip_lzma.tar

sure ${P7ZIP} a -txz -mcrc8 7za433_7zip_lzma_CRC64.tar.xz 7za433_7zip_lzma.tar

sure ${P7ZIP} a -tgzip 7za433_7zip_lzma.tar.gz 7za433_7zip_lzma.tar

sure ${P7ZIP} a -tbzip2 7za433_7zip_lzma.tar.bz2 7za433_7zip_lzma.tar

sure ${P7ZIP} a -twim 7za433_7zip_lzma.tar.wim 7za433_7zip_lzma.tar


echo ""
echo "# EXTRACTING (PASS 2) ..."
echo "#########################"

sure rm -fr 7za433_7zip_bzip2 7za433_7zip_lzma 7za433_7zip_lzma_crypto 7za433_7zip_ppmd 7za433_tar

sure ${P7ZIP} x 7za433_tar.tar
sure diff -r 7za433_ref 7za433_tar

sure ${P7ZIP} x 7za433_7zip_lzma.7z
sure diff -r 7za433_ref 7za433_7zip_lzma

sure rm -fr 7za433_7zip_lzma
# FIXME - only for 7zG
sure chmod +x ./7za433_7zip_lzma.x
# sure ${P7ZIP} x -sfx7zCon.sfx 7za433_7zip_lzma.x
sure ${TOOLS} ./7za433_7zip_lzma.x
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma

sure ${P7ZIP} x -pqwerty 7za433_7zip_lzma_crypto.7z
sure diff -r 7za433_ref 7za433_7zip_lzma_crypto

sure ${P7ZIP} x 7za433_7zip_ppmd.7z
sure diff -r 7za433_ref 7za433_7zip_ppmd

sure ${P7ZIP} x 7za433_7zip_bzip2.7z
sure diff -r 7za433_ref 7za433_7zip_bzip2

sure ${P7ZIP} x 7za433_7zip_lzma.7z
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma

sure ${P7ZIP} x 7za433_7zip_lzma2.7z
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma

sure ${P7ZIP} x 7za433_7zip_Deflate.7z
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma

sure ${P7ZIP} x 7za433_7zip_Delta.7z
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma

sure ${P7ZIP} x 7za433_7zip_BCJ.7z
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma

sure ${P7ZIP} x 7za433_7zip_BCJ2.7z
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma

sure ${P7ZIP} x 7za433_7zip_Copy.7z
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma

sure ${P7ZIP} x 7za433_7zip_zstd.7z
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma

sure ${P7ZIP} x 7za433_7zip_lz4.7z
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma

sure ${P7ZIP} x 7za433_7zip_lz5.7z
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma

sure ${P7ZIP} x 7za433_7zip_brotli.7z
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma

sure ${P7ZIP} x 7za433_7zip_lizard.7z
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma

sure ${P7ZIP} x 7za433_7zip_flzma2.7z
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma

sure ${P7ZIP} x 7za433_7zip_lzma3.7z
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma

sure ${P7ZIP} x 7za433_7zip_lzma4.7z
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma

sure ${P7ZIP} x 7za433_7zip_lzma5.7z
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma

sure ${P7ZIP} x 7za433_7zip_lzma6.7z
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma

sure ${P7ZIP} x 7za433_7zip_lzma7.7z
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma

sure ${P7ZIP} x 7za433_7zip.7z
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma

sure ${P7ZIP} x 7za433_7zip_lzma.zip
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma

sure ${P7ZIP} x -pqazwsx 7za433_7zip_ZipCrypto.zip
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma

sure ${P7ZIP} x -pqazwsx 7za433_7zip_AES128.zip
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma

sure ${P7ZIP} x -pqazwsx 7za433_7zip_AES192.zip
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma

sure ${P7ZIP} x -pqazwsx 7za433_7zip_AES256.zip
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma

sure ${P7ZIP} x 7za433_7zip_Copy.zip
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma

sure ${P7ZIP} x 7za433_7zip_Deflate.zip
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma

sure ${P7ZIP} x 7za433_7zip_Deflate64.zip
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma

sure ${P7ZIP} x 7za433_7zip_BZip2.zip
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma

sure ${P7ZIP} x 7za433_7zip_PPMd.zip
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma

sure ${P7ZIP} x 7za433_7zip_mc.zip
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma

sure ${P7ZIP} x 7za433_7zip_zstd.zip
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma

sure ${P7ZIP} x 7za433_7zip_lzma.tar.xz -y
sure ${P7ZIP} x 7za433_7zip_lzma.tar
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma.tar

sure ${P7ZIP} x 7za433_7zip_lzma_CRCNone.tar.xz -y
sure ${P7ZIP} x 7za433_7zip_lzma_CRCNone.tar
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma.tar

sure ${P7ZIP} x 7za433_7zip_lzma_CRC32.tar.xz -y
sure ${P7ZIP} x 7za433_7zip_lzma_CRC32.tar
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma.tar

sure ${P7ZIP} x 7za433_7zip_lzma_CRC64.tar.xz -y
sure ${P7ZIP} x 7za433_7zip_lzma_CRC64.tar
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma.tar

sure ${P7ZIP} x 7za433_7zip_lzma.tar.gz
sure ${P7ZIP} x 7za433_7zip_lzma.tar
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma.tar

sure ${P7ZIP} x 7za433_7zip_lzma.tar.bz2
sure ${P7ZIP} x 7za433_7zip_lzma.tar
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma.tar

sure ${P7ZIP} x 7za433_7zip_lzma.tar.wim
sure ${P7ZIP} x 7za433_7zip_lzma.tar
sure diff -r 7za433_ref 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma
sure rm -rf 7za433_7zip_lzma.tar


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

sure ${P7ZIP} x ../test/7za.exe.lz
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
echo "==========="
echo "ALL SUCCESS"
echo "==========="
echo ""
