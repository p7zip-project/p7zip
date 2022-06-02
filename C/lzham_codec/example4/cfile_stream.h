// File: cfile_stream.h
// LZHAM is in the Public Domain. Please see the Public Domain declaration at the end of include/lzham.h
#pragma once
#include "data_stream.h"
#include <stdio.h>

namespace lzham_ex
{
   // stdio.h FILE stream. Supports 64-bit offsets/file sizes.
   class cfile_stream : public data_stream
   {
   public:
      cfile_stream();
      cfile_stream(FILE* pFile, const char* pFilename, uint attribs, bool has_ownership);
      cfile_stream(const char* pFilename, uint attribs = cDataStreamReadable | cDataStreamSeekable, bool open_existing = false);
      
      virtual ~cfile_stream();
      
      virtual bool close();
 
      bool open(FILE* pFile, const char* pFilename, uint attribs, bool has_ownership);
      bool open(const char* pFilename, uint attribs = cDataStreamReadable | cDataStreamSeekable, bool open_existing = false);
  
      inline FILE* get_file() const { return m_pFile; }

      virtual uint read(void* pBuf, uint len);
      virtual uint write(const void* pBuf, uint len);
      virtual bool flush();

      virtual uint64 get_size();
      virtual uint64 get_remaining();
      virtual uint64 get_ofs();

      virtual bool seek(int64 ofs, bool relative);
     
      static bool read_file_into_array(const char* pFilename, std::vector<uint8>& buf);
      static bool write_array_to_file(const char* pFilename, const std::vector<uint8>& buf);
      
   private:
      FILE* m_pFile;
      uint64 m_size, m_ofs;
      bool m_has_ownership;
   };

} // namespace lzham_ex
