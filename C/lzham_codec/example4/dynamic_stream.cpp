// File: dynamic_stream.cpp
// See Copyright Notice and license at the end of include/lzham.h
#include "dynamic_stream.h"      

namespace lzham_ex
{
   dynamic_stream::dynamic_stream(uint64 initial_size, const char* pName, uint attribs) : 
      data_stream(pName, attribs),
      m_ofs(0)
   {
      open(initial_size, pName, attribs);
   }

   dynamic_stream::dynamic_stream(const void* pBuf, uint64 size, const char* pName, uint attribs) : 
      data_stream(pName, attribs),
      m_ofs(0)
   {
      open(pBuf, size, pName, attribs);
   }

   dynamic_stream::dynamic_stream() : 
      data_stream(),
      m_ofs(0)
   {
      open();
   }

   dynamic_stream::~dynamic_stream()
   {
   }

   bool dynamic_stream::open(uint64 initial_size, const char* pName, uint attribs)
   {
      if (initial_size > static_cast<size_t>(-1))
         return false;

      close();

      m_opened = true;
      m_buf.clear();
      m_buf.resize(static_cast<size_t>(initial_size));
      m_ofs = 0;
      m_name = pName ? pName : "dynamic_stream";
      m_attribs = static_cast<attribs_t>(attribs);
      return true;
   }

   bool dynamic_stream::reopen(const char* pName, uint attribs)
   {
      if (!m_opened)
      {
         return open(0, pName, attribs);
      }

      m_name = pName ? pName : "dynamic_stream";
      m_attribs = static_cast<attribs_t>(attribs);
      return true;
   }

   bool dynamic_stream::open(const void* pBuf, uint64 size, const char* pName, uint attribs)
   {
      if (!m_opened)
      {
         if (size > static_cast<size_t>(-1))
            return false;

         m_opened = true;
         m_buf.resize(static_cast<size_t>(size));
         if (size)
            memcpy(&m_buf[0], pBuf, static_cast<size_t>(size));
         m_ofs = 0;
         m_name = pName ? pName : "dynamic_stream";
         m_attribs = static_cast<attribs_t>(attribs);
         return true;
      }

      return false;
   }

   bool dynamic_stream::close()
   {
      if (m_opened)
      {
         m_opened = false;
         m_buf.clear();
         m_ofs = 0;
         return true;
      }

      return false;
   }
   
   bool dynamic_stream::reserve(uint64 size)
   {
      if (m_opened)
      {
         if (size > static_cast<size_t>(-1))
            return false;
         m_buf.reserve(static_cast<size_t>(size));
      }
      return true;
   }
   
   uint dynamic_stream::read(void* pBuf, uint len)
   {
      if ((!m_opened) || (!is_readable()) || (!len))
         return 0;

      assert(m_ofs <= m_buf.size());

      uint64 bytes_left = m_buf.size() - m_ofs;

      len = static_cast<uint>(LZHAM_EX_MIN(len, bytes_left));

      if (len)
      {
         memcpy(pBuf, &m_buf[static_cast<size_t>(m_ofs)], len);

         m_ofs += len;
      }

      return len;
   }

   uint dynamic_stream::write(const void* pBuf, uint len)
   {
      if ((!m_opened) || (!is_writable()) || (!len))
         return 0;

      assert(m_ofs <= m_buf.size());

      uint64 new_ofs = m_ofs + len;
      if (new_ofs > m_buf.size())
      {
         if (new_ofs > static_cast<size_t>(-1))
            return false;
         m_buf.resize(static_cast<size_t>(new_ofs));
      }

      memcpy(&m_buf[static_cast<size_t>(m_ofs)], pBuf, len);
      m_ofs = new_ofs;

      return len;
   }

   bool dynamic_stream::flush()
   {
      if (!m_opened)
         return false;

      return true;
   }

   uint64 dynamic_stream::get_size() 
   { 
      if (!m_opened)
         return 0;

      return m_buf.size(); 
   }

   uint64 dynamic_stream::get_remaining()
   {
      if (!m_opened)
         return 0;

      assert(m_ofs <= m_buf.size());

      return m_buf.size() - m_ofs;
   }

   uint64 dynamic_stream::get_ofs() 
   {
      if (!m_opened)
         return 0;

      return m_ofs;
   }

   bool dynamic_stream::seek(int64 ofs, bool relative) 
   {
      if ((!m_opened) || (!is_seekable()))
         return false;

      int64 new_ofs = relative ? (m_ofs + ofs) : ofs;

      if (new_ofs < 0)
         return false;
      else if (new_ofs > static_cast<int64>(m_buf.size()))
         return false;

      m_ofs = static_cast<uint64>(new_ofs);

      post_seek();

      return true;
   }

} // namespace lzham_ex
