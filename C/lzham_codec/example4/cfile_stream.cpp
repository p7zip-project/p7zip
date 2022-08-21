// File: cfile_stream.cpp
// See Copyright Notice and license at the end of include/lzham.h
#include "cfile_stream.h"

namespace lzham_ex
{
   cfile_stream::cfile_stream() : 
      data_stream(), m_pFile(NULL), m_size(0), m_ofs(0), m_has_ownership(false)
   {
   }

   cfile_stream::cfile_stream(FILE* pFile, const char* pFilename, uint attribs, bool has_ownership) :
      data_stream(), m_pFile(NULL), m_size(0), m_ofs(0), m_has_ownership(false)
   {
      open(pFile, pFilename, attribs, has_ownership);
   }

   cfile_stream::cfile_stream(const char* pFilename, uint attribs, bool open_existing) :
      data_stream(), m_pFile(NULL), m_size(0), m_ofs(0), m_has_ownership(false)
   {
      open(pFilename, attribs, open_existing);
   }

   cfile_stream::~cfile_stream()
   {
      close();
   }

   bool cfile_stream::close()
   {
      clear_error();

      if (m_opened)
      {
         bool status = true;
         if (m_has_ownership)
         {
            if (EOF == fclose(m_pFile))
               status = false;
         }

         m_pFile = NULL;
         m_opened = false;
         m_size = 0;
         m_ofs = 0;
         m_has_ownership = false;

         return status;
      }

      return false;
   }

   bool cfile_stream::open(FILE* pFile, const char* pFilename, uint attribs, bool has_ownership)
   {
      assert(pFile);
      assert(pFilename);

      close();

      set_name(pFilename);
      m_pFile = pFile;
      m_has_ownership = has_ownership;
      m_attribs = static_cast<uint16>(attribs);

      m_ofs = _ftelli64(m_pFile);
      _fseeki64(m_pFile, 0, SEEK_END);
      m_size = _ftelli64(m_pFile);
      _fseeki64(m_pFile, m_ofs, SEEK_SET);

      m_opened = true;

      return true;
   }

   bool cfile_stream::open(const char* pFilename, uint attribs, bool open_existing)
   {
      assert(pFilename);

      close();

      m_attribs = static_cast<uint16>(attribs);

      const char* pMode;
      if ((is_readable()) && (is_writable()))
         pMode = open_existing ? "r+b" : "w+b";
      else if (is_writable())
         pMode = open_existing ? "ab" : "wb";
      else if (is_readable())
         pMode = "rb";
      else
      {
         set_error();
         return false;
      }

      FILE* pFile = NULL;
#ifdef _MSC_VER
      fopen_s(&pFile, pFilename, pMode);
#else
      pFile = fopen(pFilename, pMode);
#endif
      m_has_ownership = true;

      if (!pFile)
      {
         set_error();
         return false;
      }

      return open(pFile, pFilename, attribs, true);
   }
   
   uint cfile_stream::read(void* pBuf, uint len)
   {
      assert(pBuf && (len <= 0x7FFFFFFF));

      if (!m_opened || (!is_readable()) || (!len))
         return 0;

      len = static_cast<uint>(LZHAM_EX_MIN(len, get_remaining()));

      if (fread(pBuf, 1, len, m_pFile) != len)
      {
         set_error();
         return 0;
      }

      m_ofs += len;
      return len;
   }

   uint cfile_stream::write(const void* pBuf, uint len)
   {
      assert(pBuf && (len <= 0x7FFFFFFF));

      if (!m_opened || (!is_writable()) || (!len))
         return 0;

      size_t n = fwrite(pBuf, 1, len, m_pFile);

      m_ofs += n;
      m_size = LZHAM_EX_MAX(m_size, m_ofs);

      if (n != len)
         set_error();

      return static_cast<uint>(n);
   }

   bool cfile_stream::flush()
   {
      if ((!m_opened) || (!is_writable()))
         return false;

      if (EOF == fflush(m_pFile))
      {
         set_error();
         return false;
      }

      return true;
   }

   uint64 cfile_stream::get_size()
   {
      if (!m_opened)
         return 0;

      return m_size;
   }

   uint64 cfile_stream::get_remaining()
   {
      if (!m_opened)
         return 0;

      assert(m_ofs <= m_size);
      return m_size - m_ofs;
   }

   uint64 cfile_stream::get_ofs()
   {
      if (!m_opened)
         return 0;

      return m_ofs;
   }

   bool cfile_stream::seek(int64 ofs, bool relative)
   {
      if ((!m_opened) || (!is_seekable()))
         return false;

      int64 new_ofs = relative ? (m_ofs + ofs) : ofs;
      if (new_ofs < 0)
         return false;
      else if (static_cast<uint64>(new_ofs) > m_size)
         return false;

      if (static_cast<uint64>(new_ofs) != m_ofs)
      {
         if (_fseeki64(m_pFile, new_ofs, SEEK_SET) != 0)
         {
            set_error();
            return false;
         }

         m_ofs = new_ofs;
      }

      return true;
   }

   bool cfile_stream::read_file_into_array(const char* pFilename, std::vector<uint8>& buf)
   {
      cfile_stream in_stream(pFilename);
      if (!in_stream.is_opened())
         return false;
      return in_stream.read_array(buf);
   }

   bool cfile_stream::write_array_to_file(const char* pFilename, const std::vector<uint8>& buf)
   {
      cfile_stream out_stream(pFilename, cDataStreamWritable|cDataStreamSeekable);
      if (!out_stream.is_opened())
         return false;
      return out_stream.write_array(buf);
   }

} // namespace lzham_ex
