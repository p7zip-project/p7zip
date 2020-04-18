    p7zip 16.02
    ===========

Homepage : http://p7zip.sourceforge.net/

p7zip is a port of the Windows programs 7z.exe and 7za.exe provided by 7-zip.

7-zip is a file archiver with the highest compression ratio.
Homepage : www.7-zip.org

  7z uses plugins (7z.so and Codecs/Rar.so) to handle archives.
  7za is a stand-alone executable (7za handles less archive formats than 7z).
  7zr is a light stand-alone executable that supports only 7z/LZMA/BCJ/BCJ2.

  Some GUI :
    7zG  : beta  stage
    7zFM : alpha stage

CAUTION :
---------

- FIRST : DO NOT USE the 7-zip format for backup purpose on Linux/Unix because :
  - 7-zip does not store the owner/group of the file

  On Linux/Unix, in order to backup directories you must use tar !
  to backup a directory  : tar cf - directory | 7za a -si directory.tar.7z
  to restore your backup : 7za x -so directory.tar.7z | tar xf -

- if you want to send files and directories (not the owner of file)
  to others Unix/MacOS/Windows users, you can use the 7-zip format.

  example : 7za a directory.7z  directory

  do not use "-r" because this flag does not do what you think
  do not use "directory/*" because of ".*" files
   (example : "directory/*" does not match "directory/.profile")

BUILD :
-------

  If you have downloaded the "bin" package,
    use directly the program bin/7za.   (tested on Debian 3, Ubuntu 14.10)
    As the program is statically linked, its should run on many x86 or amd64 linux.

  If you have downloaded the "source" package,

    According to your OS, copy makefile.linux,
    makefile.freebsd, makefile.cygwin, ...
        over makefile.machine

  Example : building 7za, 7z (with its plugins), 7zr and Client7z and passing the internal tests
    cp makefile.linux_x86_asm_gcc_4 makefile.linux
    make all_test


    If you want to make a parallel build on a 4 cpu machine : make -j 4 TARGET

    If you have trouble, try : make -f makefile.oldmake TARGET

    make depend   : to rebuild the makefile.depend
    make clean    : to clean all directories
    make          : to build bin/7za
    make sfx      : to build bin/7zCon.sfx (7za can now create SFX archive)
    make 7z       : to build bin/7z and its plugins :
                          - "bin/7z.so" (GNU LGPL + AES code license)
                          - "bin/Codecs/Rar.so" (GNU LGPL + unRAR restriction)
    make 7zr      : to build bin/7zr
    make all      : to build bin/7za and bin/7zCon.sfx
    make all2     : to build bin/7za, bin/7z (with its plugins) and bin/7zCon.sfx
    make all3     : to build bin/7za, bin/7z (with its plugins), bin/7zr and bin/7zCon.sfx
    make test     : to test bin/7za (extracting, archiving, password ...)
    make test_7z  : to test bin/7z  (extracting, archiving, password ...)
    make test_7zr : to test bin/7zr (extracting, archiving, ...)

    make 7zG      : to build bin/7zG and its plugins :
                          - "bin/7z.so" (GNU LGPL + AES code license)
                          - "bin/Codecs/Rar.so" (GNU LGPL + unRAR restriction)
    make test_7zG : to test bin/7zG (extracting, archiving, ...)


  this procedure has been tested on :
   - x86 CPU :
    Linux - Debian 3.0 Stable
    Linux - Ubuntu 14.10
    Linux - Ubuntu 15.10    
    MacOS 10.6.6

   - AMD64 CPU :
    Linux - Ubuntu 14.10
    Linux - Ubuntu 15.10     
    MacOS 10.6.6

   - powerpc CPU :
    MacOS X 10.4 (ppc)

   - arm :
    Android (Galaxy Note 3)

  Some older versions of p7zip were tested :
   - x86 CPU :
    DOS   - (built with DJGPP, see http://blairdude.googlepages.com/p7zip )
    Linux - Redhat 9.0 Standart
    Linux - Fedora 2 (Redhat) (gcc 3.3.3 and gcc-3.4.1 with
                stack-smashing protector from www.trl.ibm.com/projects/security/ssp/)
    Linux - Mandrake 10.0 Official
    Linux - Ubuntu 8.04
    FreeBSD 5.2.1 (gcc 3.3.3)
    MacOS 10.4.8
    NetBSD
    CYGWIN_NT-5.1 1.5.9(0.112/4/2) 2004-03-18 23:05 i686 Cygwin
    Solaris  9 (x86) with gcc 3.3.2
    Solaris 10 (x86)

   - alpha CPU :
    Linux - Debian 3.0 (alpha) with gcc 2.95.4

   - AMD64 CPU :
    Linux - SuSE 8 ES (AMD64 Opteron) with gcc 3.2.2
    Linux - Fedora 4  (AMD64 Turion)  with gcc 4.0.1
    Linux - Ubuntu 8.10

   - Itanium CPU :
        HP-UX B.11.31 U ia64 with aCC (HP C/aC++ B3910B A.06.14 [Feb 22 2007])

   - s390x CPU :
    Linux - SUSE Enterprise Linux 10 - with gcc 4.1.2
    ( only the 32bits built works, the 64bits built does not pass the tests )

   - sparc CPU :
    Solaris 9 (sparc) with gcc 3.3.2

   - sparc CPU :
    Solaris 8 (sparc) with gcc 2.95.2


   - powerpc CPU :
    MacOS X 10.1/darwin 5.5 with gcc 932.1 (gcc 2.95.2)
    MacOS X 10.3 with XCode 1.5

   - openpower CPU :
        Linux openpower-linux1 2.6.5-7.97-pseries64 (ppc64) with gcc 3.3.3

   - IBM :
    AIX 5.3 with gcc 4.1.0

   - Tru64 :
    OSF 5.1 with gcc 3.4.2

   - PA-RISC :
    HP-UX 11.11 with aCC (HP ANSI C++ B3910B A.03.73)
    HP-UX 11.11 with gcc 3.4.2 (7za and 7zr : OK, 7z : failed because of a buggy gcc for HP-UX,
                                    plugins don't work because C++ constructors are not called !)


BUILD with cmake
----------------
  You can only build 7za.

  cd CPP/7zip/CMAKE

  ./generate.sh  (or ./generate_xcode.sh for MacOSX)
  
  In CPP/7zip, you now have directories like :
   - P7ZIP.codeblocks/
   - P7ZIP.EclipseCDT4/
   - P7ZIP.Unix/
   (or P7ZIP.Xcode/  for MacOSX)
   
   
  For the target "Unix Makefiles" :
	- cd CPP/7zip/P7ZIP.Unix
	- make
	
  Remark : these targets use only C/C++ code. To build targets with ASM code you must read the previous paragraph.	

  For others target, use the IDE (CodeBlocks, KDevelop3, Eclipse CDT4)

INSTALL :
---------

  method 1
  --------
  - edit install.sh to change DEST_HOME
  - ./install.sh : to install
  Remark : you must be "root" to install 7za in the directory "/usr/local"

  method 2
  --------
  - 7za is a stand-alone program, you can put this program where you want.
  example :  cp bin/7za /usr/local/bin/7za

  - 7z needs its plugins. You must copy the file 7z, 7z.so
    and the directory Codecs in the same destination directory.

  - if you want to be able to create SFX archive, copy bin/7zCon.sfx
  to the directory where 7za or 7z can be found.


USAGE:
------
  Remark : you can replace 7za with 7z.

  7za t archive.7z  : tests files in the archive archive.7z
  7za l archive.7z  : lists all files from archive archive.7z
  7za x archive.7z  : extracts all files from archive archive.7z
                   to the current directory

  7za a archive.7z file1 fileN : add files to the archive archive.7z

  7za a archive.7z  dir1 :  add all files and subdirectories from directory "dir1" to archive archive.7z

  CAUTION : do not use the flag "-r" unless you know what you are doing ...

  7za a -sfx archive.x dir1 : add all files from directory "dir1" to SFX archive archive.x

  You can also add or extract files to/from .zip or .tar archives.
  You can also compress/uncompress .gz or .bz2 files.

  7za a -tzip   archive.zip file file2 file3
  7za a -ttar   archive.tar file
  7za a -tgzip  file.gz     file
  7za a -tbzip2 archive.bz2 file

  You can use password for archives:
  7za a -pmy_password archive.7z  dir1

  For .7z archive only, you can enable archive header encryption with "-mhe"
  7za a -mhe=on -pmy_password archive.7z  dir1

  For more, see the documentation of 7za.exe in DOC/MANUAL directory
  7za a -t7z  -m0=lzma -mx=9 -mfb=64 -md=32m -ms=on archive.7z  dir1
      adds all files from directory "dir1" to archive archive.7z
      using "ultra settings".
      -t7z     : 7z archive
      -m0=lzma : lzma method
      -mx=9    : level of compression = 9 (Ultra)
      -mfb=64  : number of fast bytes for LZMA = 64
      -md=32m  : dictionary size = 32 megabytes
      -ms=on   : solid archive = on

  To use the new codec "lzma2"
  7za a -m0=lzma2 archive.7z directory


BENCHMARKS :
------------
  7za b : bench LZMA codec
  7z b -mm=crc : make internal crc check.
  7za b "-mm=*" : test all methods



hugetlbfs (large pages) :
-------------------------

  Using huges pages in Linux requires some preparations.
  First, make sure your running kernel has support for hugetlbfs compiled in:

  ~$ grep hugetlbfs /proc/filesystems
  nodev       hugetlbfs

  You can view your current huge page configuration like this:

  ~$ grep Huge /proc/meminfo
  HugePages_Total:       0
  HugePages_Free:        0
  HugePages_Rsvd:        0
  HugePages_Surp:        0
  Hugepagesize:       2048 kB

  In this case the size of a huge page is 2 MiB.
  So, if you have 2 GiB of RAM and want to reserve 512 MiB for huge pages,
  you would need 256 pages.

  Do the following as root:

  ~# echo 296 >/proc/sys/vm/nr_hugepages
  ~# grep Huge /proc/meminfo
  HugePages_Total:     296
  HugePages_Free:      296
  HugePages_Rsvd:        0
  HugePages_Surp:        0
  Hugepagesize:       2048 kB

  Finally, make access from user space possible:

  ~# mkdir /hugepages
  ~# mount -t hugetlbfs -o rw,nosuid,nodev,noexec,noatime none /hugepages
  ~# chmod 1777 /hugepages

  Option 1 : Now huge pages are configured.
           In your shell, set the environment variable HUGETLB_PATH to the mount point:
           ~$ export HUGETLB_PATH=/hugepages
  or

  Option 2 : Let p7zip find out the first directory with the type "hugetlbfs"


  To enable huge page use in p7zip, pass the '-slp' switch to it.
  Ex: 7za -slp a t.7z dir



  If you want this to be a permanent setup :
  - mkdir /hugepages

  - add an entry to your /etc/fstab so that when you reboot your computer,
    your hugetlbfs will mount automatically :
    hugetlbfs       /hugepages  hugetlbfs     rw,nosuid,nodev,noexec,noatime   0 0

  - add this in your /etc/rc.local :
    chmod 1777 /hugepages
    echo 296 >  /proc/sys/vm/nr_hugepages

  optional : export HUGETLB_PATH=/hugepages

  To enable huge page use in p7zip, pass the '-slp' switch to it : 7za -slp b


  Remark : with current CPU : "7za -slp b" is not faster than "7za b".
           If you want to see some speedup, compress at least 300MB
           Ex : 7za -slp -mx=9 -a big_file.tar.7z  big_file.tar


LICENSE :
---------
  please read DOC/License.txt.

LIMITATIONS from 7-zip :
------------------------

  - does not support uid/gid for the .tar format (so, use tar on Unix)

LIMITATIONS for Unix version only :
-----------------------------------

  - can handle properly UNICODE filenames only if the environment is UTF-8.
  Example : export LANG=en_US.UTF-8 ; unset LC_CTYPE
  Remark  : see the possible values for LANG in the directory
            - /usr/lib/locale   : Fedora 2 / Solaris 8
            - /usr/share/locale : OpenBSD / Debian / FreeBSD / MacOS X / Mandrake 10.1 / NetBSD

    On Linux, to display the available locales : locale -a

  p7zip relies on LC_CTYPE and then on LANG to convert name to/from UNICODE.
  the command "locale" should display these environment variables.
  you can also do "echo $LC_CTYPE" and "echo $LANG".

  If you do not plan to export your archive, you can use the flag "-no-utf16".
  Usage :
    7za a -no-utf16 archive.7z dirOrFile1 ... dirOrFileN
    7za t -no-utf16 archive.7z
    7za l -no-utf16 archive.7z
    7za x -no-utf16 archive.7z

  - ignores Windows file access permissions (files are created with default permissions)

  see also TODO file.

DEVELOPPER CORNER:
------------------

  - static analysis with cppcheck
    make cppcheck

  - static analysis with clang
    scan-build make all4 2>&1 | tee -i scan-build.out

  - dynamic analysis : valgrind
    cp makefile.linux_valgrind makefile.machine
    make test
    
  - dynamic analysis : gcc sanitize
    cp makefile.linux_amd64_sanitizer makefile.machine
    make test

  - WaitForMultipleObject has no equivalence on Unix.
  - Events don't exist.
  - sizeof(wchar_t) = 4 with GCC (2 with MS VC++)
  - "FIXME" in source code indicates that you should add codes to better handle all cases.

  - "FIXED" in source code indicates that the original code has been changed to work in Unix environment.

  see also TODO file.
