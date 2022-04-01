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
$O/libzstd.a: ../../../../C/zstd/lib/zstd.h
	$(RM) zstd_build
	$(MY_MKDIR) -p zstd_build
	cd zstd_build; \
	cmake ../../../../../C/zstd/build/cmake; \
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

# Build lz4 lib static and dynamic
$O/liblz4.a: ../../../../C/lz4/lib/lz4.h
	$(RM) lz4_build
	$(MY_MKDIR) -p lz4_build
	cd lz4_build; \
	cmake -DBUILD_STATIC_LIBS=ON ../../../../../C/lz4/build/cmake; \
	make -j; \
	cd ..; \
	cp lz4_build/liblz4.a $O

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
$O/libbrotlicommon-static.a: ../../../../C/brotli/c/include/brotli/decode.h
	$(RM) brotli_build
	$(MY_MKDIR) -p brotli_build
	cd brotli_build; \
	cmake ../../../../../C/brotli/; \
	make -j; \
	cd ..; \
	cp brotli_build/libbrotlicommon-static.a $O
	cp brotli_build/libbrotlidec-static.a $O
	cp brotli_build/libbrotlienc-static.a $O

# Compile brotli method and Handler 
$O/BrotliDecoder.o: ../../Compress/BrotliDecoder.cpp
	$(CXX) $(CXXFLAGS) $< -I ../../../../C/brotli/c/include
$O/BrotliEncoder.o: ../../Compress/BrotliEncoder.cpp
	$(CXX) $(CXXFLAGS) $< -I ../../../../C/brotli/c/include
$O/BrotliRegister.o: ../../Compress/BrotliRegister.cpp
	$(CXX) $(CXXFLAGS) $< -I ../../../../C/brotli/c/include

# Build lizard lib static and dynamic
$O/liblizard.a: ../../../../C/lizard/lib/lizard_frame.h
	make -C ../../../../C/lizard/lib
	cp ../../../../C/lizard/lib/liblizard.a $O

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
$O/liblz5.a: ../../../../C/lz5/lib/lz5frame.h
	make -C ../../../../C/lz5/lib
	cp ../../../../C/lz5/lib/liblz5.a $O

# Compile lz5 method and Handler
$O/Lz5Decoder.o: ../../Compress/Lz5Decoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Lz5Encoder.o: ../../Compress/Lz5Encoder.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Lz5Register.o: ../../Compress/Lz5Register.cpp
	$(CXX) $(CXXFLAGS) $<
$O/Lz5Handler.o: ../../Archive/Lz5Handler.cpp
	$(CXX) $(CXXFLAGS) $<

clean2:
	$(RM) zstd_build
	$(RM) lz4_build
	$(RM) brotli_build
	make -C ../../../../C/lizard/lib clean
	make -C ../../../../C/lz5/lib clean
