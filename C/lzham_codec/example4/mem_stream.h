// File: mem_stream.h
// See Copyright Notice and license at the end of include/lzham.h
#pragma once
#include "data_stream.h"

namespace lzham_ex
{
   // Provides a read/write stream view of a static, user provided memory buffer. 
   class mem_stream : public data_stream
   {
   public:
      mem_stream();
      mem_stream(void* p, uint64 size);
      mem_stream(const void* p, uint64 size);
         
      virtual ~mem_stream();
      
      bool open(const void* p, uint64 size);
      bool open(void* p, uint64 size);
      
      virtual bool close();
   
      inline const void* get_buf() const   { return m_pBuf; }
      inline       void* get_buf()         { return m_pBuf; }

      virtual const void* get_ptr() const { return m_pBuf; }

      virtual uint read(void* pBuf, uint len);
      virtual uint write(const void* pBuf, uint len);
      
      virtual bool flush();
      
      virtual uint64 get_size();
      virtual uint64 get_remaining();
      virtual uint64 get_ofs();

      virtual bool seek(int64 ofs, bool relative);
      
   private:
      uint8*   m_pBuf;
      uint64   m_size;
      uint64   m_ofs;
   };

} // namespace lzham_ex

