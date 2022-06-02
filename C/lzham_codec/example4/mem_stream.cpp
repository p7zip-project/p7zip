// File: mem_stream.cpp
// LZHAM is in the Public Domain. Please see the Public Domain declaration at the end of include/lzham.h
#include "mem_stream.h"

namespace lzham_ex
{
   mem_stream::mem_stream() :
      data_stream(),
      m_pBuf(NULL),
      m_size(0),
      m_ofs(0)
   {
   }

   mem_stream::mem_stream(void* p, uint64 size) :
      data_stream(),
      m_pBuf(NULL),
      m_size(0),
      m_ofs(0)
   {
      open(p, size);
   }

   mem_stream::mem_stream(const void* p, uint64 size) :
      data_stream(),
      m_pBuf(NULL),
      m_size(0),
      m_ofs(0)
   {
      open(p, size);
   }

   mem_stream::~mem_stream()
   {
   }

   bool mem_stream::open(const void* p, uint64 size)
   {
      assert(p);

      close();

      if ((!p) || (!size))
         return false;

      m_opened = true;
      m_pBuf = (uint8*)(p);
      m_size = size;
      m_ofs = 0;
      m_attribs = cDataStreamSeekable | cDataStreamReadable;
      return true;
   }

   bool mem_stream::open(void* p, uint64 size)
   {
      assert(p);

      close();

      if ((!p) || (!size))
         return false;

      m_opened = true;
      m_pBuf = static_cast<uint8*>(p);
      m_size = size;
      m_ofs = 0;
      m_attribs = cDataStreamSeekable | cDataStreamWritable | cDataStreamReadable;
      return true;
   }

   bool mem_stream::close()
   {
      if (m_opened)
      {
         m_opened = false;
         m_pBuf = NULL;
         m_size = 0;
         m_ofs = 0;
         return true;
      }

      return false;
   }
      
   uint mem_stream::read(void* pBuf, uint len)
   {
      if ((!m_opened) || (!is_readable()) || (!len))
         return 0;

      assert(m_ofs <= m_size);

      uint64 bytes_left = m_size - m_ofs;

      len = (uint)LZHAM_EX_MIN(len, bytes_left);

      if (len)
      {
         memcpy(pBuf, &m_pBuf[m_ofs], len);

         m_ofs += len;
      }

      return len;
   }

   uint mem_stream::write(const void* pBuf, uint len)
   {
      if ((!m_opened) || (!is_writable()) || (!len))
         return 0;

      assert(m_ofs <= m_size);

      uint64 bytes_left = m_size - m_ofs;

      len = (uint)LZHAM_EX_MIN(len, bytes_left);

      if (len)
      {
         memcpy(&m_pBuf[m_ofs], pBuf, len);

         m_ofs += len;
      }

      return len;
   }

   bool mem_stream::flush()
   {
      if (!m_opened)
         return false;

      return true;
   }

   uint64 mem_stream::get_size()
   {
      if (!m_opened)
         return 0;

      return m_size;
   }

   uint64 mem_stream::get_remaining()
   {
      if (!m_opened)
         return 0;

      assert(m_ofs <= m_size);

      return m_size - m_ofs;
   }

   uint64 mem_stream::get_ofs()
   {
      if (!m_opened)
         return 0;

      return m_ofs;
   }

   bool mem_stream::seek(int64 ofs, bool relative)
   {
      if ((!m_opened) || (!is_seekable()))
         return false;

      int64 new_ofs = relative ? (m_ofs + ofs) : ofs;

      if (new_ofs < 0)
         return false;
      else if (new_ofs > (int64)m_size)
         return false;

      m_ofs = (uint64)new_ofs;

      post_seek();

      return true;
   }

} // namespace lzham_ex

