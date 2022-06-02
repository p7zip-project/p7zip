// example3.cpp - Tests streaming and random access packet compression using the compressor's LZHAM_Z_FULL_FLUSH flush mode, and the
// decompressor's LZHAM_Z_SYNC_FLUSH flush mode.
// This example statically links against lzhamlib, lzhamcomp, and lzhamdecomp libs.
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <vector>

// Define LZHAM_DEFINE_ZLIB_API causes lzham.h to remap the zlib.h functions/macro definitions to lzham's.
// This is totally optional - you can also directly use the lzham_* functions and macros instead.
#define LZHAM_DEFINE_ZLIB_API
#include "lzham_static_lib.h"

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint;

#define my_max(a,b) (((a) > (b)) ? (a) : (b))
#define my_min(a,b) (((a) < (b)) ? (a) : (b))

#define BUF_SIZE (1024 * 1024)
static uint8 s_inbuf[BUF_SIZE];
static uint8 s_outbuf[BUF_SIZE];

#define WINDOW_BITS 20

// Tests 
static bool sequential_test()
{
   printf("Sequential record/packet decompression test:\n");

   z_stream stream;
   memset(&stream, 0, sizeof(stream));
   stream.next_in = s_inbuf;
   stream.avail_in = 0;
   stream.next_out = s_outbuf;
   stream.avail_out = BUF_SIZE;

   if (deflateInit2(&stream, Z_BEST_COMPRESSION, LZHAM_Z_LZHAM, WINDOW_BITS, 9, Z_DEFAULT_STRATEGY) != Z_OK)
      return false;

   std::vector<uint8> comp_buf;
   const uint cRecordSize = 128;

   for (uint i = 0; i < 1000; i++)
   {
      memset(s_inbuf, 0, 128);
      sprintf((char*)s_inbuf, "%u, %u, %u, %u, This is a testThis is a testThis is a testThis is a testThis is a test\n", i, i, i, i);
      
      stream.next_in = s_inbuf;
      stream.avail_in = cRecordSize;

      while (stream.avail_in)
      {
         stream.next_out = s_outbuf;
         stream.avail_out = BUF_SIZE;

         if (deflate(&stream, Z_FULL_FLUSH) != Z_OK)
         {
            deflateEnd(&stream);
            return false;
         }

         comp_buf.insert(comp_buf.end(), s_outbuf, s_outbuf + (BUF_SIZE - stream.avail_out));
      }

      printf("Compressing record %u and flushing compressor, %u bytes\n", i, stream.total_out);
   }

   stream.avail_in = 0;
   for ( ; ; )
   {
      stream.next_out = s_outbuf;
      stream.avail_out = BUF_SIZE;
      int status = deflate(&stream, Z_FINISH);

      comp_buf.insert(comp_buf.end(), s_outbuf, s_outbuf + (BUF_SIZE - stream.avail_out));

      if (status == Z_STREAM_END)
         break;
      else if (status != Z_OK)
      {
         deflateEnd(&stream);
         return false;
      }
   }

   deflateEnd(&stream);

   memset(&stream, 0, sizeof(stream));
   stream.next_in = &comp_buf[0];
   stream.avail_in = (uint)comp_buf.size();
   stream.next_out = s_outbuf;
   stream.avail_out = BUF_SIZE;

   if (inflateInit2(&stream, WINDOW_BITS) != Z_OK)
      return false;

   for (uint i = 0; i < 1000; i++)
   {
      stream.next_out = s_outbuf;
      stream.avail_out = cRecordSize;

      int status = inflate(&stream, Z_SYNC_FLUSH);
      if (status == Z_STREAM_END)
         break;
      if (status != Z_OK)
      {
         inflateEnd(&stream);
         return false;
      }

      memset(s_inbuf, 0, 128);
      sprintf((char*)s_inbuf, "%u, %u, %u, %u, This is a testThis is a testThis is a testThis is a testThis is a test\n", i, i, i, i);
      if (memcmp(s_inbuf, s_outbuf, cRecordSize) != 0)
         return false;

      printf("Successfully decompressed record %u\n", i);
   }

   inflateEnd(&stream);

   return true;
}

static bool random_test()
{
   printf("Random record/packet decompression test:\n");

   z_stream stream;
   memset(&stream, 0, sizeof(stream));
   stream.next_in = s_inbuf;
   stream.avail_in = 0;
   stream.next_out = s_outbuf;
   stream.avail_out = BUF_SIZE;

   if (deflateInit2(&stream, Z_BEST_COMPRESSION, LZHAM_Z_LZHAM, WINDOW_BITS, 9, Z_DEFAULT_STRATEGY) != Z_OK)
      return false;

   const uint cTotalRecords = 200;
   std::vector< std::vector<uint8> > comp_bufs(cTotalRecords);
   
   const uint cRecordSize = 128;

   for (uint i = 0; i < cTotalRecords; i++)
   {
      memset(s_inbuf, 0, 128);
      sprintf((char*)s_inbuf, "%u, %u, %u, %u, This is a testThis is a testThis is a testThis is a testThis is a test\n", i, i, i, i);

      stream.next_in = s_inbuf;
      stream.avail_in = cRecordSize;

      while (stream.avail_in)
      {
         stream.next_out = s_outbuf;
         stream.avail_out = BUF_SIZE;

         if (deflate(&stream, Z_FULL_FLUSH) != Z_OK)
         {
            deflateEnd(&stream);
            return false;
         }

         comp_bufs[i].insert(comp_bufs[i].end(), s_outbuf, s_outbuf + (BUF_SIZE - stream.avail_out));
      }

      printf("Compressing record %u and flushing compressor, %u bytes\n", i, stream.total_out);
   }
  
   deflateEnd(&stream);

   memset(&stream, 0, sizeof(stream));
   stream.next_in = &(comp_bufs[0])[0];
   stream.avail_in = (uint)comp_bufs[0].size();
   stream.next_out = s_outbuf;
   stream.avail_out = BUF_SIZE;

   if (inflateInit2(&stream, WINDOW_BITS) != Z_OK)
      return false;

   for (uint i = 0; i < cTotalRecords * 2; i++)
   {
      uint rec_index = 0;
      if (i)
         rec_index = (rand() % (cTotalRecords - 1)) + 1;

      stream.next_in = &(comp_bufs[rec_index])[0];
      stream.avail_in = (uint)comp_bufs[rec_index].size();
      stream.next_out = s_outbuf;
      stream.avail_out = cRecordSize;

      int status = inflate(&stream, Z_SYNC_FLUSH);
      if (status == Z_STREAM_END)
         break;
      if (status != Z_OK)
      {
         inflateEnd(&stream);
         return false;
      }

      memset(s_inbuf, 0, 128);
      sprintf((char*)s_inbuf, "%u, %u, %u, %u, This is a testThis is a testThis is a testThis is a testThis is a test\n", rec_index, rec_index, rec_index, rec_index);
      if (memcmp(s_inbuf, s_outbuf, cRecordSize) != 0)
         return false;

      printf("Successfully decompressed random record %u\n", rec_index);
   }

   inflateEnd(&stream);

   return true;
}

int main(int argc, char *argv[])
{
   argc, argv;
   printf("LZHAM example3 (single function call compression/decompression with flushing)\nUsing library version: %s\n", ZLIB_VERSION);

   if (!sequential_test())
   {
      printf("Sequential test failed!\n");
      return EXIT_FAILURE;
   }

   if (!random_test())
   {
      printf("Random test failed!\n");
      return EXIT_FAILURE;
   }

   printf("Test succeeded.\n");
   return EXIT_SUCCESS;
}
