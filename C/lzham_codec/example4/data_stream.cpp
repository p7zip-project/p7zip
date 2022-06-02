// File: data_stream.cpp
// LZHAM is in the Public Domain. Please see the Public Domain declaration at the end of include/lzham.h
#include "data_stream.h"
#include <stdarg.h>

namespace lzham_ex
{
   data_stream::data_stream() :
      m_attribs(0),
      m_opened(false), m_error(false), m_got_cr(false)
   {
   }

   data_stream::data_stream(const char* pName, uint attribs) :
      m_name(pName),
      m_attribs(static_cast<uint16>(attribs)),
      m_opened(false), m_error(false), m_got_cr(false)
   {
   }

   uint64 data_stream::skip(uint64 len)
   {
      uint64 total_bytes_read = 0;

      const uint cBufSize = 1024;
      uint8 buf[cBufSize];

      while (len)
      {
         const uint64 bytes_to_read = LZHAM_EX_MIN(sizeof(buf), len);
         const uint64 bytes_read = read(buf, static_cast<uint>(bytes_to_read));
         total_bytes_read += bytes_read;

         if (bytes_read != bytes_to_read)
            break;

         len -= bytes_read;
      }

      return total_bytes_read;
   }

   bool data_stream::read_line(std::string& str)
   {
      str.resize(0);

      for ( ; ; )
      {
         const int c = read_byte();

         const bool prev_got_cr = m_got_cr;
         m_got_cr = false;

         if (c < 0)
         {
            if (!str.empty())
               break;

            return false;
         }
         else if ((26 == c) || (!c))
            continue;
         else if (13 == c)
         {
            m_got_cr = true;
            break;
         }
         else if (10 == c)
         {
            if (prev_got_cr)
               continue;

            break;
         }

         str.append(1, static_cast<char>(c));
      }

      return true;
   }

   bool data_stream::printf(const char* p, ...)
   {
      va_list args;

      va_start(args, p);
      char buf[4096];
#ifdef _MSC_VER
      int l = vsprintf_s(buf, sizeof(buf), p, args);
#else
      int l = vsprintf(buf, p, args);
#endif
      va_end(args);
      if (l < 0)
         return false;
      return write(buf, l) == static_cast<uint>(l);
   }
   
   bool data_stream::write_line(const std::string& str)
   {
      if (!str.empty())
         return write(str.c_str(), static_cast<uint>(str.length())) == str.length();

      return true;
   }
   
   bool data_stream::read_array(std::vector<uint8>& buf)
   {
      if (get_remaining() > 1024U*1024U*1024U)
         return false;

      uint32 bytes_to_read = static_cast<uint32>(get_remaining());
      if (!bytes_to_read)
      {
         buf.resize(0);
         return true;
      }

      if (buf.size() < bytes_to_read)
         buf.resize(bytes_to_read);
      
      return read(&buf[0], bytes_to_read) == bytes_to_read;
   }

   bool data_stream::write_array(const std::vector<uint8>& buf)
   {
      size_t cur_ofs = 0;

      while (cur_ofs < buf.size())
      {
         size_t num_remaining = buf.size() - cur_ofs;
         uint32 n = static_cast<uint32>(LZHAM_EX_MIN(256U*1024U*1024U, num_remaining));

         if (write(&buf[cur_ofs], n) != n)
            return false;

         cur_ofs += n;
      }

      return true;
   }

} // namespace lzham_ex
