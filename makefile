
include makefile.common

test_lib: common
	$(MAKE) -C CPP/myWindows              all

other: common
	$(MAKE) -C Utils/CPUTest/MemLat       all
	$(MAKE) -C Utils/CPUTest/PipeLen      all
	$(MAKE) -C CPP/7zip/UI/P7ZIP          all
	$(MAKE) -C CPP/7zip/TEST/TestUI       all

other2:
	$(MAKE) -C CPP/7zip/Bundles/AloneGCOV all

7za: common
	$(MAKE) -C CPP/7zip/Bundles/Alone all

7zr: common
	$(MAKE) -C CPP/7zip/Bundles/Alone7z all

Client7z: common
	$(MKDIR) bin/Codecs
	$(MAKE) -C CPP/7zip/Bundles/Format7zFree all
	$(MAKE) -C CPP/7zip/UI/Client7z          all

depend:
	$(MAKE) -C CPP/7zip/Bundles/Alone         depend
	$(MAKE) -C CPP/7zip/Bundles/Alone7z       depend
	$(MAKE) -C CPP/7zip/Bundles/SFXCon        depend
	$(MAKE) -C CPP/7zip/UI/Client7z           depend
	$(MAKE) -C CPP/7zip/UI/Console            depend
	$(MAKE) -C CPP/7zip/Bundles/Format7zFree  depend
	$(MAKE) -C CPP/7zip/Compress/Rar          depend
	$(MAKE) -C CPP/7zip/UI/GUI                depend
	$(MAKE) -C CPP/7zip/UI/FileManager        depend

sfx: common
	$(MKDIR) bin
	$(MAKE) -C CPP/7zip/Bundles/SFXCon  all

common7z:common
	$(MKDIR) bin/Codecs
	$(MAKE) -C CPP/7zip/Bundles/Format7zFree all
	$(MAKE) -C CPP/7zip/Compress/Rar         all

lzham:common
	$(MKDIR) bin/Codecs
	$(MAKE) -C CPP/7zip/Compress/Lzham all

7z: common7z
	$(MAKE) -C CPP/7zip/UI/Console           all

7zG: common7z LangAndHelp
	$(MAKE) -C CPP/7zip/UI/GUI               all

7zFM: common7z LangAndHelp
	$(MAKE) -C CPP/7zip/UI/FileManager       all

clean_C:
	$(MAKE) -C CPP/myWindows                 clean
	$(MAKE) -C CPP/7zip/Bundles/Alone        clean
	$(MAKE) -C CPP/7zip/Bundles/Alone7z      clean
	$(MAKE) -C CPP/7zip/Bundles/SFXCon       clean
	$(MAKE) -C CPP/7zip/UI/Client7z          clean
	$(MAKE) -C CPP/7zip/UI/Console           clean
	$(MAKE) -C CPP/7zip/UI/FileManager       clean
	$(MAKE) -C CPP/7zip/UI/GUI               clean
	$(MAKE) -C CPP/7zip/Bundles/Format7zFree clean
	$(MAKE) -C CPP/7zip/Compress/Rar         clean
	$(MAKE) -C CPP/7zip/Compress/Lzham       clean
	$(MAKE) -C CPP/7zip/Bundles/LzmaCon      clean2
	$(MAKE) -C CPP/7zip/Bundles/AloneGCOV    clean
	$(MAKE) -C CPP/7zip/TEST/TestUI          clean
	$(MAKE) -C CPP/ANDROID                   clean
	$(MAKE) -C Utils/CPUTest/MemLat          clean
	$(MAKE) -C Utils/CPUTest/PipeLen         clean
	$(MAKE) -C CPP/7zip/UI/P7ZIP             clean

LzmaCon:
	$(MAKE) -C CPP/7zip/Bundles/LzmaCon

test_LzmaCon:
	$(MAKE) -C CPP/7zip/Bundles/LzmaCon  test

