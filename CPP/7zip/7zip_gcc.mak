# USE_CLANG=1
# USE_ASM = 1
# IS_X64 = 1
# MY_ARCH =
# USE_ASM=
# USE_JWASM=1

MY_ARCH_2 = $(MY_ARCH)

MY_ASM = asmc
ifdef USE_JWASM
MY_ASM = jwasm
endif


PROGPATH = $(O)/$(PROG)
PROGPATH_STATIC = $(O)/$(PROG)s


ifneq ($(CC), xlc)
CFLAGS_WARN_WALL = -Wall -Wextra #-Werror
endif

# for object file
CFLAGS_BASE_LIST = -c
# CFLAGS_BASE_LIST = -S
CFLAGS_BASE = -O2 $(CFLAGS_BASE_LIST) $(CFLAGS_WARN_WALL) $(CFLAGS_WARN) \
 -DNDEBUG -D_REENTRANT -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE \
 -fPIC

# -D_7ZIP_AFFINITY_DISABLE


ifdef SystemDrive
IS_MINGW = 1
endif

ifdef IS_MINGW
LDFLAGS_STATIC_2 = -static
else
ifndef DEF_FILE
ifndef IS_NOT_STANDALONE
ifndef MY_DYNAMIC_LINK
ifneq ($(CC), clang)
LDFLAGS_STATIC_2 =
# -static
# -static-libstdc++ -static-libgcc
endif
endif
endif
endif
endif

LDFLAGS_STATIC = -DNDEBUG $(LDFLAGS_STATIC_2)

ifndef O
  ifdef IS_MINGW
    O=_o
  else
    O=_o
  endif
endif


ifdef DEF_FILE


ifdef IS_MINGW
SHARED_EXT=.dll
LDFLAGS = -shared -DEF $(DEF_FILE) $(LDFLAGS_STATIC)
else
SHARED_EXT=.so
LDFLAGS = -shared -fPIC $(LDFLAGS_STATIC)
CC_SHARED=-fPIC
endif


else

LDFLAGS = $(LDFLAGS_STATIC)
# -s is not required for clang, do we need it for GGC ???
# -s

#-static -static-libgcc -static-libstdc++

ifdef IS_MINGW
SHARED_EXT=.exe
else
SHARED_EXT=
endif

endif


PROGPATH = $(O)/$(PROG)$(SHARED_EXT)
PROGPATH_STATIC = $(O)/$(PROG)s$(SHARED_EXT)
	
ifdef IS_MINGW

RM = del
MY_MKDIR=mkdir
LIB2_GUI = -lOle32 -lGdi32 -lComctl32 -lComdlg32
LIB2 = -loleaut32 -luuid -ladvapi32 -lUser32 $(LIB2_GUI)

CXXFLAGS_EXTRA = -DUNICODE -D_UNICODE
# -Wno-delete-non-virtual-dtor

DEL_OBJ_EXE = -$(RM) $(O)\*.o $(O)\$(PROG).exe $(O)\$(PROG).dll
 
else

RM = rm -rf
MY_MKDIR=mkdir -p
CD = cd

ifeq ($(shell uname),Darwin)
CP = cp -rf
SET_RPATH = install_name_tool -add_rpath
else
CP = cp -drf
SET_RPATH = patchelf --force-rpath --set-rpath
endif
MAKE = make
# CFLAGS_BASE := $(CFLAGS_BASE) -D_7ZIP_ST
# CXXFLAGS_EXTRA = -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE

# LOCAL_LIBS=-lpthread
# LOCAL_LIBS_DLL=$(LOCAL_LIBS) -ldl
LIB2 = -lpthread -ldl

ifndef INSTALL_PREFIX
INSTALL_PREFIX = /usr/local
endif

ifndef DYSUFFIX

ifeq ($(shell uname),Darwin)
DYSUFFIX=dylib
else
DYSUFFIX=so
endif

endif


DEL_OBJ_EXE = -$(RM) $(O)

endif



CFLAGS = $(MY_ARCH_2) $(LOCAL_FLAGS) $(CFLAGS_BASE2) $(CFLAGS_BASE) $(CC_SHARED) -o $@


ifdef IS_MINGW
AFLAGS_ABI = -coff -DABI_CDECL
AFLAGS = -nologo $(AFLAGS_ABI) -Fo$(O)/$(basename $(<F)).o
else
ifdef IS_X64
AFLAGS_ABI = -elf64 -DABI_LINUX
else
AFLAGS_ABI = -elf -DABI_LINUX -DABI_CDECL
# -DABI_CDECL
# -DABI_LINUX
# -DABI_CDECL
endif
AFLAGS = -nologo $(AFLAGS_ABI) -Fo$(O)/
endif

ifdef USE_ASM
CONSOLE_ASM_FLAGS=-D_7ZIP_ASM
else
CONSOLE_ASM_FLAGS=
endif

CXX_WARN_FLAGS =
#-Wno-invalid-offsetof
#-Wno-reorder

CXXFLAGS = $(MY_ARCH_2) $(LOCAL_FLAGS) $(CXXFLAGS_BASE2) $(CFLAGS_BASE) $(CXXFLAGS_EXTRA) $(CC_SHARED) -o $@ $(CXX_WARN_FLAGS)

STATIC_TARGET=
ifdef COMPL_STATIC
STATIC_TARGET=$(PROGPATH_STATIC)
endif


ifndef 7Z_ADDON_CODEC
7Z_ADDON_CODEC = 7z_addon_codec
endif

7z_ADDON_LIB =
7z_ADDON_LIB_FLAG =
7z_BIN = bin
7z_LIB = lib
ifeq ($(PROG), 7za)
7z_ADDON_LIB = -lzstd -llz4 -lbrotlienc -lbrotlidec -lbrotlicommon -llizard -llz5 -lfast-lzma2 -llzhamcomp -llzhamdecomp -llzhamdll
7z_ADDON_LIB_FLAG = -L./$(O)/$(7z_LIB)/$(7Z_ADDON_CODEC) -Wl,-rpath='$$ORIGIN'/../$(7z_LIB)/$(7Z_ADDON_CODEC)
PROGPATH = $(O)/$(7z_BIN)/$(PROG)$(SHARED_EXT)
else ifeq ($(PROG), 7zz)
7z_ADDON_LIB = -lzstd -llz4 -lbrotlienc -lbrotlidec -lbrotlicommon -llizard -llz5 -lfast-lzma2 -llzhamcomp -llzhamdecomp -llzhamdll
7z_ADDON_LIB_FLAG = -L./$(O)/$(7z_LIB)/$(7Z_ADDON_CODEC) -Wl,-rpath='$$ORIGIN'/../$(7z_LIB)/$(7Z_ADDON_CODEC)
PROGPATH = $(O)/$(7z_BIN)/$(PROG)$(SHARED_EXT)
else ifeq ($(PROG), 7z)
7z_ADDON_LIB = -lzstd -llz4 -lbrotlienc -lbrotlidec -lbrotlicommon -llizard -llz5 -lfast-lzma2 -llzhamcomp -llzhamdecomp -llzhamdll
7z_ADDON_LIB_FLAG = -L./$(O)/$(7z_LIB)/$(7Z_ADDON_CODEC) -Wl,-rpath='$$ORIGIN'/$(7Z_ADDON_CODEC)
PROGPATH = $(O)/$(7z_LIB)/$(PROG)$(SHARED_EXT)
else
# do nothing
endif

all: $(O) $(7z_BIN) $(7Z_ADDON_CODEC) $(PROGPATH) $(STATIC_TARGET)

$(O):
	$(MY_MKDIR) $(O)
$(7z_BIN):
	$(MY_MKDIR) $(O)/$(7z_BIN)
$(7Z_ADDON_CODEC):
	$(MY_MKDIR) $(O)/$(7z_LIB)/$(7Z_ADDON_CODEC)

LFLAGS_ALL = -s $(MY_ARCH_2) $(LDFLAGS) $(LD_arch) $(OBJS) $(MY_LIBS) $(LIB2)
$(PROGPATH): $(ZSTD_LIB) $(LZ4_LIB) $(BROTLI_LIB) $(LIZARD_LIB) $(LZ5_LIB) $(FAST-LZMA2_LIB) $(LZHAM_LIB) $(OBJS)
	$(CXX) -o $(PROGPATH) $(LFLAGS_ALL) $(7z_ADDON_LIB) $(7z_ADDON_LIB_FLAG)

$(PROGPATH_STATIC): $(OBJS)
	$(CXX) -static -o $(PROGPATH_STATIC) $(LFLAGS_ALL)

#	-s strips debug sections from executable in GCC




ifndef NO_DEFAULT_RES
$O/resource.o: resource.rc
	windres.exe $(RFLAGS) resource.rc $O/resource.o
endif

$O/LzmaAlone.o: LzmaAlone.cpp
	$(CXX) $(CXXFLAGS) $<


$O/CommandLineParser.o: ../../../Common/CommandLineParser.cpp
	$(CXX) $(CXXFLAGS) $<
$O/CRC.o: ../../../Common/CRC.cpp
	$(CXX) $(CXXFLAGS) $<

$O/CrcReg.o: ../../../Common/CrcReg.cpp
	$(CXX) $(CXXFLAGS) $<

$O/DynLimBuf.o: ../../../Common/DynLimBuf.cpp
	$(CXX) $(CXXFLAGS) $<
$O/IntToString.o: ../../../Common/IntToString.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Lang.o: ../../../Common/Lang.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ListFileUtils.o: ../../../Common/ListFileUtils.cpp
	$(CXX) $(CXXFLAGS) $<
$O/LzFindPrepare.o: ../../../Common/LzFindPrepare.cpp
	$(CXX) $(CXXFLAGS) $<
$O/MyMap.o: ../../../Common/MyMap.cpp
	$(CXX) $(CXXFLAGS) $<
$O/MyString.o: ../../../Common/MyString.cpp
	$(CXX) $(CXXFLAGS) $<
$O/MyVector.o: ../../../Common/MyVector.cpp
	$(CXX) $(CXXFLAGS) $<
$O/MyWindows.o: ../../../Common/MyWindows.cpp
	$(CXX) $(CXXFLAGS) $<
$O/MyWindows2.o: ../../../Common/MyWindows2.cpp
	$(CXX) $(CXXFLAGS) $<
$O/MyXml.o: ../../../Common/MyXml.cpp
	$(CXX) $(CXXFLAGS) $<
$O/NewHandler.o: ../../../Common/NewHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Random.o: ../../../Common/Random.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Sha1Prepare.o: ../../../Common/Sha1Prepare.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Sha1Reg.o: ../../../Common/Sha1Reg.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Sha256Prepare.o: ../../../Common/Sha256Prepare.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Sha256Reg.o: ../../../Common/Sha256Reg.cpp
	$(CXX) $(CXXFLAGS) $<
$O/StdInStream.o: ../../../Common/StdInStream.cpp
	$(CXX) $(CXXFLAGS) $<
$O/StdOutStream.o: ../../../Common/StdOutStream.cpp
	$(CXX) $(CXXFLAGS) $<
$O/StringConvert.o: ../../../Common/StringConvert.cpp
	$(CXX) $(CXXFLAGS) $<
$O/StringToInt.o: ../../../Common/StringToInt.cpp
	$(CXX) $(CXXFLAGS) $<
$O/TextConfig.o: ../../../Common/TextConfig.cpp
	$(CXX) $(CXXFLAGS) $<
$O/UTFConvert.o: ../../../Common/UTFConvert.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Wildcard.o: ../../../Common/Wildcard.cpp
	$(CXX) $(CXXFLAGS) $<
$O/XzCrc64Init.o: ../../../Common/XzCrc64Init.cpp
	$(CXX) $(CXXFLAGS) $<
$O/XzCrc64Reg.o: ../../../Common/XzCrc64Reg.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Md2Reg.o: ../../../Common/Md2Reg.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Md4Reg.o: ../../../Common/Md4Reg.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Md5Reg.o: ../../../Common/Md5Reg.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Sha384Reg.o: ../../../Common/Sha384Reg.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Sha512Reg.o: ../../../Common/Sha512Reg.cpp
	$(CXX) $(CXXFLAGS) $<
$O/XXH32Reg.o: ../../../Common/XXH32Reg.cpp
	$(CXX) $(CXXFLAGS) $<
$O/XXH64Reg.o: ../../../Common/XXH64Reg.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Blake3Reg.o: ../../../Common/Blake3Reg.cpp
	$(CXX) $(CXXFLAGS) $<


$O/Clipboard.o: ../../../Windows/Clipboard.cpp
	$(CXX) $(CXXFLAGS) $<
$O/COM.o: ../../../Windows/COM.cpp
	$(CXX) $(CXXFLAGS) $<
$O/CommonDialog.o: ../../../Windows/CommonDialog.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Console.o: ../../../Windows/Console.cpp
	$(CXX) $(CXXFLAGS) $<
$O/DLL.o: ../../../Windows/DLL.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ErrorMsg.o: ../../../Windows/ErrorMsg.cpp
	$(CXX) $(CXXFLAGS) $<
$O/FileDir.o: ../../../Windows/FileDir.cpp
	$(CXX) $(CXXFLAGS) $<
$O/FileFind.o: ../../../Windows/FileFind.cpp
	$(CXX) $(CXXFLAGS) $<

$O/FileIO.o: ../../../Windows/FileIO.cpp
	$(CXX) $(CXXFLAGS) $<

$O/FileLink.o: ../../../Windows/FileLink.cpp
	$(CXX) $(CXXFLAGS) $<
$O/FileMapping.o: ../../../Windows/FileMapping.cpp
	$(CXX) $(CXXFLAGS) $<
$O/FileName.o: ../../../Windows/FileName.cpp
	$(CXX) $(CXXFLAGS) $<
$O/FileSystem.o: ../../../Windows/FileSystem.cpp
	$(CXX) $(CXXFLAGS) $<
$O/MemoryGlobal.o: ../../../Windows/MemoryGlobal.cpp
	$(CXX) $(CXXFLAGS) $<
$O/MemoryLock.o: ../../../Windows/MemoryLock.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Menu.o: ../../../Windows/Menu.cpp
	$(CXX) $(CXXFLAGS) $<
$O/NationalTime.o: ../../../Windows/NationalTime.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Net.o: ../../../Windows/Net.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ProcessMessages.o: ../../../Windows/ProcessMessages.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ProcessUtils.o: ../../../Windows/ProcessUtils.cpp
	$(CXX) $(CXXFLAGS) $<
$O/PropVariant.o: ../../../Windows/PropVariant.cpp
	$(CXX) $(CXXFLAGS) $<
$O/PropVariantConv.o: ../../../Windows/PropVariantConv.cpp
	$(CXX) $(CXXFLAGS) $<
$O/PropVariantUtils.o: ../../../Windows/PropVariantUtils.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Registry.o: ../../../Windows/Registry.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ResourceString.o: ../../../Windows/ResourceString.cpp
	$(CXX) $(CXXFLAGS) $<
$O/SecurityUtils.o: ../../../Windows/SecurityUtils.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Shell.o: ../../../Windows/Shell.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Synchronization.o: ../../../Windows/Synchronization.cpp
	$(CXX) $(CXXFLAGS) $<
$O/System.o: ../../../Windows/System.cpp
	$(CXX) $(CXXFLAGS) $<
$O/SystemInfo.o: ../../../Windows/SystemInfo.cpp
	$(CXX) $(CXXFLAGS) $<
$O/TimeUtils.o: ../../../Windows/TimeUtils.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Window.o: ../../../Windows/Window.cpp
	$(CXX) $(CXXFLAGS) $<



$O/ComboBox.o: ../../../Windows/Control/ComboBox.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Dialog.o: ../../../Windows/Control/Dialog.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ImageList.o: ../../../Windows/Control/ImageList.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ListView.o: ../../../Windows/Control/ListView.cpp
	$(CXX) $(CXXFLAGS) $<
$O/PropertyPage.o: ../../../Windows/Control/PropertyPage.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Window2.o: ../../../Windows/Control/Window2.cpp
	$(CXX) $(CXXFLAGS) $<


$O/CreateCoder.o: ../../Common/CreateCoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/CWrappers.o: ../../Common/CWrappers.cpp
	$(CXX) $(CXXFLAGS) $<
$O/FilePathAutoRename.o: ../../Common/FilePathAutoRename.cpp
	$(CXX) $(CXXFLAGS) $<
$O/FileStreams.o: ../../Common/FileStreams.cpp
	$(CXX) $(CXXFLAGS) $<
$O/FilterCoder.o: ../../Common/FilterCoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/InBuffer.o: ../../Common/InBuffer.cpp
	$(CXX) $(CXXFLAGS) $<
$O/InOutTempBuffer.o: ../../Common/InOutTempBuffer.cpp
	$(CXX) $(CXXFLAGS) $<
$O/LimitedStreams.o: ../../Common/LimitedStreams.cpp
	$(CXX) $(CXXFLAGS) $<
$O/LockedStream.o: ../../Common/LockedStream.cpp
	$(CXX) $(CXXFLAGS) $<
$O/MemBlocks.o: ../../Common/MemBlocks.cpp
	$(CXX) $(CXXFLAGS) $<
$O/MethodId.o: ../../Common/MethodId.cpp
	$(CXX) $(CXXFLAGS) $<
$O/MethodProps.o: ../../Common/MethodProps.cpp
	$(CXX) $(CXXFLAGS) $<
$O/OffsetStream.o: ../../Common/OffsetStream.cpp
	$(CXX) $(CXXFLAGS) $<
$O/OutBuffer.o: ../../Common/OutBuffer.cpp
	$(CXX) $(CXXFLAGS) $<
$O/OutMemStream.o: ../../Common/OutMemStream.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ProgressMt.o: ../../Common/ProgressMt.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ProgressUtils.o: ../../Common/ProgressUtils.cpp
	$(CXX) $(CXXFLAGS) $<
$O/PropId.o: ../../Common/PropId.cpp
	$(CXX) $(CXXFLAGS) $<
$O/StreamBinder.o: ../../Common/StreamBinder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/StreamObjects.o: ../../Common/StreamObjects.cpp
	$(CXX) $(CXXFLAGS) $<
$O/StreamUtils.o: ../../Common/StreamUtils.cpp
	$(CXX) $(CXXFLAGS) $<
$O/UniqBlocks.o: ../../Common/UniqBlocks.cpp
	$(CXX) $(CXXFLAGS) $<
$O/VirtThread.o: ../../Common/VirtThread.cpp
	$(CXX) $(CXXFLAGS) $<


$O/ApfsHandler.o: ../../Archive/ApfsHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ApmHandler.o: ../../Archive/ApmHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ArchiveExports.o: ../../Archive/ArchiveExports.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ArHandler.o: ../../Archive/ArHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ArjHandler.o: ../../Archive/ArjHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Base64Handler.o: ../../Archive/Base64Handler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Bz2Handler.o: ../../Archive/Bz2Handler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ComHandler.o: ../../Archive/ComHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/CpioHandler.o: ../../Archive/CpioHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/CramfsHandler.o: ../../Archive/CramfsHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/DeflateProps.o: ../../Archive/DeflateProps.cpp
	$(CXX) $(CXXFLAGS) $<
$O/DllExports.o: ../../Archive/DllExports.cpp
	$(CXX) $(CXXFLAGS) $<
$O/DllExports2.o: ../../Archive/DllExports2.cpp
	$(CXX) $(CXXFLAGS) $<
$O/DmgHandler.o: ../../Archive/DmgHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ElfHandler.o: ../../Archive/ElfHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ExtHandler.o: ../../Archive/ExtHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/FatHandler.o: ../../Archive/FatHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/FlvHandler.o: ../../Archive/FlvHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/GptHandler.o: ../../Archive/GptHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/GzHandler.o: ../../Archive/GzHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/HandlerCont.o: ../../Archive/HandlerCont.cpp
	$(CXX) $(CXXFLAGS) $<
$O/HfsHandler.o: ../../Archive/HfsHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/IhexHandler.o: ../../Archive/IhexHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/LpHandler.o: ../../Archive/LpHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/LzhHandler.o: ../../Archive/LzhHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/LzmaHandler.o: ../../Archive/LzmaHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/MachoHandler.o: ../../Archive/MachoHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/MbrHandler.o: ../../Archive/MbrHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/MslzHandler.o: ../../Archive/MslzHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/MubHandler.o: ../../Archive/MubHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/NtfsHandler.o: ../../Archive/NtfsHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/PeHandler.o: ../../Archive/PeHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/PpmdHandler.o: ../../Archive/PpmdHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/QcowHandler.o: ../../Archive/QcowHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/RpmHandler.o: ../../Archive/RpmHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/SparseHandler.o: ../../Archive/SparseHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/SplitHandler.o: ../../Archive/SplitHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/SquashfsHandler.o: ../../Archive/SquashfsHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/SwfHandler.o: ../../Archive/SwfHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/UefiHandler.o: ../../Archive/UefiHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/VdiHandler.o: ../../Archive/VdiHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/VhdHandler.o: ../../Archive/VhdHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/VhdxHandler.o: ../../Archive/VhdxHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/VmdkHandler.o: ../../Archive/VmdkHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/XarHandler.o: ../../Archive/XarHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/XzHandler.o: ../../Archive/XzHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ZHandler.o: ../../Archive/ZHandler.cpp
	$(CXX) $(CXXFLAGS) $<


$O/7zCompressionMode.o: ../../Archive/7z/7zCompressionMode.cpp
	$(CXX) $(CXXFLAGS) $<
$O/7zDecode.o: ../../Archive/7z/7zDecode.cpp
	$(CXX) $(CXXFLAGS) $<
$O/7zEncode.o: ../../Archive/7z/7zEncode.cpp
	$(CXX) $(CXXFLAGS) $<
$O/7zExtract.o: ../../Archive/7z/7zExtract.cpp
	$(CXX) $(CXXFLAGS) $<
$O/7zFolderInStream.o: ../../Archive/7z/7zFolderInStream.cpp
	$(CXX) $(CXXFLAGS) $<
$O/7zHandler.o: ../../Archive/7z/7zHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/7zHandlerOut.o: ../../Archive/7z/7zHandlerOut.cpp
	$(CXX) $(CXXFLAGS) $<
$O/7zHeader.o: ../../Archive/7z/7zHeader.cpp
	$(CXX) $(CXXFLAGS) $<
$O/7zIn.o: ../../Archive/7z/7zIn.cpp
	$(CXX) $(CXXFLAGS) $<
$O/7zOut.o: ../../Archive/7z/7zOut.cpp
	$(CXX) $(CXXFLAGS) $<
$O/7zProperties.o: ../../Archive/7z/7zProperties.cpp
	$(CXX) $(CXXFLAGS) $<
$O/7zRegister.o: ../../Archive/7z/7zRegister.cpp
	$(CXX) $(CXXFLAGS) $<
$O/7zSpecStream.o: ../../Archive/7z/7zSpecStream.cpp
	$(CXX) $(CXXFLAGS) $<
$O/7zUpdate.o: ../../Archive/7z/7zUpdate.cpp
	$(CXX) $(CXXFLAGS) $<


$O/CabBlockInStream.o: ../../Archive/Cab/CabBlockInStream.cpp
	$(CXX) $(CXXFLAGS) $<
$O/CabHandler.o: ../../Archive/Cab/CabHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/CabHeader.o: ../../Archive/Cab/CabHeader.cpp
	$(CXX) $(CXXFLAGS) $<
$O/CabIn.o: ../../Archive/Cab/CabIn.cpp
	$(CXX) $(CXXFLAGS) $<
$O/CabRegister.o: ../../Archive/Cab/CabRegister.cpp
	$(CXX) $(CXXFLAGS) $<


$O/ChmHandler.o: ../../Archive/Chm/ChmHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ChmIn.o: ../../Archive/Chm/ChmIn.cpp
	$(CXX) $(CXXFLAGS) $<

$O/IsoHandler.o: ../../Archive/Iso/IsoHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/IsoHeader.o: ../../Archive/Iso/IsoHeader.cpp
	$(CXX) $(CXXFLAGS) $<
$O/IsoIn.o: ../../Archive/Iso/IsoIn.cpp
	$(CXX) $(CXXFLAGS) $<
$O/IsoRegister.o: ../../Archive/Iso/IsoRegister.cpp
	$(CXX) $(CXXFLAGS) $<

$O/NsisDecode.o: ../../Archive/Nsis/NsisDecode.cpp
	$(CXX) $(CXXFLAGS) $<
$O/NsisHandler.o: ../../Archive/Nsis/NsisHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/NsisIn.o: ../../Archive/Nsis/NsisIn.cpp
	$(CXX) $(CXXFLAGS) $<
$O/NsisRegister.o: ../../Archive/Nsis/NsisRegister.cpp
	$(CXX) $(CXXFLAGS) $<

$O/Rar5Handler.o: ../../Archive/Rar/Rar5Handler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/RarHandler.o: ../../Archive/Rar/RarHandler.cpp
	$(CXX) $(CXXFLAGS) $<

$O/TarHandler.o: ../../Archive/Tar/TarHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/TarHandlerOut.o: ../../Archive/Tar/TarHandlerOut.cpp
	$(CXX) $(CXXFLAGS) $<
$O/TarHeader.o: ../../Archive/Tar/TarHeader.cpp
	$(CXX) $(CXXFLAGS) $<
$O/TarIn.o: ../../Archive/Tar/TarIn.cpp
	$(CXX) $(CXXFLAGS) $<
$O/TarOut.o: ../../Archive/Tar/TarOut.cpp
	$(CXX) $(CXXFLAGS) $<
$O/TarRegister.o: ../../Archive/Tar/TarRegister.cpp
	$(CXX) $(CXXFLAGS) $<
$O/TarUpdate.o: ../../Archive/Tar/TarUpdate.cpp
	$(CXX) $(CXXFLAGS) $<

$O/UdfHandler.o: ../../Archive/Udf/UdfHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/UdfIn.o: ../../Archive/Udf/UdfIn.cpp
	$(CXX) $(CXXFLAGS) $<


$O/WimHandler.o: ../../Archive/Wim/WimHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/WimHandlerOut.o: ../../Archive/Wim/WimHandlerOut.cpp
	$(CXX) $(CXXFLAGS) $<
$O/WimIn.o: ../../Archive/Wim/WimIn.cpp
	$(CXX) $(CXXFLAGS) $<
$O/WimRegister.o: ../../Archive/Wim/WimRegister.cpp
	$(CXX) $(CXXFLAGS) $<


$O/ZipAddCommon.o: ../../Archive/Zip/ZipAddCommon.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ZipHandler.o: ../../Archive/Zip/ZipHandler.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ZipHandlerOut.o: ../../Archive/Zip/ZipHandlerOut.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ZipIn.o: ../../Archive/Zip/ZipIn.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ZipItem.o: ../../Archive/Zip/ZipItem.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ZipOut.o: ../../Archive/Zip/ZipOut.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ZipUpdate.o: ../../Archive/Zip/ZipUpdate.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ZipRegister.o: ../../Archive/Zip/ZipRegister.cpp
	$(CXX) $(CXXFLAGS) $<


$O/Bcj2Coder.o: ../../Compress/Bcj2Coder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Bcj2Register.o: ../../Compress/Bcj2Register.cpp
	$(CXX) $(CXXFLAGS) $<
$O/BcjCoder.o: ../../Compress/BcjCoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/BcjRegister.o: ../../Compress/BcjRegister.cpp
	$(CXX) $(CXXFLAGS) $<
$O/BitlDecoder.o: ../../Compress/BitlDecoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/BranchMisc.o: ../../Compress/BranchMisc.cpp
	$(CXX) $(CXXFLAGS) $<
$O/BranchRegister.o: ../../Compress/BranchRegister.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ByteSwap.o: ../../Compress/ByteSwap.cpp
	$(CXX) $(CXXFLAGS) $<
$O/BZip2Crc.o: ../../Compress/BZip2Crc.cpp
	$(CXX) $(CXXFLAGS) $<
$O/BZip2Decoder.o: ../../Compress/BZip2Decoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/BZip2Encoder.o: ../../Compress/BZip2Encoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/BZip2Register.o: ../../Compress/BZip2Register.cpp
	$(CXX) $(CXXFLAGS) $<
$O/CodecExports.o: ../../Compress/CodecExports.cpp
	$(CXX) $(CXXFLAGS) $<
$O/CopyCoder.o: ../../Compress/CopyCoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/CopyRegister.o: ../../Compress/CopyRegister.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Deflate64Register.o: ../../Compress/Deflate64Register.cpp
	$(CXX) $(CXXFLAGS) $<
$O/DeflateDecoder.o: ../../Compress/DeflateDecoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/DeflateEncoder.o: ../../Compress/DeflateEncoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/DeflateRegister.o: ../../Compress/DeflateRegister.cpp
	$(CXX) $(CXXFLAGS) $<
$O/DeltaFilter.o: ../../Compress/DeltaFilter.cpp
	$(CXX) $(CXXFLAGS) $<
$O/DllExports2Compress.o: ../../Compress/DllExports2Compress.cpp
	$(CXX) $(CXXFLAGS) $<
$O/DllExportsCompress.o: ../../Compress/DllExportsCompress.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ImplodeDecoder.o: ../../Compress/ImplodeDecoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ImplodeHuffmanDecoder.o: ../../Compress/ImplodeHuffmanDecoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/LzfseDecoder.o: ../../Compress/LzfseDecoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/LzhDecoder.o: ../../Compress/LzhDecoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Lzma2Decoder.o: ../../Compress/Lzma2Decoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Lzma2Encoder.o: ../../Compress/Lzma2Encoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Lzma2Register.o: ../../Compress/Lzma2Register.cpp
	$(CXX) $(CXXFLAGS) $<
$O/LzmaDecoder.o: ../../Compress/LzmaDecoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/LzmaEncoder.o: ../../Compress/LzmaEncoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/LzmaRegister.o: ../../Compress/LzmaRegister.cpp
	$(CXX) $(CXXFLAGS) $<
$O/LzmsDecoder.o: ../../Compress/LzmsDecoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/LzOutWindow.o: ../../Compress/LzOutWindow.cpp
	$(CXX) $(CXXFLAGS) $<
$O/LzxDecoder.o: ../../Compress/LzxDecoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/PpmdDecoder.o: ../../Compress/PpmdDecoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/PpmdEncoder.o: ../../Compress/PpmdEncoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/PpmdRegister.o: ../../Compress/PpmdRegister.cpp
	$(CXX) $(CXXFLAGS) $<
$O/PpmdZip.o: ../../Compress/PpmdZip.cpp
	$(CXX) $(CXXFLAGS) $<
$O/QuantumDecoder.o: ../../Compress/QuantumDecoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Rar1Decoder.o: ../../Compress/Rar1Decoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Rar2Decoder.o: ../../Compress/Rar2Decoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Rar3Decoder.o: ../../Compress/Rar3Decoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Rar3Vm.o: ../../Compress/Rar3Vm.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Rar5Decoder.o: ../../Compress/Rar5Decoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/RarCodecsRegister.o: ../../Compress/RarCodecsRegister.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ShrinkDecoder.o: ../../Compress/ShrinkDecoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/XpressDecoder.o: ../../Compress/XpressDecoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/XzDecoder.o: ../../Compress/XzDecoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/XzEncoder.o: ../../Compress/XzEncoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ZDecoder.o: ../../Compress/ZDecoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ZlibDecoder.o: ../../Compress/ZlibDecoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ZlibEncoder.o: ../../Compress/ZlibEncoder.cpp
	$(CXX) $(CXXFLAGS) $<


$O/7zAes.o: ../../Crypto/7zAes.cpp
	$(CXX) $(CXXFLAGS) $<
$O/7zAesRegister.o: ../../Crypto/7zAesRegister.cpp
	$(CXX) $(CXXFLAGS) $<
$O/HmacSha1.o: ../../Crypto/HmacSha1.cpp
	$(CXX) $(CXXFLAGS) $<
$O/HmacSha256.o: ../../Crypto/HmacSha256.cpp
	$(CXX) $(CXXFLAGS) $<
$O/MyAes.o: ../../Crypto/MyAes.cpp
	$(CXX) $(CXXFLAGS) $<
$O/MyAesReg.o: ../../Crypto/MyAesReg.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Pbkdf2HmacSha1.o: ../../Crypto/Pbkdf2HmacSha1.cpp
	$(CXX) $(CXXFLAGS) $<
$O/RandGen.o: ../../Crypto/RandGen.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Rar20Crypto.o: ../../Crypto/Rar20Crypto.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Rar5Aes.o: ../../Crypto/Rar5Aes.cpp
	$(CXX) $(CXXFLAGS) $<
$O/RarAes.o: ../../Crypto/RarAes.cpp
	$(CXX) $(CXXFLAGS) $<
$O/WzAes.o: ../../Crypto/WzAes.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ZipCrypto.o: ../../Crypto/ZipCrypto.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ZipStrong.o: ../../Crypto/ZipStrong.cpp
	$(CXX) $(CXXFLAGS) $<



$O/CoderMixer2.o: ../../Archive/Common/CoderMixer2.cpp
	$(CXX) $(CXXFLAGS) $<
$O/DummyOutStream.o: ../../Archive/Common/DummyOutStream.cpp
	$(CXX) $(CXXFLAGS) $<
$O/FindSignature.o: ../../Archive/Common/FindSignature.cpp
	$(CXX) $(CXXFLAGS) $<
$O/HandlerOut.o: ../../Archive/Common/HandlerOut.cpp
	$(CXX) $(CXXFLAGS) $<
$O/InStreamWithCRC.o: ../../Archive/Common/InStreamWithCRC.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ItemNameUtils.o: ../../Archive/Common/ItemNameUtils.cpp
	$(CXX) $(CXXFLAGS) $<
$O/MultiStream.o: ../../Archive/Common/MultiStream.cpp
	$(CXX) $(CXXFLAGS) $<
$O/OutStreamWithCRC.o: ../../Archive/Common/OutStreamWithCRC.cpp
	$(CXX) $(CXXFLAGS) $<
$O/OutStreamWithSha1.o: ../../Archive/Common/OutStreamWithSha1.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ParseProperties.o: ../../Archive/Common/ParseProperties.cpp
	$(CXX) $(CXXFLAGS) $<




$O/ArchiveCommandLine.o: ../../UI/Common/ArchiveCommandLine.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ArchiveExtractCallback.o: ../../UI/Common/ArchiveExtractCallback.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ArchiveName.o: ../../UI/Common/ArchiveName.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ArchiveOpenCallback.o: ../../UI/Common/ArchiveOpenCallback.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Bench.o: ../../UI/Common/Bench.cpp
	$(CXX) $(CXXFLAGS) $<
$O/CompressCall.o: ../../UI/Common/CompressCall.cpp
	$(CXX) $(CXXFLAGS) $<
$O/CompressCall2.o: ../../UI/Common/CompressCall2.cpp
	$(CXX) $(CXXFLAGS) $<
$O/DefaultName.o: ../../UI/Common/DefaultName.cpp
	$(CXX) $(CXXFLAGS) $<
$O/EnumDirItems.o: ../../UI/Common/EnumDirItems.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Extract.o: ../../UI/Common/Extract.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ExtractingFilePath.o: ../../UI/Common/ExtractingFilePath.cpp
	$(CXX) $(CXXFLAGS) $<
$O/HashCalc.o: ../../UI/Common/HashCalc.cpp
	$(CXX) $(CXXFLAGS) $<
$O/LoadCodecs.o: ../../UI/Common/LoadCodecs.cpp
	$(CXX) $(CXXFLAGS) $<
$O/OpenArchive.o: ../../UI/Common/OpenArchive.cpp
	$(CXX) $(CXXFLAGS) $<
$O/PropIDUtils.o: ../../UI/Common/PropIDUtils.cpp
	$(CXX) $(CXXFLAGS) $<
$O/SetProperties.o: ../../UI/Common/SetProperties.cpp
	$(CXX) $(CXXFLAGS) $<
$O/SortUtils.o: ../../UI/Common/SortUtils.cpp
	$(CXX) $(CXXFLAGS) $<
$O/TempFiles.o: ../../UI/Common/TempFiles.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Update.o: ../../UI/Common/Update.cpp
	$(CXX) $(CXXFLAGS) $<
$O/UpdateAction.o: ../../UI/Common/UpdateAction.cpp
	$(CXX) $(CXXFLAGS) $<
$O/UpdateCallback.o: ../../UI/Common/UpdateCallback.cpp
	$(CXX) $(CXXFLAGS) $<
$O/UpdatePair.o: ../../UI/Common/UpdatePair.cpp
	$(CXX) $(CXXFLAGS) $<
$O/UpdateProduce.o: ../../UI/Common/UpdateProduce.cpp
	$(CXX) $(CXXFLAGS) $<
$O/WorkDir.o: ../../UI/Common/WorkDir.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ZipRegistry.o: ../../UI/Common/ZipRegistry.cpp
	$(CXX) $(CXXFLAGS) $<


$O/Agent.o: ../../UI/Agent/Agent.cpp
	$(CXX) $(CXXFLAGS) $<
$O/AgentOut.o: ../../UI/Agent/AgentOut.cpp
	$(CXX) $(CXXFLAGS) $<
$O/AgentProxy.o: ../../UI/Agent/AgentProxy.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ArchiveFolder.o: ../../UI/Agent/ArchiveFolder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ArchiveFolderOpen.o: ../../UI/Agent/ArchiveFolderOpen.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ArchiveFolderOut.o: ../../UI/Agent/ArchiveFolderOut.cpp
	$(CXX) $(CXXFLAGS) $<
$O/UpdateCallbackAgent.o: ../../UI/Agent/UpdateCallbackAgent.cpp
	$(CXX) $(CXXFLAGS) $<


$O/Client7z.o: ../../UI/Client7z/Client7z.cpp
	$(CXX) $(CXXFLAGS) $<


$O/BenchCon.o: ../../UI/Console/BenchCon.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ConsoleClose.o: ../../UI/Console/ConsoleClose.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ExtractCallbackConsole.o: ../../UI/Console/ExtractCallbackConsole.cpp
	$(CXX) $(CXXFLAGS) $<
$O/HashCon.o: ../../UI/Console/HashCon.cpp
	$(CXX) $(CXXFLAGS) $<
$O/List.o: ../../UI/Console/List.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Main.o: ../../UI/Console/Main.cpp ../../../../C/7zVersion.h
	$(CXX) $(CXXFLAGS) $(CONSOLE_VARIANT_FLAGS) $(CONSOLE_ASM_FLAGS) $<
$O/MainAr.o: ../../UI/Console/MainAr.cpp
	$(CXX) $(CXXFLAGS) $<
$O/OpenCallbackConsole.o: ../../UI/Console/OpenCallbackConsole.cpp
	$(CXX) $(CXXFLAGS) $<
$O/PercentPrinter.o: ../../UI/Console/PercentPrinter.cpp
	$(CXX) $(CXXFLAGS) $<
$O/UpdateCallbackConsole.o: ../../UI/Console/UpdateCallbackConsole.cpp
	$(CXX) $(CXXFLAGS) $<
$O/UserInputUtils.o: ../../UI/Console/UserInputUtils.cpp
	$(CXX) $(CXXFLAGS) $<

$O/BenchmarkDialog.o: ../../UI/GUI/BenchmarkDialog.cpp
	$(CXX) $(CXXFLAGS) $<
$O/CompressDialog.o: ../../UI/GUI/CompressDialog.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ExtractDialog.o: ../../UI/GUI/ExtractDialog.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ExtractGUI.o: ../../UI/GUI/ExtractGUI.cpp
	$(CXX) $(CXXFLAGS) $<
$O/GUI.o: ../../UI/GUI/GUI.cpp
	$(CXX) $(CXXFLAGS) $<
$O/HashGUI.o: ../../UI/GUI/HashGUI.cpp
	$(CXX) $(CXXFLAGS) $<
$O/UpdateCallbackGUI.o: ../../UI/GUI/UpdateCallbackGUI.cpp
	$(CXX) $(CXXFLAGS) $<
$O/UpdateCallbackGUI2.o: ../../UI/GUI/UpdateCallbackGUI2.cpp
	$(CXX) $(CXXFLAGS) $<
$O/UpdateGUI.o: ../../UI/GUI/UpdateGUI.cpp
	$(CXX) $(CXXFLAGS) $<

$O/MyMessages.o: ../../UI/Explorer/MyMessages.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ContextMenu.o: ../../UI/Explorer/ContextMenu.cpp
	$(CXX) $(CXXFLAGS) $<
$O/DllExportsExplorer.o: ../../UI/Explorer/DllExportsExplorer.cpp
	$(CXX) $(CXXFLAGS) $<
$O/RegistryContextMenu.o: ../../UI/Explorer/RegistryContextMenu.cpp
	$(CXX) $(CXXFLAGS) $<



$O/AboutDialog.o: ../../UI/FileManager/AboutDialog.cpp
	$(CXX) $(CXXFLAGS) $<
$O/AltStreamsFolder.o: ../../UI/FileManager/AltStreamsFolder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/App.o: ../../UI/FileManager/App.cpp
	$(CXX) $(CXXFLAGS) $<
$O/BrowseDialog.o: ../../UI/FileManager/BrowseDialog.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ClassDefs.o: ../../UI/FileManager/ClassDefs.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ComboDialog.o: ../../UI/FileManager/ComboDialog.cpp
	$(CXX) $(CXXFLAGS) $<
$O/CopyDialog.o: ../../UI/FileManager/CopyDialog.cpp
	$(CXX) $(CXXFLAGS) $<
$O/EditDialog.o: ../../UI/FileManager/EditDialog.cpp
	$(CXX) $(CXXFLAGS) $<
$O/EditPage.o: ../../UI/FileManager/EditPage.cpp
	$(CXX) $(CXXFLAGS) $<
$O/EnumFormatEtc.o: ../../UI/FileManager/EnumFormatEtc.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ExtractCallback.o: ../../UI/FileManager/ExtractCallback.cpp
	$(CXX) $(CXXFLAGS) $<
$O/FileFolderPluginOpen.o: ../../UI/FileManager/FileFolderPluginOpen.cpp
	$(CXX) $(CXXFLAGS) $<
$O/FilePlugins.o: ../../UI/FileManager/FilePlugins.cpp
	$(CXX) $(CXXFLAGS) $<
$O/FM.o: ../../UI/FileManager/FM.cpp
	$(CXX) $(CXXFLAGS) $<
$O/FoldersPage.o: ../../UI/FileManager/FoldersPage.cpp
	$(CXX) $(CXXFLAGS) $<
$O/FormatUtils.o: ../../UI/FileManager/FormatUtils.cpp
	$(CXX) $(CXXFLAGS) $<
$O/FSDrives.o: ../../UI/FileManager/FSDrives.cpp
	$(CXX) $(CXXFLAGS) $<
$O/FSFolder.o: ../../UI/FileManager/FSFolder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/FSFolderCopy.o: ../../UI/FileManager/FSFolderCopy.cpp
	$(CXX) $(CXXFLAGS) $<
$O/HelpUtils.o: ../../UI/FileManager/HelpUtils.cpp
	$(CXX) $(CXXFLAGS) $<
$O/LangPage.o: ../../UI/FileManager/LangPage.cpp
	$(CXX) $(CXXFLAGS) $<
$O/LangUtils.o: ../../UI/FileManager/LangUtils.cpp
	$(CXX) $(CXXFLAGS) $<
$O/LinkDialog.o: ../../UI/FileManager/LinkDialog.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ListViewDialog.o: ../../UI/FileManager/ListViewDialog.cpp
	$(CXX) $(CXXFLAGS) $<
$O/MenuPage.o: ../../UI/FileManager/MenuPage.cpp
	$(CXX) $(CXXFLAGS) $<
$O/MessagesDialog.o: ../../UI/FileManager/MessagesDialog.cpp
	$(CXX) $(CXXFLAGS) $<
$O/MyLoadMenu.o: ../../UI/FileManager/MyLoadMenu.cpp
	$(CXX) $(CXXFLAGS) $<
$O/NetFolder.o: ../../UI/FileManager/NetFolder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/OpenCallback.o: ../../UI/FileManager/OpenCallback.cpp
	$(CXX) $(CXXFLAGS) $<
$O/OptionsDialog.o: ../../UI/FileManager/OptionsDialog.cpp
	$(CXX) $(CXXFLAGS) $<
$O/OverwriteDialog.o: ../../UI/FileManager/OverwriteDialog.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Panel.o: ../../UI/FileManager/Panel.cpp
	$(CXX) $(CXXFLAGS) $<
$O/PanelCopy.o: ../../UI/FileManager/PanelCopy.cpp
	$(CXX) $(CXXFLAGS) $<
$O/PanelCrc.o: ../../UI/FileManager/PanelCrc.cpp
	$(CXX) $(CXXFLAGS) $<
$O/PanelDrag.o: ../../UI/FileManager/PanelDrag.cpp
	$(CXX) $(CXXFLAGS) $<
$O/PanelFolderChange.o: ../../UI/FileManager/PanelFolderChange.cpp
	$(CXX) $(CXXFLAGS) $<
$O/PanelItemOpen.o: ../../UI/FileManager/PanelItemOpen.cpp
	$(CXX) $(CXXFLAGS) $<
$O/PanelItems.o: ../../UI/FileManager/PanelItems.cpp
	$(CXX) $(CXXFLAGS) $<
$O/PanelKey.o: ../../UI/FileManager/PanelKey.cpp
	$(CXX) $(CXXFLAGS) $<
$O/PanelListNotify.o: ../../UI/FileManager/PanelListNotify.cpp
	$(CXX) $(CXXFLAGS) $<
$O/PanelMenu.o: ../../UI/FileManager/PanelMenu.cpp
	$(CXX) $(CXXFLAGS) $<
$O/PanelOperations.o: ../../UI/FileManager/PanelOperations.cpp
	$(CXX) $(CXXFLAGS) $<
$O/PanelSelect.o: ../../UI/FileManager/PanelSelect.cpp
	$(CXX) $(CXXFLAGS) $<
$O/PanelSort.o: ../../UI/FileManager/PanelSort.cpp
	$(CXX) $(CXXFLAGS) $<
$O/PanelSplitFile.o: ../../UI/FileManager/PanelSplitFile.cpp
	$(CXX) $(CXXFLAGS) $<
$O/PasswordDialog.o: ../../UI/FileManager/PasswordDialog.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ProgramLocation.o: ../../UI/FileManager/ProgramLocation.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ProgressDialog.o: ../../UI/FileManager/ProgressDialog.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ProgressDialog2.o: ../../UI/FileManager/ProgressDialog2.cpp
	$(CXX) $(CXXFLAGS) $<
$O/PropertyName.o: ../../UI/FileManager/PropertyName.cpp
	$(CXX) $(CXXFLAGS) $<
$O/RegistryAssociations.o: ../../UI/FileManager/RegistryAssociations.cpp
	$(CXX) $(CXXFLAGS) $<
$O/RegistryPlugins.o: ../../UI/FileManager/RegistryPlugins.cpp
	$(CXX) $(CXXFLAGS) $<
$O/RegistryUtils.o: ../../UI/FileManager/RegistryUtils.cpp
	$(CXX) $(CXXFLAGS) $<
$O/RootFolder.o: ../../UI/FileManager/RootFolder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/SettingsPage.o: ../../UI/FileManager/SettingsPage.cpp
	$(CXX) $(CXXFLAGS) $<
$O/SplitDialog.o: ../../UI/FileManager/SplitDialog.cpp
	$(CXX) $(CXXFLAGS) $<
$O/SplitUtils.o: ../../UI/FileManager/SplitUtils.cpp
	$(CXX) $(CXXFLAGS) $<
$O/StringUtils.o: ../../UI/FileManager/StringUtils.cpp
	$(CXX) $(CXXFLAGS) $<
$O/SysIconUtils.o: ../../UI/FileManager/SysIconUtils.cpp
	$(CXX) $(CXXFLAGS) $<
$O/SystemPage.o: ../../UI/FileManager/SystemPage.cpp
	$(CXX) $(CXXFLAGS) $<
$O/TextPairs.o: ../../UI/FileManager/TextPairs.cpp
	$(CXX) $(CXXFLAGS) $<
$O/UpdateCallback100.o: ../../UI/FileManager/UpdateCallback100.cpp
	$(CXX) $(CXXFLAGS) $<
$O/VerCtrl.o: ../../UI/FileManager/VerCtrl.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ViewSettings.o: ../../UI/FileManager/ViewSettings.cpp
	$(CXX) $(CXXFLAGS) $<

$O/SfxCon.o: ../../Bundles/SFXCon/SfxCon.cpp
	$(CXX) $(CXXFLAGS) $<

$O/$(FILE_IO).o: ../../../$(FILE_IO_2).cpp
	$(CXX) $(CXXFLAGS) $<






$O/7zAlloc.o: ../../../../C/7zAlloc.c
	$(CC) $(CFLAGS) $<
$O/7zArcIn.o: ../../../../C/7zArcIn.c
	$(CC) $(CFLAGS) $<
$O/7zBuf.o: ../../../../C/7zBuf.c
	$(CC) $(CFLAGS) $<
$O/7zBuf2.o: ../../../../C/7zBuf2.c
	$(CC) $(CFLAGS) $<
$O/7zCrc.o: ../../../../C/7zCrc.c
	$(CC) $(CFLAGS) $<
$O/7zDec.o: ../../../../C/7zDec.c
	$(CC) $(CFLAGS) $<
$O/7zFile.o: ../../../../C/7zFile.c
	$(CC) $(CFLAGS) $<
$O/7zStream.o: ../../../../C/7zStream.c
	$(CC) $(CFLAGS) $<
$O/Aes.o: ../../../../C/Aes.c
	$(CC) $(CFLAGS) $<
$O/Alloc.o: ../../../../C/Alloc.c
	$(CC) $(CFLAGS) $<
$O/Bcj2.o: ../../../../C/Bcj2.c
	$(CC) $(CFLAGS) $<
$O/Bcj2Enc.o: ../../../../C/Bcj2Enc.c
	$(CC) $(CFLAGS) $<
$O/Blake2s.o: ../../../../C/Blake2s.c
	$(CC) $(CFLAGS) $<
$O/Bra.o: ../../../../C/Bra.c
	$(CC) $(CFLAGS) $<
$O/Bra86.o: ../../../../C/Bra86.c
	$(CC) $(CFLAGS) $<
$O/BraIA64.o: ../../../../C/BraIA64.c
	$(CC) $(CFLAGS) $<
$O/BwtSort.o: ../../../../C/BwtSort.c
	$(CC) $(CFLAGS) $<

$O/CpuArch.o: ../../../../C/CpuArch.c
	$(CC) $(CFLAGS) $<
$O/Delta.o: ../../../../C/Delta.c
	$(CC) $(CFLAGS) $<
$O/DllSecur.o: ../../../../C/DllSecur.c
	$(CC) $(CFLAGS) $<
$O/HuffEnc.o: ../../../../C/HuffEnc.c
	$(CC) $(CFLAGS) $<
$O/LzFind.o: ../../../../C/LzFind.c
	$(CC) $(CFLAGS) $<

# ifdef MT_FILES
$O/LzFindMt.o: ../../../../C/LzFindMt.c
	$(CC) $(CFLAGS) $<

$O/Threads.o: ../../../../C/Threads.c
	$(CC) $(CFLAGS) $<
# endif

$O/LzmaEnc.o: ../../../../C/LzmaEnc.c
	$(CC) $(CFLAGS) $<
$O/Lzma86Dec.o: ../../../../C/Lzma86Dec.c
	$(CC) $(CFLAGS) $<
$O/Lzma86Enc.o: ../../../../C/Lzma86Enc.c
	$(CC) $(CFLAGS) $<
$O/Lzma2Dec.o: ../../../../C/Lzma2Dec.c
	$(CC) $(CFLAGS) $<
$O/Lzma2DecMt.o: ../../../../C/Lzma2DecMt.c
	$(CC) $(CFLAGS) $<
$O/Lzma2Enc.o: ../../../../C/Lzma2Enc.c
	$(CC) $(CFLAGS) $<
$O/LzmaLib.o: ../../../../C/LzmaLib.c
	$(CC) $(CFLAGS) $<
$O/MtCoder.o: ../../../../C/MtCoder.c
	$(CC) $(CFLAGS) $<
$O/MtDec.o: ../../../../C/MtDec.c
	$(CC) $(CFLAGS) $<
$O/Ppmd7.o: ../../../../C/Ppmd7.c
	$(CC) $(CFLAGS) $<
$O/Ppmd7aDec.o: ../../../../C/Ppmd7aDec.c
	$(CC) $(CFLAGS) $<
$O/Ppmd7Dec.o: ../../../../C/Ppmd7Dec.c
	$(CC) $(CFLAGS) $<
$O/Ppmd7Enc.o: ../../../../C/Ppmd7Enc.c
	$(CC) $(CFLAGS) $<
$O/Ppmd8.o: ../../../../C/Ppmd8.c
	$(CC) $(CFLAGS) $<
$O/Ppmd8Dec.o: ../../../../C/Ppmd8Dec.c
	$(CC) $(CFLAGS) $<
$O/Ppmd8Enc.o: ../../../../C/Ppmd8Enc.c
	$(CC) $(CFLAGS) $<
$O/Sha1.o: ../../../../C/Sha1.c
	$(CC) $(CFLAGS) $<
$O/Sha256.o: ../../../../C/Sha256.c
	$(CC) $(CFLAGS) $<
$O/Sort.o: ../../../../C/Sort.c
	$(CC) $(CFLAGS) $<
$O/Xz.o: ../../../../C/Xz.c
	$(CC) $(CFLAGS) $<
$O/XzCrc64.o: ../../../../C/XzCrc64.c
	$(CC) $(CFLAGS) $<

ifdef USE_ASM
ifdef IS_X64
USE_X86_ASM=1
USE_X64_ASM=1
else
ifdef IS_X86
USE_X86_ASM=1
endif
endif
endif

ifdef USE_X86_ASM
$O/7zCrcOpt.o: ../../../../Asm/x86/7zCrcOpt.asm
	$(MY_ASM) $(AFLAGS) $<
$O/XzCrc64Opt.o: ../../../../Asm/x86/XzCrc64Opt.asm
	$(MY_ASM) $(AFLAGS) $<
$O/Sha1Opt.o: ../../../../Asm/x86/Sha1Opt.asm
	$(MY_ASM) $(AFLAGS) $<
$O/Sha256Opt.o: ../../../../Asm/x86/Sha256Opt.asm
	$(MY_ASM) $(AFLAGS) $<

ifndef USE_JWASM
USE_X86_ASM_AES=1
endif

else
$O/7zCrcOpt.o: ../../../../C/7zCrcOpt.c
	$(CC) $(CFLAGS) $<
$O/XzCrc64Opt.o: ../../../../C/XzCrc64Opt.c
	$(CC) $(CFLAGS) $<
$O/Sha1Opt.o: ../../../../C/Sha1Opt.c
	$(CC) $(CFLAGS) $<
$O/Sha256Opt.o: ../../../../C/Sha256Opt.c
	$(CC) $(CFLAGS) $<
endif


ifdef USE_X86_ASM_AES
$O/AesOpt.o: ../../../../Asm/x86/AesOpt.asm
	$(MY_ASM) $(AFLAGS) $<
else
$O/AesOpt.o: ../../../../C/AesOpt.c
	$(CC) $(CFLAGS) $<
endif


ifdef USE_X64_ASM
$O/LzFindOpt.o: ../../../../Asm/x86/LzFindOpt.asm
	$(MY_ASM) $(AFLAGS) $<
else
$O/LzFindOpt.o: ../../../../C/LzFindOpt.c
	$(CC) $(CFLAGS) $<
endif

ifdef USE_LZMA_DEC_ASM

ifdef IS_X64
$O/LzmaDecOpt.o: ../../../../Asm/x86/LzmaDecOpt.asm
	$(MY_ASM) $(AFLAGS) $<
endif

ifdef IS_ARM64
$O/LzmaDecOpt.o: ../../../../Asm/arm64/LzmaDecOpt.S ../../../../Asm/arm64/7zAsm.S
	$(CC) $(CFLAGS) $<
endif

$O/LzmaDec.o: ../../../../C/LzmaDec.c
	$(CC) $(CFLAGS) -D_LZMA_DEC_OPT $<

else

$O/LzmaDec.o: ../../../../C/LzmaDec.c
	$(CC) $(CFLAGS) $<

endif



$O/XzDec.o: ../../../../C/XzDec.c
	$(CC) $(CFLAGS) $<
$O/XzEnc.o: ../../../../C/XzEnc.c
	$(CC) $(CFLAGS) $<
$O/XzIn.o: ../../../../C/XzIn.c
	$(CC) $(CFLAGS) $<


$O/7zMain.o: ../../../../C/Util/7z/7zMain.c
	$(CC) $(CFLAGS) $<
$O/LzmaUtil.o: ../../../../C/Util/Lzma/LzmaUtil.c
	$(CC) $(CFLAGS) $<

# Build HASH
$O/md2.o: ../../../../C/hashes/md2.c
	$(CC) $(CFLAGS) $<
$O/md4.o: ../../../../C/hashes/md4.c
	$(CC) $(CFLAGS) $<
$O/md5.o: ../../../../C/hashes/md5.c
	$(CC) $(CFLAGS) $<
$O/sha512.o: ../../../../C/hashes/sha512.c
	$(CC) $(CFLAGS) $<
$O/blake3.o: ../../../../C/hashes/blake3.c
	$(CC) $(CFLAGS) $<

# Build MT API 
$O/lz4-mt_common.o: ../../../../C/zstdmt/lz4-mt_common.c
	$(CC) $(CFLAGS) $<
$O/lz4-mt_compress.o: ../../../../C/zstdmt/lz4-mt_compress.c
	$(CC) $(CFLAGS) $<
$O/lz4-mt_decompress.o: ../../../../C/zstdmt/lz4-mt_decompress.c
	$(CC) $(CFLAGS) $<
$O/brotli-mt_common.o: ../../../../C/zstdmt/brotli-mt_common.c
	$(CC) $(CFLAGS) $< -I ../../../../C/brotli/c/include
$O/brotli-mt_compress.o: ../../../../C/zstdmt/brotli-mt_compress.c
	$(CC) $(CFLAGS) $< -I ../../../../C/brotli/c/include
$O/brotli-mt_decompress.o: ../../../../C/zstdmt/brotli-mt_decompress.c
	$(CC) $(CFLAGS) $< -I ../../../../C/brotli/c/include
$O/lizard-mt_common.o: ../../../../C/zstdmt/lizard-mt_common.c
	$(CC) $(CFLAGS) $<
$O/lizard-mt_compress.o: ../../../../C/zstdmt/lizard-mt_compress.c
	$(CC) $(CFLAGS) $<
$O/lizard-mt_decompress.o: ../../../../C/zstdmt/lizard-mt_decompress.c
	$(CC) $(CFLAGS) $<
$O/lz5-mt_common.o: ../../../../C/zstdmt/lz5-mt_common.c
	$(CC) $(CFLAGS) $<
$O/lz5-mt_compress.o: ../../../../C/zstdmt/lz5-mt_compress.c
	$(CC) $(CFLAGS) $<
$O/lz5-mt_decompress.o: ../../../../C/zstdmt/lz5-mt_decompress.c
	$(CC) $(CFLAGS) $<


# Build zstd lib static and dynamic
$O/libzstd.$(DYSUFFIX): ../../../../C/zstd/lib/zstd.h
	$(RM) zstd_build
	$(MY_MKDIR) -p zstd_build
	$(CD) zstd_build; \
	cmake ../../../../../C/zstd/build/cmake -DCMAKE_BUILD_TYPE=Release -DZSTD_BUILD_PROGRAMS=OFF -DZSTD_BUILD_STATIC=OFF -DCMAKE_C_COMPILER=$(CC) -DCMAKE_CXX_COMPILER=$(CXX); \
	$(MAKE) -j; \
	$(CD) ../; \
	$(CP) zstd_build/lib/libzstd.$(DYSUFFIX)* $O/$(7z_LIB)/$(7Z_ADDON_CODEC)

# Compile zstd method and Handler
$O/ZstdDecoder.o: ../../Compress/ZstdDecoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ZstdEncoder.o: ../../Compress/ZstdEncoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ZstdRegister.o: ../../Compress/ZstdRegister.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ZstdHandler.o: ../../Archive/ZstdHandler.cpp
	$(CXX) $(CXXFLAGS) $<

# Build lz4 lib static and dynamic
$O/liblz4.$(DYSUFFIX): ../../../../C/lz4/lib/lz4.h
	$(RM) lz4_build
	$(MY_MKDIR) -p lz4_build
	$(CD) lz4_build; \
	cmake ../../../../../C/lz4/build/cmake -DCMAKE_BUILD_TYPE=Release -DLZ4_BUILD_CLI=OFF -DLZ4_BUILD_LEGACY_LZ4C=OFF -DCMAKE_C_COMPILER=$(CC) -DCMAKE_CXX_COMPILER=$(CXX); \
	$(MAKE) -j; \
	$(CD) ../; \
	$(CP) lz4_build/liblz4.$(DYSUFFIX)* $O/$(7z_LIB)/$(7Z_ADDON_CODEC)

# Compile lz4 method and Handler
$O/Lz4Decoder.o: ../../Compress/Lz4Decoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Lz4Encoder.o: ../../Compress/Lz4Encoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Lz4Register.o: ../../Compress/Lz4Register.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Lz4Handler.o: ../../Archive/Lz4Handler.cpp
	$(CXX) $(CXXFLAGS) $<

# Build brotli lib static and dynamic
$O/libbrotlicommon.$(DYSUFFIX) $O/libbrotlienc.$(DYSUFFIX) $O/libbrotlidec.$(DYSUFFIX): ../../../../C/brotli/c/include/brotli/decode.h
	$(RM) brotli_build
	$(MY_MKDIR) -p brotli_build
	$(CD) brotli_build; \
	cmake ../../../../../C/brotli/ -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -DCMAKE_C_COMPILER=$(CC) -DCMAKE_CXX_COMPILER=$(CXX); \
	$(MAKE) -j; \
	$(CD) ../; \
	$(CP) brotli_build/libbrotlicommon.$(DYSUFFIX)* $O/$(7z_LIB)/$(7Z_ADDON_CODEC)
	$(CP) brotli_build/libbrotlidec.$(DYSUFFIX)* $O/$(7z_LIB)/$(7Z_ADDON_CODEC)
	$(CP) brotli_build/libbrotlienc.$(DYSUFFIX)* $O/$(7z_LIB)/$(7Z_ADDON_CODEC)
	$(SET_RPATH) '$$ORIGIN/' $O/$(7z_LIB)/$(7Z_ADDON_CODEC)/libbrotlidec.$(DYSUFFIX)*
	$(SET_RPATH) '$$ORIGIN/' $O/$(7z_LIB)/$(7Z_ADDON_CODEC)/libbrotlienc.$(DYSUFFIX)*

# Compile brotli method and Handler 
$O/BrotliDecoder.o: ../../Compress/BrotliDecoder.cpp
	$(CXX) $(CXXFLAGS) $< -I ../../../../C/brotli/c/include
$O/BrotliEncoder.o: ../../Compress/BrotliEncoder.cpp
	$(CXX) $(CXXFLAGS) $< -I ../../../../C/brotli/c/include
$O/BrotliRegister.o: ../../Compress/BrotliRegister.cpp
	$(CXX) $(CXXFLAGS) $< -I ../../../../C/brotli/c/include

# Build lizard lib static and dynamic
$O/liblizard.$(DYSUFFIX): ../../../../C/lizard/lib/lizard_frame.h
	$(MAKE) -C ../../../../C/lizard/lib
	$(CP) ../../../../C/lizard/lib/liblizard.$(DYSUFFIX)* $O/$(7z_LIB)/$(7Z_ADDON_CODEC)

# Compile lizard method and Handler
$O/LizardDecoder.o: ../../Compress/LizardDecoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/LizardEncoder.o: ../../Compress/LizardEncoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/LizardRegister.o: ../../Compress/LizardRegister.cpp
	$(CXX) $(CXXFLAGS) $<
$O/LizardHandler.o: ../../Archive/LizardHandler.cpp
	$(CXX) $(CXXFLAGS) $<

# Build lz5 lib static and dynamic
$O/liblz5.$(DYSUFFIX): ../../../../C/lz5/lib/lz5frame.h
	$(MAKE) -C ../../../../C/lz5/lib
	$(CP) ../../../../C/lz5/lib/liblz5.$(DYSUFFIX)* $O/$(7z_LIB)/$(7Z_ADDON_CODEC)

# Compile lz5 method and Handler
$O/Lz5Decoder.o: ../../Compress/Lz5Decoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Lz5Encoder.o: ../../Compress/Lz5Encoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Lz5Register.o: ../../Compress/Lz5Register.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Lz5Handler.o: ../../Archive/Lz5Handler.cpp
	$(CXX) $(CXXFLAGS) $<

# Build lzham lib static and dynamic
$O/liblzhamdll.$(DYSUFFIX) $O/liblzhamcomp.$(DYSUFFIX) $O/liblzhamdecomp.$(DYSUFFIX): ../../../../C/lzham_codec/lzhamdll/lzham_api.cpp
	$(RM) lzham_build
	$(MY_MKDIR) -p lzham_build
	$(CD) lzham_build; \
	cmake ../../../../../C/lzham_codec/ -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=$(CC) -DCMAKE_CXX_COMPILER=$(CXX); \
	$(MAKE) -j; \
	$(CD) ../; \
	$(CP) lzham_build/lzhamcomp/liblzhamcomp.$(DYSUFFIX)* $O/$(7z_LIB)/$(7Z_ADDON_CODEC)
	$(CP) lzham_build/lzhamdecomp/liblzhamdecomp.$(DYSUFFIX)* $O/$(7z_LIB)/$(7Z_ADDON_CODEC)
	$(CP) lzham_build/lzhamdll/liblzhamdll.$(DYSUFFIX)* $O/$(7z_LIB)/$(7Z_ADDON_CODEC)
	$(SET_RPATH) '$$ORIGIN/' $O/$(7z_LIB)/$(7Z_ADDON_CODEC)/liblzhamdll.$(DYSUFFIX)*

# Compile lzham register
$O/LzhamRegister.o: ../../Compress/LzhamRegister.cpp
	$(CXX) $(CXXFLAGS) $< -I ../../../../C/lzham_codec/include

# Compile FastLzma2 method
$O/libfast-lzma2.$(DYSUFFIX): ../../../../C/fast-lzma2/fast-lzma2.h
	$(MAKE) -C ../../../../C/fast-lzma2
	$(CP) ../../../../C/fast-lzma2/libfast-lzma2.$(DYSUFFIX)* $O/$(7z_LIB)/$(7Z_ADDON_CODEC)
	$(CD) $O/$(7z_LIB)/$(7Z_ADDON_CODEC); \
	ln -sf libfast-lzma2.$(DYSUFFIX).1.0 libfast-lzma2.$(DYSUFFIX).1; \
	ln -sf libfast-lzma2.$(DYSUFFIX).1 libfast-lzma2.$(DYSUFFIX); \
# only for 7zr
$O/libfast-lzma2.a: ../../../../C/fast-lzma2/fast-lzma2.h
	$(MAKE) -C ../../../../C/fast-lzma2
	$(CP) ../../../../C/fast-lzma2/libfast-lzma2.a $O

$O/FastLzma2Register.o: ../../Compress/FastLzma2Register.cpp
	$(CXX) $(CXXFLAGS) $<

ifneq ($(CC), xlc)
SHOW_PREDEF=-dM
else
SHOW_PREDEF= -qshowmacros=pre
endif

predef_cc:
	$(CC) $(CFLAGS) -E $(SHOW_PREDEF)  ../../../../C/CpuArch.c > predef_cc_log
#	$(CC) $(CFLAGS) -E -dM -  < /dev/null
predef_cxx:
	$(CXX) $(CFLAGS) -E $(SHOW_PREDEF) ../../../Common/CrcReg.cpp  > predef_cxx_log

predef: predef_cc predef_cxx

install:
	$(MY_MKDIR) $(INSTALL_PREFIX)/bin
	$(CP) $(O)/$(7z_BIN)/* $(INSTALL_PREFIX)/bin 2>/dev/null || :
	$(MY_MKDIR) $(INSTALL_PREFIX)/lib
	$(CP) $(O)/$(7z_LIB)/* $(INSTALL_PREFIX)/lib

uninstall:
	$(RM) $(INSTALL_PREFIX)/bin/7z*
	$(RM) $(INSTALL_PREFIX)/lib/7z*

clean:
	-$(DEL_OBJ_EXE)
	$(RM) zstd_build
	$(RM) lz4_build
	$(RM) brotli_build
	$(RM) lzham_build
	$(RM) b
	$(MAKE) -C ../../../../C/lizard/lib clean
	$(MAKE) -C ../../../../C/lz5/lib clean
