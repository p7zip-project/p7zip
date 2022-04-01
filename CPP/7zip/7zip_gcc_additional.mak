# Build MT API 
$O/lz4-mt_common.o: ../../../../Codecs/zstdmt/lib/lz4-mt_common.c
	$(CC) $(CFLAGS) $< -I ../../../../Codecs/lz4/lib
$O/lz4-mt_compress.o: ../../../../Codecs/zstdmt/lib/lz4-mt_compress.c
	$(CC) $(CFLAGS) $< -I ../../../../Codecs/lz4/lib
$O/lz4-mt_decompress.o: ../../../../Codecs/zstdmt/lib/lz4-mt_decompress.c
	$(CC) $(CFLAGS) $< -I ../../../../Codecs/lz4/lib
$O/brotli-mt_common.o: ../../../../Codecs/zstdmt/lib/brotli-mt_common.c
	$(CC) $(CFLAGS) $< -I ../../../../Codecs/brotli/c/include
$O/brotli-mt_compress.o: ../../../../Codecs/zstdmt/lib/brotli-mt_compress.c
	$(CC) $(CFLAGS) $< -I ../../../../Codecs/brotli/c/include
$O/brotli-mt_decompress.o: ../../../../Codecs/zstdmt/lib/brotli-mt_decompress.c
	$(CC) $(CFLAGS) $< -I ../../../../Codecs/brotli/c/include
$O/lizard-mt_common.o: ../../../../Codecs/zstdmt/lib/lizard-mt_common.c
	$(CC) $(CFLAGS) $< -I ../../../../Codecs/lizard/lib
$O/lizard-mt_compress.o: ../../../../Codecs/zstdmt/lib/lizard-mt_compress.c
	$(CC) $(CFLAGS) $< -I ../../../../Codecs/lizard/lib
$O/lizard-mt_decompress.o: ../../../../Codecs/zstdmt/lib/lizard-mt_decompress.c
	$(CC) $(CFLAGS) $< -I ../../../../Codecs/lizard/lib
$O/lz5-mt_common.o: ../../../../Codecs/zstdmt/lib/lz5-mt_common.c
	$(CC) $(CFLAGS) $< -I ../../../../Codecs/lz5/lib
$O/lz5-mt_compress.o: ../../../../Codecs/zstdmt/lib/lz5-mt_compress.c
	$(CC) $(CFLAGS) $< -I ../../../../Codecs/lz5/lib
$O/lz5-mt_decompress.o: ../../../../Codecs/zstdmt/lib/lz5-mt_decompress.c
	$(CC) $(CFLAGS) $< -I ../../../../Codecs/lz5/lib

# Build zstd lib static
$O/libzstd.a: ../../../../Codecs/zstd/lib/zstd.h
	$(RM) zstd_build
	$(MY_MKDIR) -p zstd_build
	cd zstd_build; \
	cmake -DZSTD_BUILD_STATIC=ON -DZSTD_BUILD_SHARED=OFF -DZSTD_BUILD_PROGRAMS=OFF ../../../../../Codecs/zstd/build/cmake; \
	make -j; \
	cd ..; \
	cp zstd_build/lib/libzstd.a $O

# Compile zstd method and Handler
$O/ZstdDecoder.o: ../../Compress/ZstdDecoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ZstdEncoder.o: ../../Compress/ZstdEncoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ZstdRegister.o: ../../Compress/ZstdRegister.cpp
	$(CXX) $(CXXFLAGS) $<
$O/ZstdHandler.o: ../../Archive/ZstdHandler.cpp
	$(CXX) $(CXXFLAGS) $<

# Build lz4 lib static
$O/liblz4.a: ../../../../Codecs/lz4/lib/lz4.h
	$(RM) lz4_build
	$(MY_MKDIR) -p lz4_build
	cd lz4_build; \
	cmake -DBUILD_STATIC_LIBS=ON -DBUILD_SHARED_LIBS=OFF -DLZ4_BUILD_CLI=OFF -DLZ4_BUILD_LEGACY_LZ4C=OFF ../../../../../Codecs/lz4/build/cmake; \
	make -j; \
	cd ..; \
	cp lz4_build/liblz4.a $O

$O/lz4.o: ../../../../Codecs/lz4/lib/lz4.c
	$(CC) $(CFLAGS) $<
$O/lz4hc.o: ../../../../Codecs/lz4/lib/lz4hc.c
	$(CC) $(CFLAGS) $<
$O/lz4frame.o: ../../../../Codecs/lz4/lib/lz4frame.c
	$(CC) $(CFLAGS) $<
$O/xxhash.o: ../../../../Codecs/lz4/lib/xxhash.c
	$(CC) $(CFLAGS) $<

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
$O/libbrotlienc-static.a $O/libbrotlidec-static.a: $O/libbrotlicommon-static.a
$O/libbrotlicommon-static.a: ../../../../Codecs/brotli/c/include/brotli/decode.h
	$(RM) brotli_build
	$(MY_MKDIR) -p brotli_build
	cd brotli_build; \
	cmake ../../../../../Codecs/brotli/; \
	make -j; \
	cd ..; \
	cp brotli_build/libbrotlicommon-static.a $O
	cp brotli_build/libbrotlidec-static.a $O
	cp brotli_build/libbrotlienc-static.a $O

# Compile brotli method and Handler 
$O/BrotliDecoder.o: ../../Compress/BrotliDecoder.cpp
	$(CXX) $(CXXFLAGS) $< -I ../../../../Codecs/brotli/c/include
$O/BrotliEncoder.o: ../../Compress/BrotliEncoder.cpp
	$(CXX) $(CXXFLAGS) $< -I ../../../../Codecs/brotli/c/include
$O/BrotliRegister.o: ../../Compress/BrotliRegister.cpp
	$(CXX) $(CXXFLAGS) $< -I ../../../../Codecs/brotli/c/include

# Build lizard lib static
$O/liblizard.a: ../../../../Codecs/lizard/lib/lizard_frame.h
	make -C ../../../../Codecs/lizard/lib liblizard.a
	cp ../../../../Codecs/lizard/lib/liblizard.a $O

# Compile lizard method and Handler
$O/LizardDecoder.o: ../../Compress/LizardDecoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/LizardEncoder.o: ../../Compress/LizardEncoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/LizardRegister.o: ../../Compress/LizardRegister.cpp
	$(CXX) $(CXXFLAGS) $<
$O/LizardHandler.o: ../../Archive/LizardHandler.cpp
	$(CXX) $(CXXFLAGS) $<

# Build lz5 lib static
$O/lz5.o: ../../../../Codecs/lz5/lib/lz5.c
	$(CC) $(CFLAGS) $<
$O/lz5hc.o: ../../../../Codecs/lz5/lib/lz5hc.c
	$(CC) $(CFLAGS) $<
$O/lz5frame.o: ../../../../Codecs/lz5/lib/lz5frame.c
	$(CC) $(CFLAGS) $< -Wno-implicit-fallthrough

# Compile lz5 method and Handler
$O/Lz5Decoder.o: ../../Compress/Lz5Decoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Lz5Encoder.o: ../../Compress/Lz5Encoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Lz5Register.o: ../../Compress/Lz5Register.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Lz5Handler.o: ../../Archive/Lz5Handler.cpp
	$(CXX) $(CXXFLAGS) $<

# Build lzham lib static
$O/liblzhamdecomp.a: $O/liblzhamcomp.a
$O/liblzhamcomp.a: ../../../../Codecs/lzham_codec_devel/lzhamcomp/lzham_comp.h
	$(RM) lzham_build
	$(MY_MKDIR) -p lzham_build
	cd lzham_build; \
	cmake -DBUILD_X64=ON -DBUILD_SHARED_LIBS=OFF ../../../../../Codecs/lzham_codec_devel; \
	make -j lzhamcomp lzhamdecomp; \
	cd ..; \
	cp lzham_build/lzhamcomp/liblzhamcomp.a $O
	cp lzham_build/lzhamdecomp/liblzhamdecomp.a $O

$O/lzham_lib.o: ../../../../Codecs/lzham_codec_devel/lzhamlib/lzham_lib.cpp
	$(CXX) $(CXXFLAGS) $< -I ../../../../Codecs/lzham_codec_devel/include -I ../../../../Codecs/lzham_codec_devel/lzhamcomp -I ../../../../Codecs/lzham_codec_devel/lzhamdecomp

# Compile lzham method and Handler 
$O/LzhamRegister.o: ../../Compress/LzhamRegister.cpp
	$(CXX) $(CXXFLAGS) $< -Wno-reorder -Wno-unused-parameter -Wno-unused-variable -I ../../../.. -I ../../../../CPP -I ../../../../Codecs/lzham_codec_devel/include -I ../../../../Codecs/lzham_codec_devel/lzhamdecomp

# Build pkimplode lib
$O/explode.o: ../../../../Codecs/StormLib/src/pklib/explode.c
	$(CC) $(CFLAGS) $<
$O/implode.o: ../../../../Codecs/StormLib/src/pklib/implode.c
	$(CC) $(CFLAGS) $< -Wno-implicit-fallthrough

# Compile pkimplode method
$O/PKImplodeDecoder.o: ../../Compress/PKImplodeDecoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/PKImplodeEncoder.o: ../../Compress/PKImplodeEncoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/PKImplodeRegister.o: ../../Compress/PKImplodeRegister.cpp
	$(CXX) $(CXXFLAGS) $<

# Build hashes lib
$O/md2.o: ../../../../Codecs/hashes/md2.c
	$(CC) $(CFLAGS) $<
$O/md4.o: ../../../../Codecs/hashes/md4.c
	$(CC) $(CFLAGS) $<
$O/md5.o: ../../../../Codecs/hashes/md5.c
	$(CC) $(CFLAGS) $<
$O/sha512.o: ../../../../Codecs/hashes/sha512.c
	$(CC) $(CFLAGS) $<

# Compile hashes method 
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

clean2:
	$(RM) zstd_build
	# $(RM) lz4_build
	$(RM) brotli_build
	$(RM) lzham_build
	make -C ../../../../Codecs/lizard/lib clean
	# make -C ../../../../Codecs/lz5/lib clean
