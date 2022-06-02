// File: dynamic_stream.h
// LZHAM is in the Public Domain. Please see the Public Domain declaration at the end of include/lzham.h
#pragma once
#include "data_stream.h"

namespace lzham_ex
{
   // A fully growable stream, internally uses std::vector<uint8> to store written data.
   class dynamic_stream : public data_stream
   {
   public:
      dynamic_stream(uint64 initial_size, const char* pName = "dynamic_stream", uint attribs = cDataStreamSeekable | cDataStreamWritable | cDataStreamReadable);
      dynamic_stream(const void* pBuf, uint64 size, const char* pName = "dynamic_stream", uint attribs = cDataStreamSeekable | cDataStreamWritable | cDataStreamReadable);
      dynamic_stream();
            
      virtual ~dynamic_stream();
            
      bool open(uint64 initial_size = 0, const char* pName = "dynamic_stream", uint attribs = cDataStreamSeekable | cDataStreamWritable | cDataStreamReadable);
      bool reopen(const char* pName, uint attribs);
      bool open(const void* pBuf, uint64 size, const char* pName = "dynamic_stream", uint attribs = cDataStreamSeekable | cDataStreamWritable | cDataStreamReadable);
            
      virtual bool close();
           
      inline const std::vector<uint8>& get_buf() const { return m_buf; }
      inline       std::vector<uint8>& get_buf()       { return m_buf; }

      bool reserve(uint64 size);
         
      virtual const void* get_ptr() const { return m_buf.empty() ? NULL : &m_buf[0]; }
      
      virtual uint read(void* pBuf, uint len);
            
      virtual uint write(const void* pBuf, uint len);
            
      virtual bool flush();
      
      virtual uint64 get_size();
      virtual uint64 get_remaining();
      virtual uint64 get_ofs();
            
      virtual bool seek(int64 ofs, bool relative);
                     
   private:
      std::vector<uint8> m_buf;
      uint64             m_ofs;
   };

} // namespace lzham_ex

