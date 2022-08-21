// example4.cpp - Simple example of the various example stream and serialization classes in the "lzham_ex" namespace.
// This example statically links against the lzhamlib, lzhamcomp, and lzhamdecomp libs.
//
// Note: The C++ classes in the "lzham_ex" namespace are totally optional and not part of the core LZHAM codec itself. They're mainly here to serve as more 
// complex examples of how to use the library. I debated whether to include them at all, given their complexity, but they make decent examples of how to 
// create a easy to use stream/serialization library with built-in support for compression. 
//
// Short description of each header:
//  data_stream.h    - Defines the data_stream abstract base class. A data_stream is like a file: it's a sequential stream of bytes that can be read/written from/to the current file position.
//                     data_stream's can be read-only, write-only, readable+writable, and seekable/non-seekable, and have a finite, infinite, or unknown length.
//  dynamic_stream.h - A growable data stream. Internally, it used a std::vector<uint8> to store the stream's data.
//  mem_stream.h     - A non-growable data stream aliased over a user-provided memory buffer.
//  cfile_stream.h   - A data stream read and/or written to a file. Internally used the C stdio API's fopen(), fread(), etc.
//  comp_stream.h    - compression_stream is a write-only non-seekable stream that automatically compresses all data passed to it. decompression_stream is its read-only, non-seekable counterpart.
//                     compression_stream writes the compressed data to the specified output data_stream. It supports non-seekable output streams.
//                     decompression_stream reads the compressed data from the specified input data_stream. It supports non-seekable input streams.
//                     All compressed data streams begin with a endian-neutral header (see compressed_stream_header struct).
//  data_stream_serializer.h - A simple serialization/deserialization helper class. Supports big and little endian data.
#include "data_stream.h"
#include "dynamic_stream.h"
#include "mem_stream.h"
#include "comp_stream.h"
#include "cfile_stream.h"
#include "data_stream_serializer.h"

#include "lzham_static_lib.h"

int main(int argc, char *argv[])
{
   argc, argv;
   printf("LZHAM example4 (example stream/serialization classes)\nUsing library version: 0x%04X\n", LZHAM_DLL_VERSION);

   // Create a write-only growable data buffer to hold the compressed data.
   lzham_ex::dynamic_stream compressed_data_buf(0, "compressed_data", lzham_ex::cDataStreamWritable);
   
   // Create and open a compression stream, have its output written to the growable data buffer.
   lzham_ex::compression_stream compression_stream;
   if (!compression_stream.open(&compressed_data_buf))
   {
      printf("Failed opening compression stream!\n");
      return EXIT_FAILURE;
   }
   
   // printf some stuff into the compression stream.
   for (int i = 0; i < 500; i++)
      compression_stream.printf("This is a test %u\n", i);

   // Create a serializer chained to the compression stream, and output some things to it.
   lzham_ex::data_stream_serializer serializer(&compression_stream);
   serializer << 9999 << "This is a test";

   printf("Uncompressed size: %i\n", static_cast<int>(compression_stream.get_ofs()));
   
   // Close the compression stream.
   if (!compression_stream.close())
   {
      printf("Failed closing compression stream!\n");
      return EXIT_FAILURE;
   }
   
   printf("Compressed size: %i\n", static_cast<int>(compressed_data_buf.get_ofs()));
   
   // Now reopen the growable data buffer as readable/seekable (this is optional/only for testing - it could have been open with all permissions above).
   compressed_data_buf.reopen("dynamic_string", lzham_ex::cDataStreamSeekable | lzham_ex::cDataStreamReadable);
   // Set the data buffer's pointer to the beginning of the stream.
   compressed_data_buf.seek(0, false);

   // Create a decompression stream which gets its input from the growable data buffer used to hold the compressed data.
   lzham_ex::decompression_stream decompression_stream;
   if (!decompression_stream.open(&compressed_data_buf))
   {
      printf("Failed opening decompression stream!\n");
      return EXIT_FAILURE;
   }

   // Read the lines from the decompression stream.
   for (int i = 0; i < 500; i++)
   {
      std::string str;
      if (!decompression_stream.read_line(str))
      {
         printf("Failed reading from decompression stream!\n");
         return EXIT_FAILURE;
      }
      printf("%s\n", str.c_str());
   }

   // Create another serializer that reads from the decompression stream, and deserialize some things.
   lzham_ex::data_stream_serializer deserializer(&decompression_stream);
   int i;
   std::string s;
   deserializer >> i >> s;
   printf("%i %s\n", i, s.c_str());

   // Close the decompression stream.
   if (!decompression_stream.close())
   {
      printf("Failed closing decompression stream!\n");
      return EXIT_FAILURE;
   }
      
   printf("Test succeeded.\n");
   return EXIT_SUCCESS;
}
