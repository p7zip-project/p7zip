// File: lzhamtest.cpp
// This is a small app primarily designed to test the codec. Sorry, it doesn't really make the best sample, it's just a test harness.
// I also test LZHAM in a private branch of 7zip, mostly to ensure the streaming API is solid. Please contact me if you would like this branch.
// See the decompress_file() function to see how to use the decompression API, and the compress_file() function for the compression API.
// See include/lzham.h for documentation on the public LZHAM API.
// Tested on Windows, Linux, and OSX. On iOS, I use a small "Hello World" test app to test the codec.
// See Copyright Notice and license at the end of include/lzham.h
#if defined(__GNUC__)
#define _FILE_OFFSET_BITS 64
#endif

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <memory.h>
#include <stdarg.h>
#if !defined(__APPLE__) && !defined(__FreeBSD__)
#include <malloc.h>
#endif
#include <vector>
#include <string>

#include "timer.h"

#define my_max(a,b) (((a) > (b)) ? (a) : (b))
#define my_min(a,b) (((a) < (b)) ? (a) : (b))

#define LZHAM_PRINT_OUTPUT_PROGRESS

// Note: lzham can be used as static libs, or as a DLL. The default (out of the box) configuration under Win32 loads the DLL.
// To test lzham as a static library under Win32, set LZHAM_USE_LZHAM_DLL to 0, add ../lzhamcomp and ../lzhamdecomp to the additional inc paths, and link with the lzhamcomp and lzhamdecomp libs.
// On non-Win32 platforms, this test app currently assumes static libs.
#ifdef _XBOX
   #include <xtl.h>
   #include <xbdm.h>
   #define LZHAM_USE_LZHAM_DLL 1
#elif defined(WIN32)
   #define WIN32_LEAN_AND_MEAN
   #include <windows.h>
   #define LZHAM_USE_LZHAM_DLL 1
#elif defined(__APPLE__)
   #include <unistd.h>
   #define Sleep(ms) usleep(ms*1000)
   #define _aligned_malloc(size, alignment) malloc(size)
   #define _aligned_free free
   #define fopen fopen
   #define _fseeki64 fseek
   #define _ftelli64 ftell
#elif defined(__FreeBSD__)
   #include <unistd.h>
   #define Sleep(ms) usleep(ms*1000)
   #define _aligned_malloc(size, alignment) aligned_alloc(alignment, size)
   #define _aligned_free free
   #define fopen fopen
   #define _fseeki64 fseeko
   #define _ftelli64 ftello
#else
   #include <unistd.h>
   #define Sleep(ms) usleep(ms*1000)
   #define _aligned_malloc(size, alignment) memalign(alignment, size)
   #define _aligned_free free
   #define fopen fopen64
   #define _fseeki64 fseeko64
   #define _ftelli64 ftello64
#endif

#ifdef WIN32
   #define QUAD_INT_FMT "%I64u"
#else
   #define QUAD_INT_FMT "%llu"
#endif

#if LZHAM_USE_LZHAM_DLL
   #include "lzham_dynamic_lib.h"
#else
   #include "lzham_static_lib.h"
#endif

#ifdef _DEBUG
const bool g_is_debug = true;
#else
const bool g_is_debug = false;
#endif

typedef unsigned char uint8;
typedef unsigned int uint;
typedef unsigned int uint32;

#ifdef __GNUC__
   typedef unsigned long long    uint64;
   typedef long long             int64;
#else
   typedef unsigned __int64      uint64;
   typedef signed __int64        int64;
#endif

#ifdef LZHAM_64BIT
   #define LZHAMTEST_MAX_POSSIBLE_DICT_SIZE LZHAM_MAX_DICT_SIZE_LOG2_X64
   // 256MB default dictionary size under x64 (max is 512MB, but this requires more than 4GB of physical memory without thrashing)
   #define LZHAMTEST_DEFAULT_DICT_SIZE 28
#else
   #define LZHAMTEST_MAX_POSSIBLE_DICT_SIZE LZHAM_MAX_DICT_SIZE_LOG2_X86
   // 64MB default dictionary size under x86
   #define LZHAMTEST_DEFAULT_DICT_SIZE LZHAM_MAX_DICT_SIZE_LOG2_X86
#endif

#if 1
   #define LZHAMTEST_COMP_INPUT_BUFFER_SIZE 65536*4
   #define LZHAMTEST_COMP_OUTPUT_BUFFER_SIZE 65536*4
   #define LZHAMTEST_DECOMP_INPUT_BUFFER_SIZE 65536*4
   #define LZHAMTEST_DECOMP_OUTPUT_BUFFER_SIZE 65536*4
#else
   #define LZHAMTEST_COMP_INPUT_BUFFER_SIZE 1
   #define LZHAMTEST_COMP_OUTPUT_BUFFER_SIZE 1
   #define LZHAMTEST_DECOMP_INPUT_BUFFER_SIZE 1
   #define LZHAMTEST_DECOMP_OUTPUT_BUFFER_SIZE 1
#endif

#define LZHAMTEST_NO_RANDOM_EXTREME_PARSING 1

struct comp_options
{
   comp_options() :
      m_comp_level(LZHAM_COMP_LEVEL_UBER),
      m_dict_size_log2(LZHAMTEST_DEFAULT_DICT_SIZE),
      m_compute_adler32_during_decomp(true),
      m_max_helper_threads(0),
      m_unbuffered_decompression(false),
      m_verify_compressed_data(false),
      m_randomize_params(false),
      m_extreme_parsing(false),
      m_deterministic_parsing(false),
      m_tradeoff_decomp_rate_for_comp_ratio(false),
      m_test_compressor_reinit(false),
		m_table_update_rate(LZHAM_DEFAULT_TABLE_UPDATE_RATE)
   {
   }

   void print()
   {
      printf("Comp level: %u\n", m_comp_level);
      printf("Dict size: %i (%u bytes)\n", m_dict_size_log2, 1 << m_dict_size_log2);
      printf("Compute adler32 during decompression: %u\n", (uint)m_compute_adler32_during_decomp);
      printf("Max helper threads: %i\n", m_max_helper_threads);
      printf("Unbuffered decompression: %u\n", (uint)m_unbuffered_decompression);
      printf("Verify compressed data: %u\n", (uint)m_verify_compressed_data);
      printf("Extreme parsing: %u\n", (uint)m_extreme_parsing);
      printf("Randomize parameters: %u\n", m_randomize_params);
      printf("Deterministic parsing: %u\n", m_deterministic_parsing);
      printf("Trade off decompression rate for compression ratio: %u\n", m_tradeoff_decomp_rate_for_comp_ratio);
      printf("Test compressor reinit: %u\n", m_test_compressor_reinit);
		printf("Table update speed: %u\n", m_table_update_rate);
   }

   lzham_compress_level m_comp_level;
   int m_dict_size_log2;
   bool m_compute_adler32_during_decomp;
   int m_max_helper_threads;           // -1 = try to auto-detect
   bool m_unbuffered_decompression;
   bool m_verify_compressed_data;
   bool m_randomize_params;
   bool m_extreme_parsing;
   bool m_deterministic_parsing;
   bool m_tradeoff_decomp_rate_for_comp_ratio;
   bool m_test_compressor_reinit;
	uint m_table_update_rate;
};

static void print_usage()
{
   printf("Usage: [options] [mode] inpath/infile [outfile]\n");
   printf("\n");
   printf("Modes:\n");
   printf("c - Compress \"infile\" to \"outfile\"\n");
   printf("d - Decompress \"infile\" to \"outfile\"\n");
   printf("a - Recursively compress all files under \"inpath\"\n");
   printf("\n");
   printf("Options:\n");
   printf("-m[0-4] - Compression level: 0=fastest, 1=faster, 2=default, 3=better, 4=uber\n");
   printf("          Default is uber (4).\n");
   printf("-d[15-29] - Set log2 dictionary size, max. is 26 on x86 platforms, 29 on x64.\n");
   printf("          Default is 26 (64MB) on x86, 28 (256MB) on x64.\n");
   printf("-c - Do not compute or verify adler32 checksum during decompression (faster).\n");
   printf("-u - Use unbuffered decompression on files that can fit into memory.\n");
   printf("     Unbuffered decompression is faster, but may have more I/O overhead.\n");
   printf("-t[0-64] - Number of extra compression helper threads. Default=# CPU's-1.\n");
   printf("           Note: The total number of threads will be 1 + num_helper_threads,\n");
   printf("           because the main thread is counted separately.\n");
   printf("-v - Immediately decompress compressed file after compression for verification.\n");
   printf("-x - Extreme parsing, for slight compression gain (Uber only, MUCH slower).\n");
   printf("-o - Permit the compressor to trade off decompression rate for higher ratios.\n");
   printf("     Note: This flag can drop the decompression rate by 30%% or more.\n");
   printf("-e - Enable deterministic parsing for slightly higher compression and\n");
   printf("     predictable output files when enabled, but less scalability.\n");
   printf("     The default is disabled, so the generated output data may slightly vary\n");
   printf("     between runs when multithreaded compression is enabled.\n");
   printf("-afilename Enable delta compression using the specified seed file.\n");
   printf("           The same seed file MUST be used for compression/decompression.\n");
   printf("-r - Use randomized parameters for each file.\n");
	printf("-h[0-%u] - Set Huffman table update frequency. 0=Internal def, Def=%u, higher=faster.\n", LZHAM_FASTEST_TABLE_UPDATE_RATE, LZHAM_DEFAULT_TABLE_UPDATE_RATE);
	printf(" Lower settings=slower decompression, but higher ratio. Note 1=impractically slow.\n");
}

static void print_error(const char *pMsg, ...)
{
   char buf[1024];

   va_list args;
   va_start(args, pMsg);
   vsnprintf(buf, sizeof(buf), pMsg, args);
   va_end(args);

   buf[sizeof(buf) - 1] = '\0';

   fprintf(stderr, "Error: %s", buf);
}

static FILE* open_file_with_retries(const char *pFilename, const char* pMode)
{
   const uint cNumRetries = 8;
   for (uint i = 0; i < cNumRetries; i++)
   {
      FILE* pFile = fopen(pFilename, pMode);
      if (pFile)
         return pFile;
      Sleep(250);
   }
   return NULL;
}

static bool ensure_file_is_writable(const char *pFilename)
{
   const int cNumRetries = 8;
   for (int i = 0; i < cNumRetries; i++)
   {
      FILE *pFile = fopen(pFilename, "wb");
      if (pFile)
      {
         fclose(pFile);
         return true;
      }
      Sleep(250);
   }
   return false;
}

static int simple_test(ilzham &lzham_dll, const comp_options &options)
{
   printf("\n");
   printf("LZHAM simple memory to memory compression test\n");

   lzham_compress_params comp_params;
   memset(&comp_params, 0, sizeof(comp_params));
   comp_params.m_struct_size = sizeof(comp_params);
   comp_params.m_dict_size_log2 = options.m_dict_size_log2;
   comp_params.m_level = options.m_comp_level;
   comp_params.m_max_helper_threads = 1;

   lzham_uint8 cmp_buf[1024];
   size_t cmp_len = sizeof(cmp_buf);

   const char *p = "This is a test.This is a test.This is a test.1234567This is a test.This is a test.123456";
   size_t uncomp_len = strlen(p);

   lzham_uint32 comp_adler32 = 0;
   lzham_compress_status_t comp_status = lzham_dll.lzham_compress_memory(&comp_params, cmp_buf, &cmp_len, (const lzham_uint8 *)p, uncomp_len, &comp_adler32);
   if (comp_status != LZHAM_COMP_STATUS_SUCCESS)
   {
      print_error("Compression test failed with status %i!\n", comp_status);
      return EXIT_FAILURE;
   }

   printf("Uncompressed size: %u\nCompressed size: %u\n", (uint)uncomp_len, (uint)cmp_len);

   lzham_decompress_params decomp_params;
   memset(&decomp_params, 0, sizeof(decomp_params));
   decomp_params.m_struct_size = sizeof(decomp_params);
   decomp_params.m_dict_size_log2 = options.m_dict_size_log2;
   if (options.m_compute_adler32_during_decomp)
      decomp_params.m_decompress_flags |= LZHAM_DECOMP_FLAG_COMPUTE_ADLER32;

   lzham_uint8 decomp_buf[1024];
   size_t decomp_size = sizeof(decomp_buf);
   lzham_uint32 decomp_adler32 = 0;
   lzham_decompress_status_t decomp_status = lzham_dll.lzham_decompress_memory(&decomp_params, decomp_buf, &decomp_size, cmp_buf, cmp_len, &decomp_adler32);
   if (decomp_status != LZHAM_DECOMP_STATUS_SUCCESS)
   {
      print_error("Compression test failed with status %i!\n", decomp_status);
      return EXIT_FAILURE;
   }

   if ((comp_adler32 != decomp_adler32) || (decomp_size != uncomp_len) || (memcmp(decomp_buf, p, uncomp_len)))
   {
      print_error("Compression test failed!\n");
      return EXIT_FAILURE;
   }

   printf("Compression test succeeded.\n");

   return EXIT_SUCCESS;
}

static bool read_seed_file(const char *pSeed_filename, lzham_uint32 &num_seed_bytes, const void *&pSeed_bytes, uint dict_size_log2)
{
   num_seed_bytes = 0;
   pSeed_bytes = NULL;

   if (pSeed_filename)
   {
      FILE *pSeed_file = fopen(pSeed_filename, "rb");
      if (!pSeed_file)
      {
         print_error("Unable to open file: %s\n", pSeed_filename);
         return false;
      }

      _fseeki64(pSeed_file, 0, SEEK_END);
      uint64 seed_file_size = _ftelli64(pSeed_file);
      _fseeki64(pSeed_file, 0, SEEK_SET);

      lzham_uint32 seed_size = (lzham_uint32)my_min(seed_file_size, 1ULL << dict_size_log2);
      if (seed_size)
      {
         pSeed_bytes = _aligned_malloc(seed_size, 16);
         if (pSeed_bytes)
         {
            num_seed_bytes = (lzham_uint32)seed_size;

            if (fread((void*)pSeed_bytes, 1, seed_size, pSeed_file) != seed_size)
            {
               print_error("Failed reading file!\n");

               num_seed_bytes = 0;

               _aligned_free((void*)pSeed_bytes);
               pSeed_bytes = NULL;
               fclose(pSeed_file);
               return false;
            }
         }
      }

      fclose(pSeed_file);

      printf("Read File \"%s\", Size: %u bytes\n", pSeed_filename, seed_size);
   }

   return true;
}

static bool compress_file(ilzham &lzham_dll, const char* pSrc_filename, const char *pDst_filename, const comp_options &options, const char *pSeed_filename, float *pTotal_comp_time = NULL)
{
	if (pTotal_comp_time)
		*pTotal_comp_time = 0;

   printf("Testing: Streaming compression\n");

   FILE *pInFile = fopen(pSrc_filename, "rb");
   if (!pInFile)
   {
      print_error("Unable to read file: %s\n", pSrc_filename);
      return false;
   }

   FILE *pOutFile = fopen(pDst_filename, "wb");
   if (!pOutFile)
   {
      print_error("Unable to create file: %s\n", pDst_filename);
      return false;
   }

   _fseeki64(pInFile, 0, SEEK_END);
   uint64 src_file_size = _ftelli64(pInFile);
   _fseeki64(pInFile, 0, SEEK_SET);

   fputc('L', pOutFile);
   fputc('Z', pOutFile);
   fputc('H', pOutFile);
   fputc('0', pOutFile);
   fputc(options.m_dict_size_log2, pOutFile);

   for (uint i = 0; i < 8; i++)
   {
      fputc(static_cast<int>((src_file_size >> (i * 8)) & 0xFF), pOutFile);
   }

   uint64 cmp_file_header_size = _ftelli64(pOutFile);

   const uint cInBufSize = LZHAMTEST_COMP_INPUT_BUFFER_SIZE;
   const uint cOutBufSize = LZHAMTEST_COMP_OUTPUT_BUFFER_SIZE;

   uint8 *in_file_buf = static_cast<uint8*>(_aligned_malloc(cInBufSize, 16));
   uint8 *out_file_buf = static_cast<uint8*>(_aligned_malloc(cOutBufSize, 16));
   if ((!in_file_buf) || (!out_file_buf))
   {
      print_error("Out of memory!\n");
      _aligned_free(in_file_buf);
      _aligned_free(out_file_buf);
      fclose(pInFile);
      fclose(pOutFile);
      return false;
   }

   uint64 src_bytes_left = src_file_size;

   uint in_file_buf_size = 0;
   uint in_file_buf_ofs = 0;

   uint64 total_output_bytes = 0;

   timer_ticks start_time = timer::get_ticks();

   lzham_compress_params params;
   memset(&params, 0, sizeof(params));
   params.m_struct_size = sizeof(lzham_compress_params);
   params.m_dict_size_log2 = options.m_dict_size_log2;
   params.m_max_helper_threads = options.m_max_helper_threads;
   params.m_level = options.m_comp_level;
   if (options.m_extreme_parsing)
      params.m_compress_flags |= LZHAM_COMP_FLAG_EXTREME_PARSING;
   if (options.m_deterministic_parsing)
      params.m_compress_flags |= LZHAM_COMP_FLAG_DETERMINISTIC_PARSING;
   if (options.m_tradeoff_decomp_rate_for_comp_ratio)
      params.m_compress_flags |= LZHAM_COMP_FLAG_TRADEOFF_DECOMPRESSION_RATE_FOR_COMP_RATIO;
  
   params.m_table_update_rate = options.m_table_update_rate;
      
   if (pSeed_filename)
   {
      if (!read_seed_file(pSeed_filename, params.m_num_seed_bytes, params.m_pSeed_bytes, params.m_dict_size_log2))
      {
         _aligned_free(in_file_buf);
         _aligned_free(out_file_buf);
         fclose(pInFile);
         fclose(pOutFile);
         return false;
      }
   }

   timer_ticks init_start_time = timer::get_ticks();
   lzham_compress_state_ptr pComp_state = lzham_dll.lzham_compress_init(&params);
   timer_ticks total_init_time = timer::get_ticks() - init_start_time;

	float total_comp_time = (float)timer::ticks_to_secs(total_init_time);
	
   if ((pComp_state) && (options.m_test_compressor_reinit))
   {
      if (!lzham_dll.lzham_compress_reinit(pComp_state))
      {
         lzham_dll.lzham_compress_deinit(pComp_state);
         pComp_state = NULL;
      }
   }

   if (!pComp_state)
   {
      print_error("Failed initializing compressor!\n");
      _aligned_free(in_file_buf);
      _aligned_free(out_file_buf);
      fclose(pInFile);
      fclose(pOutFile);
      _aligned_free((void*)params.m_pSeed_bytes);
      return false;
   }
   
   printf("lzham_compress_init took %3.3fms\n", timer::ticks_to_secs(total_init_time)*1000.0f);

   lzham_compress_status_t status = LZHAM_COMP_STATUS_FAILED;

   // Performs 1 pass normally, or 2 passes to test compressor reinitialization (with a reinit in between the passes).
   uint total_passes = options.m_test_compressor_reinit ? 2 : 1;
   for (uint pass = 0; pass < total_passes; ++pass)
   {
      for ( ; ; )
      {
         if (src_file_size)
         {
            double total_elapsed_time = timer::ticks_to_secs(timer::get_ticks() - start_time);
            double total_bytes_processed = static_cast<double>(src_file_size - src_bytes_left);
            double comp_rate = (total_elapsed_time > 0.0f) ? total_bytes_processed / total_elapsed_time : 0.0f;

   #ifdef LZHAM_PRINT_OUTPUT_PROGRESS
            for (int i = 0; i < 15; i++)
               printf("\b\b\b\b");
            printf("Progress: %3.1f%%, Bytes Remaining: %3.1fMB, %3.3fMB/sec", (1.0f - (static_cast<float>(src_bytes_left) / src_file_size)) * 100.0f, src_bytes_left / 1048576.0f, comp_rate / (1024.0f * 1024.0f));
            printf("                \b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
   #endif
         }

         if (in_file_buf_ofs == in_file_buf_size)
         {
            in_file_buf_size = static_cast<uint>(my_min(cInBufSize, src_bytes_left));

            if (fread(in_file_buf, 1, in_file_buf_size, pInFile) != in_file_buf_size)
            {
               printf("\n");
               print_error("Failure reading from source file!\n");
               _aligned_free(in_file_buf);
               _aligned_free(out_file_buf);
               fclose(pInFile);
               fclose(pOutFile);
               _aligned_free((void*)params.m_pSeed_bytes);
               lzham_dll.lzham_compress_deinit(pComp_state);
               return false;
            }

            src_bytes_left -= in_file_buf_size;

            in_file_buf_ofs = 0;
         }

         uint8 *pIn_bytes = &in_file_buf[in_file_buf_ofs];
         size_t num_in_bytes = in_file_buf_size - in_file_buf_ofs;
         uint8* pOut_bytes = out_file_buf;
         size_t out_num_bytes = cOutBufSize;

			timer_ticks comp_start_time = timer::get_ticks();
         status = lzham_dll.lzham_compress(pComp_state, pIn_bytes, &num_in_bytes, pOut_bytes, &out_num_bytes, src_bytes_left == 0);
			timer_ticks comp_time = timer::get_ticks() - comp_start_time;

			total_comp_time += (float)timer::ticks_to_secs(comp_time);

         if (num_in_bytes)
         {
            in_file_buf_ofs += (uint)num_in_bytes;
            assert(in_file_buf_ofs <= in_file_buf_size);
         }

         if (out_num_bytes)
         {
            if (fwrite(out_file_buf, 1, static_cast<uint>(out_num_bytes), pOutFile) != out_num_bytes)
            {
               printf("\n");
               print_error("Failure writing to destination file!\n");
               _aligned_free(in_file_buf);
               _aligned_free(out_file_buf);
               fclose(pInFile);
               fclose(pOutFile);
               _aligned_free((void*)params.m_pSeed_bytes);
               lzham_dll.lzham_compress_deinit(pComp_state);
               return false;
            }

            total_output_bytes += out_num_bytes;
         }
         
         if (status >= LZHAM_COMP_STATUS_FIRST_SUCCESS_OR_FAILURE_CODE)
            break;
      }

#ifdef LZHAM_PRINT_OUTPUT_PROGRESS
      for (int i = 0; i < 15; i++)
      {
         printf("\b\b\b\b    \b\b\b\b");
      }
#endif

      if ((pass == 0) && (total_passes == 2))
      {
         printf("\n");
         
         uint64 cmp_file_size = _ftelli64(pOutFile);
         printf("Input file size: " QUAD_INT_FMT ", Compressed file size: " QUAD_INT_FMT ", Ratio: %3.2f%%\n", src_file_size, cmp_file_size, src_file_size ? ((1.0f - (static_cast<float>(cmp_file_size) / src_file_size)) * 100.0f) : 0.0f);

         init_start_time = timer::get_ticks();
         if (!lzham_dll.lzham_compress_reinit(pComp_state))
         {
            print_error("Failed reinitializing compressor!\n");
            _aligned_free(in_file_buf);
            _aligned_free(out_file_buf);
            fclose(pInFile);
            fclose(pOutFile);
            _aligned_free((void*)params.m_pSeed_bytes);
            lzham_dll.lzham_compress_deinit(pComp_state);
            return false;
         }
         total_init_time = timer::get_ticks() - init_start_time;
         printf("lzham_compress_reinit took %3.3fms\n", timer::ticks_to_secs(total_init_time)*1000.0f);

         fseek(pInFile, 0, SEEK_SET);
         fseek(pOutFile, static_cast<long>(cmp_file_header_size), SEEK_SET);
         
         src_bytes_left = src_file_size;
         in_file_buf_size = 0;
         in_file_buf_ofs = 0;
         total_output_bytes = 0;
      }
   }

   src_bytes_left += (in_file_buf_size - in_file_buf_ofs);
		
	timer_ticks deinit_start_time = timer::get_ticks();
   uint32 adler32 = lzham_dll.lzham_compress_deinit(pComp_state);
	timer_ticks total_deinit_time = timer::get_ticks() - deinit_start_time;
	
	total_comp_time += (float)timer::ticks_to_secs(total_deinit_time);
	if (pTotal_comp_time)
		*pTotal_comp_time = total_comp_time;

   pComp_state = NULL;

   timer_ticks end_time = timer::get_ticks();
   double total_time = timer::ticks_to_secs(my_max(1, end_time - start_time));

   uint64 cmp_file_size = _ftelli64(pOutFile);

   _aligned_free(in_file_buf);
   in_file_buf = NULL;
   _aligned_free(out_file_buf);
   out_file_buf = NULL;
   _aligned_free((void*)params.m_pSeed_bytes);
   params.m_pSeed_bytes = NULL;

   fclose(pInFile);
   pInFile = NULL;
   fclose(pOutFile);
   pOutFile = NULL;

   if (status != LZHAM_COMP_STATUS_SUCCESS)
   {
      print_error("Compression failed with status %i\n", status);
      return false;
   }

   if (src_bytes_left)
   {
      print_error("Compressor failed to consume entire input file!\n");
      return false;
   }

   printf("Success\n");
   printf("Input file size: " QUAD_INT_FMT ", Compressed file size: " QUAD_INT_FMT ", Ratio: %3.2f%%\n", src_file_size, cmp_file_size, src_file_size ? ((1.0f - (static_cast<float>(cmp_file_size) / src_file_size)) * 100.0f) : 0.0f);
	printf("Compression-only time: %3.6f\nConsumption rate: %9.1f bytes/sec, Emission rate: %9.1f bytes/sec\n", total_comp_time, src_file_size / total_comp_time, cmp_file_size / total_comp_time);
   printf("Total time: %3.6f\nConsumption rate: %9.1f bytes/sec, Emission rate: %9.1f bytes/sec\n", total_time, src_file_size / total_time, cmp_file_size / total_time);
   printf("Input file adler32: 0x%08X\n", adler32);
		
   return true;
}

static bool decompress_file(ilzham &lzham_dll, const char* pSrc_filename, const char *pDst_filename, comp_options options, const char *pSeed_filename, float *pTotal_decomp_time = NULL)
{
   FILE *pInFile = fopen(pSrc_filename, "rb");
   if (!pInFile)
   {
      print_error("Unable to read file: %s\n", pSrc_filename);
      return false;
   }

   _fseeki64(pInFile, 0, SEEK_END);
   uint64 src_file_size = _ftelli64(pInFile);
   _fseeki64(pInFile, 0, SEEK_SET);
   if (src_file_size < (5+9))
   {
      print_error("Compressed file is too small!\n");
      fclose(pInFile);
      return false;
   }

   int h0 = fgetc(pInFile);
   int h1 = fgetc(pInFile);
   int h2 = fgetc(pInFile);
   int h3 = fgetc(pInFile);
   int dict_size = fgetc(pInFile);
   if ((h0 != 'L') | (h1 != 'Z') || (h2 != 'H') || (h3 != '0'))
   {
      print_error("Unrecognized/invalid header in file: %s\n", pSrc_filename);
      fclose(pInFile);
      return false;
   }

   if ((dict_size < LZHAM_MIN_DICT_SIZE_LOG2) || (dict_size > LZHAM_MAX_DICT_SIZE_LOG2_X64))
   {
      print_error("Unrecognized/invalid header in file: %s\n", pSrc_filename);
      fclose(pInFile);
      return false;
   }

   FILE *pOutFile = fopen(pDst_filename, "wb");
   if (!pOutFile)
   {
      print_error("Unable to create file: %s\n", pDst_filename);
      fclose(pInFile);
      return false;
   }

   uint64 orig_file_size = 0;
   for (uint i = 0; i < 8; i++)
   {
      orig_file_size |= (static_cast<uint64>(fgetc(pInFile)) << (i * 8));
   }

   int total_header_bytes = static_cast<int>(ftell(pInFile));

   // Avoid running out of memory on large files when using unbuffered decompression.
#ifdef _XBOX
   if ((options.m_unbuffered_decompression) && (orig_file_size > 128*1024*1024))
#else
   if ((options.m_unbuffered_decompression) && (orig_file_size > 256*1024*1024))
#endif
   {
      printf("Output file is too large for unbuffered decompression - switching to streaming decompression.\n");
      options.m_unbuffered_decompression = false;
   }

   if (options.m_unbuffered_decompression)
      printf("Testing: Unbuffered decompression\n");
   else
      printf("Testing: Streaming decompression\n");

   const uint cInBufSize = LZHAMTEST_DECOMP_INPUT_BUFFER_SIZE;
   uint8 *in_file_buf = static_cast<uint8*>(_aligned_malloc(cInBufSize, 16));

   uint out_buf_size = options.m_unbuffered_decompression ? static_cast<uint>(orig_file_size) : LZHAMTEST_DECOMP_OUTPUT_BUFFER_SIZE;
   uint8 *out_file_buf = static_cast<uint8*>(_aligned_malloc(out_buf_size, 16));
   if (!out_file_buf)
   {
      print_error("Failed allocating output buffer!\n");
      _aligned_free(in_file_buf);
      fclose(pInFile);
      fclose(pOutFile);
      return false;
   }

   uint64 src_bytes_left = src_file_size - total_header_bytes;
   uint64 dst_bytes_left = orig_file_size;

   uint in_file_buf_size = 0;
   uint in_file_buf_ofs = 0;

   lzham_decompress_params params;
   memset(&params, 0, sizeof(params));
   params.m_struct_size = sizeof(lzham_decompress_params);
   params.m_dict_size_log2 = dict_size;
   if (options.m_compute_adler32_during_decomp)
      params.m_decompress_flags |= LZHAM_DECOMP_FLAG_COMPUTE_ADLER32;
   if (options.m_unbuffered_decompression)
      params.m_decompress_flags |= LZHAM_DECOMP_FLAG_OUTPUT_UNBUFFERED;
	
	params.m_table_update_rate = options.m_table_update_rate;
   	
   timer_ticks start_time = timer::get_ticks();
   double decomp_only_time = 0;

   if (pSeed_filename)
   {
      if (!read_seed_file(pSeed_filename, params.m_num_seed_bytes, params.m_pSeed_bytes, params.m_dict_size_log2))
      {
         _aligned_free(in_file_buf);
         _aligned_free(out_file_buf);
         fclose(pInFile);
         fclose(pOutFile);
         return false;
      }
   }

   timer_ticks init_start_time = timer::get_ticks();
   lzham_decompress_state_ptr pDecomp_state = lzham_dll.lzham_decompress_init(&params);
   timer_ticks total_init_time = timer::get_ticks() - init_start_time;
   if (!pDecomp_state)
   {
      print_error("Failed initializing decompressor!\n");
      _aligned_free(in_file_buf);
      _aligned_free(out_file_buf);
      _aligned_free((void*)params.m_pSeed_bytes);
      fclose(pInFile);
      fclose(pOutFile);
      return false;
   }

	if (pTotal_decomp_time)
		*pTotal_decomp_time += (float)timer::ticks_to_secs(total_init_time);

   printf("lzham_decompress_init took %3.3fms\n", timer::ticks_to_secs(total_init_time)*1000.0f);

   lzham_decompress_status_t status;
   for ( ; ; )
   {
      if (in_file_buf_ofs == in_file_buf_size)
      {
         in_file_buf_size = static_cast<uint>(my_min(cInBufSize, src_bytes_left));

         if (fread(in_file_buf, 1, in_file_buf_size, pInFile) != in_file_buf_size)
         {
            print_error("Failure reading from source file!\n");
            _aligned_free(in_file_buf);
            _aligned_free(out_file_buf);
            _aligned_free((void*)params.m_pSeed_bytes);
            lzham_dll.lzham_decompress_deinit(pDecomp_state);
            fclose(pInFile);
            fclose(pOutFile);
            return false;
         }

         src_bytes_left -= in_file_buf_size;

         in_file_buf_ofs = 0;
      }

      uint8 *pIn_bytes = &in_file_buf[in_file_buf_ofs];
      size_t num_in_bytes = in_file_buf_size - in_file_buf_ofs;
      uint8* pOut_bytes = out_file_buf;
      size_t out_num_bytes = out_buf_size;

      {
         timer decomp_only_timer;
         decomp_only_timer.start();
         status = lzham_dll.lzham_decompress(pDecomp_state, pIn_bytes, &num_in_bytes, pOut_bytes, &out_num_bytes, src_bytes_left == 0);
			double secs = decomp_only_timer.get_elapsed_secs();
         decomp_only_time += secs;
			if (pTotal_decomp_time)
				*pTotal_decomp_time += (float)secs;
      }

      if (num_in_bytes)
      {
         in_file_buf_ofs += (uint)num_in_bytes;
         assert(in_file_buf_ofs <= in_file_buf_size);
      }

      if (out_num_bytes)
      {
         if (fwrite(out_file_buf, 1, static_cast<uint>(out_num_bytes), pOutFile) != out_num_bytes)
         {
            print_error("Failure writing to destination file!\n");
            _aligned_free(in_file_buf);
            _aligned_free(out_file_buf);
            _aligned_free((void*)params.m_pSeed_bytes);
            lzham_dll.lzham_decompress_deinit(pDecomp_state);
            fclose(pInFile);
            fclose(pOutFile);
            return false;
         }

         if (out_num_bytes > dst_bytes_left)
         {
            print_error("Decompressor wrote too many bytes to destination file!\n");
            _aligned_free(in_file_buf);
            _aligned_free(out_file_buf);
            _aligned_free((void*)params.m_pSeed_bytes);
            lzham_dll.lzham_decompress_deinit(pDecomp_state);
            fclose(pInFile);
            fclose(pOutFile);
            return false;
         }
         dst_bytes_left -= out_num_bytes;
      }

      if (status >= LZHAM_DECOMP_STATUS_FIRST_SUCCESS_OR_FAILURE_CODE)
         break;
   }
   _aligned_free(in_file_buf);
   in_file_buf = NULL;

   _aligned_free(out_file_buf);
   out_file_buf = NULL;

   _aligned_free((void*)params.m_pSeed_bytes);
   params.m_pSeed_bytes = NULL;

   src_bytes_left += (in_file_buf_size - in_file_buf_ofs);

	timer deinit_timer;
	deinit_timer.start();
   uint32 adler32 = lzham_dll.lzham_decompress_deinit(pDecomp_state);
   pDecomp_state = NULL;
	if (pTotal_decomp_time)
		*pTotal_decomp_time += (float)deinit_timer.get_elapsed_secs();

   timer_ticks end_time = timer::get_ticks();
   double total_time = timer::ticks_to_secs(my_max(1, end_time - start_time));

   fclose(pInFile);
   pInFile = NULL;

   fclose(pOutFile);
   pOutFile = NULL;

   if (status != LZHAM_DECOMP_STATUS_SUCCESS)
   {
      print_error("Decompression FAILED with status %i\n", status);
      return false;
   }

   if (dst_bytes_left)
   {
      print_error("Decompressor FAILED to output the entire output file!\n");
      return false;
   }

   if (src_bytes_left)
   {
      print_error("Decompressor FAILED to read " QUAD_INT_FMT " bytes from input buffer (ignore if testing reinit() because we don't truncate the output file between passes)\n", src_bytes_left);
   }

   printf("Success\n");
   printf("Source file size: " QUAD_INT_FMT ", Decompressed file size: " QUAD_INT_FMT "\n", src_file_size, orig_file_size);
   printf("Decompressed adler32: 0x%08X\n", adler32);
   printf("Overall decompression time (decompression init+I/O+decompression): %3.6f\n  Consumption rate: %9.1f bytes/sec, Decompression rate: %9.1f bytes/sec\n", total_time, src_file_size / total_time, orig_file_size / total_time);
   printf("Decompression only time (not counting decompression init or I/O): %3.6f\n  Consumption rate: %9.1f bytes/sec, Decompression rate: %9.1f bytes/sec\n", decomp_only_time, src_file_size / decomp_only_time, orig_file_size / decomp_only_time);

   return true;
}

static bool compare_files(const char *pFilename1, const char* pFilename2)
{
   FILE* pFile1 = open_file_with_retries(pFilename1, "rb");
   if (!pFile1)
   {
      print_error("Failed opening file: %s\n", pFilename1);
      return false;
   }

   FILE* pFile2 = open_file_with_retries(pFilename2, "rb");
   if (!pFile2)
   {
      print_error("Failed opening file: %s\n", pFilename2);
      fclose(pFile1);
      return false;
   }

   fseek(pFile1, 0, SEEK_END);
   int64 fileSize1 = _ftelli64(pFile1);
   fseek(pFile1, 0, SEEK_SET);

   fseek(pFile2, 0, SEEK_END);
   int64 fileSize2 = _ftelli64(pFile2);
   fseek(pFile2, 0, SEEK_SET);

   if (fileSize1 != fileSize2)
   {
      print_error("Files to compare are not the same size: %I64i vs. %I64i.\n", fileSize1, fileSize2);
      fclose(pFile1);
      fclose(pFile2);
      return false;
   }

   const uint cBufSize = 1024 * 1024;
   std::vector<uint8> buf1(cBufSize);
   std::vector<uint8> buf2(cBufSize);

   int64 bytes_remaining = fileSize1;
   while (bytes_remaining)
   {
      const uint bytes_to_read = static_cast<uint>(my_min(cBufSize, bytes_remaining));

      if (fread(&buf1.front(), bytes_to_read, 1, pFile1) != 1)
      {
         print_error("Failed reading from file: %s\n", pFilename1);
         fclose(pFile1);
         fclose(pFile2);
         return false;
      }

      if (fread(&buf2.front(), bytes_to_read, 1, pFile2) != 1)
      {
         print_error("Failed reading from file: %s\n", pFilename2);
         fclose(pFile1);
         fclose(pFile2);
         return false;
      }

      if (memcmp(&buf1.front(), &buf2.front(), bytes_to_read) != 0)
      {
         print_error("File data comparison failed!\n");
         fclose(pFile1);
         fclose(pFile2);
         return false;
      }

      bytes_remaining -= bytes_to_read;
   }

   fclose(pFile1);
   fclose(pFile2);
   return true;
}

typedef std::vector< std::string > string_array;

#if defined(WIN32) || defined(_XBOX)
static bool find_files(std::string pathname, const std::string &filename, string_array &files, bool recursive, int level = 0)
{
   if (!pathname.empty())
   {
      char c = pathname[pathname.size() - 1];
      if ((c != ':') && (c != '\\') && (c != '/'))
         pathname += "\\";
   }

   WIN32_FIND_DATAA find_data;

   HANDLE findHandle = FindFirstFileA((pathname + filename).c_str(), &find_data);
   if (findHandle == INVALID_HANDLE_VALUE)
   {
     HRESULT hres = GetLastError();
     if ((level == 0) && (hres != NO_ERROR) && (hres != ERROR_FILE_NOT_FOUND))
       return false;
   }
   else
   {
     do
     {
        const bool is_directory = (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        const bool is_system = (find_data.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != 0;
        const bool is_hidden = (find_data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != 0;
        const bool is_temp = (find_data.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY) != 0;

        std::string filename(find_data.cFileName);

        if ((!is_directory) && (!is_system) && (!is_hidden) && (!is_temp))
           files.push_back(pathname + filename);

     } while (FindNextFileA(findHandle, &find_data));
          
     FindClose(findHandle);
   }
   
   if (recursive)
   {
      string_array paths;

      HANDLE findHandle = FindFirstFileA((pathname + "*").c_str(), &find_data);
      if (findHandle == INVALID_HANDLE_VALUE)
      {
         HRESULT hres = GetLastError();
         if ((level == 0) && (hres != NO_ERROR) && (hres != ERROR_FILE_NOT_FOUND))
            return false;
      }
      else
      {
         do
         {
            const bool is_directory = (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
            const bool is_system = (find_data.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != 0;
            const bool is_hidden = (find_data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != 0;
            const bool is_temp = (find_data.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY) != 0;

            std::string filename(find_data.cFileName);

            if ((is_directory) && (!is_hidden) && (!is_system) && (!is_temp))
               paths.push_back(filename);

         } while (FindNextFileA(findHandle, &find_data));
         
         FindClose(findHandle);
            
         for (uint i = 0; i < paths.size(); i++)
         {
            const std::string &path = paths[i];
            if (path[0] == '.')
               continue;

            if (!find_files(pathname + path, filename, files, true, level + 1))
               return false;
         }
      }
   }

   return true;
}
#else
#include <dirent.h>

static bool find_files(std::string pathname, const std::string &filename, string_array &files, bool recursive)
{
   if (!pathname.empty())
   {
      char c = pathname[pathname.size() - 1];
      if ((c != ':') && (c != '\\') && (c != '/'))
         pathname += "/";
   }

   DIR *dp = opendir(pathname.c_str());

   if (!dp)
      return false;

   string_array paths;

   for ( ; ; )
   {
      struct dirent *ep = readdir(dp);
      if (!ep)
         break;

      const bool is_directory = (ep->d_type & DT_DIR) != 0;
      const bool is_file =  (ep->d_type & DT_REG) != 0;

      if (ep->d_name[0] == '.')
         continue;

      std::string filename(ep->d_name);

      if (is_directory)
      {
         if (recursive)
            paths.push_back(filename);
      }
      else if (is_file)
         files.push_back(pathname + filename);
   }

   closedir(dp);
   dp = NULL;

   if (recursive)
   {
      for (uint i = 0; i < paths.size(); i++)
      {
         const std::string &path = paths[i];
         if (!find_files(pathname + path, filename, files, true))
            return false;
      }
   }

   return true;
}
#endif

struct file_stats
{
	file_stats()
	{
		clear();
	}

	void clear()
	{
		m_size = 0;
		m_lzma_size = 0;
		m_lzma_comp_time = 0;
		m_lzma_decomp_time = 0;
		m_lzham_size = 0;
		m_lzham_comp_time = 0;
		m_lzham_decomp_time = 0;
		m_miniz_size = 0;
		m_miniz_comp_time = 0;
		m_miniz_decomp_time = 0;
		m_lz4_size = 0;
		m_lz4_comp_time = 0;
		m_lz4_decomp_time = 0;
	}

	std::string m_filename;
	int64 m_size;
	
	int64 m_lzma_size;
	float m_lzma_comp_time;
	float m_lzma_decomp_time;

	int64 m_lzham_size;
	float m_lzham_comp_time;
	float m_lzham_decomp_time;

	int64 m_miniz_size;
	float m_miniz_comp_time;
	float m_miniz_decomp_time;

	int64 m_lz4_size;
	float m_lz4_comp_time;
	float m_lz4_decomp_time;
};
typedef std::vector<file_stats> file_stats_vec;

static bool test_recursive(ilzham &lzham_dll, const char *pPath, comp_options options, const char *pSeed_filename, file_stats_vec *pStats = NULL, int64 min_file_size = -1, int64 max_file_size = -1)
{
   string_array files;
   if (!find_files(pPath, "*", files, true))
   {
      print_error("Failed finding files under path \"%s\"!\n", pPath);
      return false;
   }

   uint total_files_compressed = 0;
   uint64 total_source_size = 0;
   uint64 total_comp_size = 0;

#ifdef WIN32
   MEMORYSTATUS initial_mem_status;
   GlobalMemoryStatus(&initial_mem_status);
#endif

   timer_ticks start_tick_count = timer::get_ticks();

   const int first_file_index = 0;

   uint unique_id = static_cast<uint>(timer::get_init_ticks());
   char cmp_file[256], decomp_file[256];

#ifdef _XBOX
   sprintf(cmp_file, "e:\\__comp_temp_%u__.tmp", unique_id);
   sprintf(decomp_file, "e:\\__decomp_temp_%u__.tmp", unique_id);
#else
   sprintf(cmp_file, "__comp_temp_%u__.tmp", unique_id);
   sprintf(decomp_file, "__decomp_temp_%u__.tmp", unique_id);
#endif

   for (uint file_index = first_file_index; file_index < files.size(); file_index++)
   {
      const std::string &src_file = files[file_index];

      printf("***** [%u of %u] Compressing file \"%s\" to \"%s\"\n", 1 + file_index, (uint)files.size(), src_file.c_str(), cmp_file);

      FILE *pFile = fopen(src_file.c_str(), "rb");
      if (!pFile)
      {
         printf("Skipping unreadable file \"%s\"\n", src_file.c_str());
         continue;
      }
      fseek(pFile, 0, SEEK_END);
      int64 src_file_size = _ftelli64(pFile);
      if (src_file_size)
      {
         // Ensure file is actually readable
         fseek(pFile, 0, SEEK_SET);
         if (fgetc(pFile) == EOF)
         {
            printf("Skipping unreadable file \"%s\"\n", src_file.c_str());
            fclose(pFile);
            continue;
         }
      }
      fclose(pFile);

		if (min_file_size != -1)
		{
			if ((src_file_size < min_file_size) || (src_file_size > max_file_size))
			{
				printf("Skipping too small or large file \"%s\"\n", src_file.c_str());
				continue;
			}	
		}

      if (!ensure_file_is_writable(cmp_file))
      {
         print_error("Unable to create file \"%s\"!\n", cmp_file);
         return false;
      }

      comp_options file_options(options);
      if (options.m_randomize_params)
      {
         file_options.m_comp_level = static_cast<lzham_compress_level>(rand() % LZHAM_TOTAL_COMP_LEVELS);
#ifdef __APPLE__
         file_options.m_dict_size_log2 = LZHAM_MIN_DICT_SIZE_LOG2 + (rand() % (26 - LZHAM_MIN_DICT_SIZE_LOG2 + 1));
#else
         file_options.m_dict_size_log2 = LZHAM_MIN_DICT_SIZE_LOG2 + (rand() % (LZHAMTEST_MAX_POSSIBLE_DICT_SIZE - LZHAM_MIN_DICT_SIZE_LOG2 + 1));
#endif
         file_options.m_max_helper_threads = rand() % (LZHAM_MAX_HELPER_THREADS + 1);
         file_options.m_unbuffered_decompression = ((rand() & 1) != 0) && (!pSeed_filename);
#if !LZHAMTEST_NO_RANDOM_EXTREME_PARSING
         file_options.m_extreme_parsing = (rand() & 1) != 0;
#endif
         file_options.m_deterministic_parsing = (rand() & 1) != 0;
         file_options.m_tradeoff_decomp_rate_for_comp_ratio = (rand() & 1) != 0;
         //file_options.m_test_compressor_reinit = (rand() & 1) != 0;
			file_options.m_table_update_rate = 2 + (rand() % (LZHAM_FASTEST_TABLE_UPDATE_RATE - 2 + 1));

         file_options.print();
      }
		
		float comp_time = 0;
		float decomp_time = 0;

      bool status = compress_file(lzham_dll, src_file.c_str(), cmp_file, file_options, pSeed_filename, &comp_time);
      if (!status)
      {
         print_error("Failed compressing file \"%s\" to \"%s\"\n", src_file.c_str(), cmp_file);
         return false;
      }

      if ((file_options.m_verify_compressed_data) || (pStats))
      {
         printf("Decompressing file \"%s\" to \"%s\"\n", cmp_file, decomp_file);

         if (!ensure_file_is_writable(decomp_file))
         {
            print_error("Unable to create file \"%s\"!\n", decomp_file);
            return false;
         }
						
         status = decompress_file(lzham_dll, cmp_file, decomp_file, file_options, pSeed_filename, &decomp_time);
         if (!status)
         {
            print_error("Failed decompressing file \"%s\" to \"%s\"\n", src_file.c_str(), decomp_file);
            return false;
         }

         printf("Comparing file \"%s\" to \"%s\"\n", decomp_file, src_file.c_str());

         if (!compare_files(decomp_file, src_file.c_str()))
         {
            print_error("Failed comparing decompressed file data while compressing \"%s\" to \"%s\"\n", src_file.c_str(), cmp_file);
            return false;
         }
         else
         {
            printf("Decompressed file compared OK to original file.\n");
         }
      }

      int64 cmp_file_size = 0;
      pFile = fopen(cmp_file, "rb");
      if (pFile)
      {
         fseek(pFile, 0, SEEK_END);
         cmp_file_size = _ftelli64(pFile);
         fclose(pFile);
      }

      total_files_compressed++;
      total_source_size += src_file_size;
      total_comp_size += cmp_file_size;

		if (pStats)
		{
			file_stats stats;
			
			stats.m_filename = src_file;
			stats.m_size = src_file_size;
			stats.m_lzham_comp_time = comp_time;
			stats.m_lzham_decomp_time = decomp_time;
			stats.m_lzham_size = cmp_file_size;
						
			pStats->push_back(stats);
		}

#ifdef WIN32
      MEMORYSTATUS mem_status;
      GlobalMemoryStatus(&mem_status);

#ifdef _XBOX
      const int64 bytes_allocated = initial_mem_status.dwAvailPhys - mem_status.dwAvailPhys;
#else
      const int64 bytes_allocated = initial_mem_status.dwAvailVirtual- mem_status.dwAvailVirtual;
#endif

      printf("Memory allocated relative to first file: %I64i\n", bytes_allocated);
#endif

      printf("\n");
   }

   timer_ticks end_tick_count = timer::get_ticks();

   double total_elapsed_time = timer::ticks_to_secs(end_tick_count - start_tick_count);

   printf("Test successful: %f secs\n", total_elapsed_time);
   printf("Total files processed: %u\n", total_files_compressed);
   printf("Total source size: " QUAD_INT_FMT "\n", total_source_size);
   printf("Total compressed size: " QUAD_INT_FMT "\n", total_comp_size);

   remove(cmp_file);
   remove(decomp_file);

   return true;
}

int main_internal(string_array cmd_line, int num_helper_threads, ilzham &lzham_dll)
{
   comp_options options;
   options.m_max_helper_threads = num_helper_threads;

#ifdef _XBOX
   options.m_dict_size_log2 = 21;
#endif

   if (!cmd_line.size())
   {
      print_usage();
      return simple_test(lzham_dll, options);
   }

   enum op_mode_t
   {
      OP_MODE_INVALID = -1,
      OP_MODE_COMPRESS = 0,
      OP_MODE_DECOMPRESS = 1,
      OP_MODE_ALL = 2
   };

   op_mode_t op_mode = OP_MODE_INVALID;
   std::string seed_filename;

   for (int i = 0; i < (int)cmd_line.size(); i++)
   {
      const std::string &str = cmd_line[i];
      if (str[0] == '-')
      {
         if (str.size() < 2)
         {
            print_error("Invalid option: %s\n", str.c_str());
            return EXIT_FAILURE;
         }
         switch (str[1])
         {
            case 'u':
            {
               options.m_unbuffered_decompression = true;
               break;
            }
            case 'd':
            {
               int dict_size = atoi(str.c_str() + 2);
               if ((dict_size < LZHAM_MIN_DICT_SIZE_LOG2) || (dict_size > LZHAMTEST_MAX_POSSIBLE_DICT_SIZE))
               {
                  print_error("Invalid dictionary size: %s\n", str.c_str());
                  return EXIT_FAILURE;
               }
               options.m_dict_size_log2 = dict_size;
               break;
            }
            case 'm':
            {
               int comp_level = atoi(str.c_str() + 2);
               if ((comp_level < 0) || (comp_level > (int)LZHAM_COMP_LEVEL_UBER))
               {
                  print_error("Invalid compression level: %s\n", str.c_str());
                  return EXIT_FAILURE;
               }
               options.m_comp_level = static_cast<lzham_compress_level>(comp_level);
               break;
            }
            case 't':
            {
               int num_threads = atoi(str.c_str() + 2);
               if ((num_threads < 0) || (num_threads > LZHAM_MAX_HELPER_THREADS))
               {
                  print_error("Invalid number of helper threads: %s\n", str.c_str());
                  return EXIT_FAILURE;
               }
               options.m_max_helper_threads = num_threads;
               break;
            }
				case 'h':
				{
					int update_rate = atoi(str.c_str() + 2);
					if ((update_rate < 0) || (update_rate > LZHAM_FASTEST_TABLE_UPDATE_RATE))
					{
						print_error("Invalid table update rate: %s\n", str.c_str());
						return EXIT_FAILURE;
					}
					options.m_table_update_rate = update_rate;
					break;
				}
            case 'c':
            {
               options.m_compute_adler32_during_decomp = false;
               break;
            }
            case 'v':
            {
               options.m_verify_compressed_data = true;
               break;
            }
            case 'r':
            {
               options.m_randomize_params = true;
               break;
            }
            case 'x':
            {
               options.m_extreme_parsing = true;
               break;
            }
            case 'e':
            {
               options.m_deterministic_parsing = true;
               break;
            }
            case 'o':
            {
               options.m_tradeoff_decomp_rate_for_comp_ratio = true;
               break;
            }
            case 'i':
            {
               options.m_test_compressor_reinit = true;
               options.m_deterministic_parsing = true;
               break;
            }
            case 's':
            {
               int seed = atoi(str.c_str() + 2);
               srand(seed);
               printf("Using random seed: %i\n", seed);
               break;
            }
            case 'a':
            {
               seed_filename = str.c_str() + 2;
               if (seed_filename.empty())
               {
                  print_error("Must specify seed filename with -a option!\n");
                  return EXIT_FAILURE;
               }
               printf("Seed filename: %s\n", seed_filename.c_str());
               break;
            }
            default:
            {
               print_error("Invalid option: %s\n", str.c_str());
               return EXIT_FAILURE;
            }
         }

         cmd_line.erase(cmd_line.begin() + i);
         i--;

         continue;
      }

      if ((options.m_unbuffered_decompression) && (!seed_filename.empty()))
      {
         print_error("Unbuffered decompression is not compatible with seed files!\n");
         return EXIT_FAILURE;
      }

      if (str.size() != 1)
      {
         print_error("Invalid mode: %s\n", str.c_str());
         return EXIT_FAILURE;
      }
      switch (tolower(str[0]))
      {
         case 'c':
         {
            op_mode = OP_MODE_COMPRESS;
            break;
         }
         case 'd':
         {
            op_mode = OP_MODE_DECOMPRESS;
            break;
         }
         case 'a':
         {
            op_mode = OP_MODE_ALL;
            break;
         }
         default:
         {
            print_error("Invalid mode: %s\n", str.c_str());
            return EXIT_FAILURE;
         }
      }
      cmd_line.erase(cmd_line.begin() + i);
      break;
   }

   if (op_mode == OP_MODE_INVALID)
   {
      print_error("No mode specified!\n");
      print_usage();
      return EXIT_FAILURE;
   }

   printf("Using options:\n");
   options.print();
   printf("\n");

   int exit_status = EXIT_FAILURE;

   switch (op_mode)
   {
      case OP_MODE_COMPRESS:
      {
         if (cmd_line.size() < 2)
         {
            print_error("Must specify input and output filenames!\n");
            return EXIT_FAILURE;
         }
         else if (cmd_line.size() > 2)
         {
            print_error("Too many filenames!\n");
            return EXIT_FAILURE;
         }

         const std::string &src_file = cmd_line[0];
         const std::string &cmp_file = cmd_line[1];

         bool comp_result = compress_file(lzham_dll, src_file.c_str(), cmp_file.c_str(), options, seed_filename.length() ? seed_filename.c_str() : NULL);
         if (comp_result)
            exit_status = EXIT_SUCCESS;

         if ((comp_result) && (options.m_verify_compressed_data))
         {
            char decomp_file[256];

#ifdef _XBOX
            sprintf(decomp_file, "e:\\__decomp_temp_%u__.tmp", (uint)GetTickCount());
#else
            sprintf(decomp_file, "__decomp_temp_%u__.tmp", (uint)timer::get_ms());
#endif
            if (!decompress_file(lzham_dll, cmp_file.c_str(), decomp_file, options, seed_filename.length() ? seed_filename.c_str() : NULL))
            {
               print_error("Failed decompressing file \"%s\" to \"%s\"\n", cmp_file.c_str(), decomp_file, seed_filename.length() ? seed_filename.c_str() : NULL);
               return EXIT_FAILURE;
            }

            printf("Comparing file \"%s\" to \"%s\"\n", decomp_file, src_file.c_str());

            if (!compare_files(decomp_file, src_file.c_str()))
            {
               print_error("Failed comparing decompressed file data while compressing \"%s\" to \"%s\"\n", src_file.c_str(), cmp_file.c_str());
               return EXIT_FAILURE;
            }
            else
            {
               printf("Decompressed file compared OK to original file.\n");
            }

            remove(decomp_file);
         }

         break;
      }
      case OP_MODE_DECOMPRESS:
      {
         if (cmd_line.size() < 2)
         {
            print_error("Must specify input and output filenames!\n");
            return EXIT_FAILURE;
         }
         else if (cmd_line.size() > 2)
         {
            print_error("Too many filenames!\n");
            return EXIT_FAILURE;
         }
         if (decompress_file(lzham_dll, cmd_line[0].c_str(), cmd_line[1].c_str(), options, seed_filename.length() ? seed_filename.c_str() : NULL))
            exit_status = EXIT_SUCCESS;
         break;
      }
      case OP_MODE_ALL:
      {
         if (!cmd_line.size())
         {
            print_error("No directory specified!\n");
            return EXIT_FAILURE;
         }
         else if (cmd_line.size() != 1)
         {
            print_error("Too many filenames!\n");
            return EXIT_FAILURE;
         }

			if (test_recursive(lzham_dll, cmd_line[0].c_str(), options, seed_filename.length() ? seed_filename.c_str() : NULL))
				exit_status = EXIT_SUCCESS;
            
         break;
      }
      default:
      {
         print_error("No mode specified!\n");
         print_usage();
         return EXIT_FAILURE;
      }
   }

   return exit_status;
}

#ifdef _XBOX
int main()
#else
//int lzhamtest_main(int argc, char *argv[])
int main(int argc, char *argv[])
#endif
{
   (void)g_is_debug;

#ifdef _XBOX
   int argc = 3;
   char *argv[4] = { "", "A", "E:\\samples", "" };

   DmMapDevkitDrive();
#endif

#ifdef LZHAM_64BIT
   printf("LZHAM Codec - x64 Command Line Test App - Compiled %s %s\n", __DATE__, __TIME__);
#else
   printf("LZHAM Codec - x86 Command Line Test App - Compiled %s %s\n", __DATE__, __TIME__);
#endif

   printf("Expecting LZHAM DLL Version 0x%04X\n", LZHAM_DLL_VERSION);

#if LZHAM_STATIC_LIB
   lzham_static_lib lzham_lib;
   lzham_lib.load();
   printf("Using static libraries.\n");
#else
   lzham_dll_loader lzham_lib;
   char lzham_dll_filename[MAX_PATH];
   lzham_dll_loader::create_module_path(lzham_dll_filename, MAX_PATH, g_is_debug);
   printf("Dynamically loading DLL \"%s\"\n", lzham_dll_filename);

   HRESULT hres = lzham_lib.load(lzham_dll_filename);
   if (FAILED(hres))
   {
      print_error("Failed loading LZHAM DLL (Status=0x%04X)!\n", (uint)hres);
      return EXIT_FAILURE;
   }
#endif

   // -1 = auto-detection
   int num_helper_threads = -1;

#ifdef _XBOX
   num_helper_threads = 5;
#elif defined(WIN32)
   SYSTEM_INFO g_system_info;
   GetSystemInfo(&g_system_info);
   if (g_system_info.dwNumberOfProcessors > 1)
   {
      num_helper_threads = g_system_info.dwNumberOfProcessors - 1;
   }
#endif

   printf("Loaded LZHAM DLL version 0x%04X\n\n", lzham_lib.lzham_get_version());

   string_array cmd_line;
   for (int i = 1; i < argc; i++)
   {
      cmd_line.push_back(std::string(argv[i]));
   }

   int exit_status = main_internal(cmd_line, num_helper_threads, lzham_lib);

   lzham_lib.unload();

   return exit_status;
}
