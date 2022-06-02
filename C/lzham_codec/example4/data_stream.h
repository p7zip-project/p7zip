// File: data_stream.h
// LZHAM is in the Public Domain. Please see the Public Domain declaration at the end of include/lzham.h
#pragma once
#include "stream_common.h"

namespace lzham_ex
{
   enum data_stream_attribs
   {
      cDataStreamReadable = 1,
      cDataStreamWritable = 2,
      cDataStreamSeekable = 4
   };
   
   const int64 DATA_STREAM_SIZE_UNKNOWN = LZHAM_INT64_MAX;
   const int64 DATA_STREAM_SIZE_INFINITE = LZHAM_UINT64_MAX;
   
   class data_stream
   {
      data_stream(const data_stream&);
      data_stream& operator= (const data_stream&);
      
   public:
      data_stream();
      data_stream(const char* pName, uint attribs);
      
      virtual ~data_stream() { }
      
      virtual data_stream *get_parent() { return NULL; }
      
      virtual bool close() { m_opened = false; m_error = false; m_got_cr = false; return true; }

      typedef uint16 attribs_t;                              
      inline attribs_t get_attribs() const { return m_attribs; }
      
      inline bool is_opened() const { return m_opened; }
      
      inline bool is_readable() const { return (m_attribs & cDataStreamReadable) != 0; }
      inline bool is_writable() const { return (m_attribs & cDataStreamWritable) != 0; }
      inline bool is_seekable() const { return (m_attribs & cDataStreamSeekable) != 0; }
            
      inline bool get_error() const { return m_error; }
      inline void set_error() { m_error = true; }
      inline void clear_error() { m_error = false; }
      
      inline const std::string& get_name() const { return m_name; }
      inline void set_name(const char* pName) { m_name = pName; }
                              
      virtual uint read(void* pBuf, uint len) = 0;
      virtual uint64 skip(uint64 len);
      
      virtual uint write(const void* pBuf, uint len) = 0;
      virtual bool flush() = 0;
      
      virtual bool is_size_known() const { return true; }
      
      // get_size() and get_remaining() return DATA_STREAM_SIZE_UNKNOWN if size hasn't been determined yet. 
      // For infinite streams, DATA_STREAM_SIZE_INFINITE is returned.
      virtual uint64 get_size() = 0;
      virtual uint64 get_remaining() = 0;
            
      virtual uint64 get_ofs() = 0;
      virtual bool seek(int64 ofs, bool relative) = 0;
      
      virtual const void* get_ptr() const { return NULL; }
      
      inline int read_byte() { uint8 c; if (read(&c, 1) != 1) return -1; return c; }
      inline bool write_byte(uint8 c) { return write(&c, 1) == 1; }
      
      bool read_line(std::string& str);
      bool write_line(const std::string& str);

      bool printf(const char* p, ...);
                  
      // Reads remaining stream into byte array. Max size is 1GB.
      bool read_array(std::vector<uint8>& buf);
      
      // Writes entire byte array to stream.
      bool write_array(const std::vector<uint8>& buf);
                                 
   protected:
      std::string    m_name;
      
      attribs_t      m_attribs;
      bool           m_opened : 1;
      bool           m_error : 1;
      bool           m_got_cr : 1;
                  
      inline void post_seek() { m_got_cr = false; }
   };
      
} // namespace lzham_ex

