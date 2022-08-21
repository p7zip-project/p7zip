// File: data_stream_serializer.h
// See Copyright Notice and license at the end of include/lzham.h
#pragma once
#include "data_stream.h"

namespace lzham_ex
{
   // little_endian is the endianness of the buffer's data
   template<typename T> 
   inline void write_obj(const T& obj, void* pBuf, bool little_endian)
   {
      const uint8* pSrc = reinterpret_cast<const uint8*>(&obj);
      uint8* pDst = static_cast<uint8*>(pBuf);

      if (LZHAM_EX_LITTLE_ENDIAN_CPU == little_endian)
         memcpy(pDst, pSrc, sizeof(T));
      else
      {
         for (uint i = 0; i < sizeof(T); i++)
            pDst[i] = pSrc[sizeof(T) - 1 - i];
      }            
   }

   template<typename T>
   inline bool write_obj(const T& obj, void*& pBuf, uint& buf_size, bool little_endian)
   {
      if (buf_size < sizeof(T))
         return false;

      write_obj(obj, pBuf, little_endian);

      pBuf = static_cast<uint8*>(pBuf) + sizeof(T);
      buf_size -= sizeof(T);

      return true;
   }

   // little_endian is the endianness of the buffer's data
   template<typename T> 
   inline void read_obj(T& obj, const void* pBuf, bool little_endian)
   {
      const uint8* pSrc = reinterpret_cast<const uint8*>(pBuf);
      uint8* pDst = reinterpret_cast<uint8*>(&obj);

      if (LZHAM_EX_LITTLE_ENDIAN_CPU == little_endian)
         memcpy(pDst, pSrc, sizeof(T));
      else
      {
         for (uint i = 0; i < sizeof(T); i++)
            pDst[i] = pSrc[sizeof(T) - 1 - i];
      }            
   }

   template<typename T>
   inline bool read_obj(T& obj, const void*& pBuf, uint& buf_size, bool little_endian)
   {
      if (buf_size < sizeof(T))
      {
         zero_object(obj);
         return false;
      }

      read_obj(obj, pBuf, little_endian);

      pBuf = static_cast<const uint8*>(pBuf) + sizeof(T);
      buf_size -= sizeof(T);

      return true;
   }
   
   // Data serialization helper class.
   // Defaults to little endian mode.
   class data_stream_serializer
   {
   public:
      inline data_stream_serializer() : m_pStream(NULL), m_little_endian(true) { }
      inline data_stream_serializer(data_stream* pStream) : m_pStream(pStream), m_little_endian(true) { }
      inline data_stream_serializer(data_stream& stream) : m_pStream(&stream), m_little_endian(true) { }
      inline data_stream_serializer(const data_stream_serializer& other) : m_pStream(other.m_pStream), m_little_endian(other.m_little_endian) { }
      
      inline data_stream_serializer& operator= (const data_stream_serializer& rhs) { m_pStream = rhs.m_pStream; m_little_endian = rhs.m_little_endian; return *this; }

      inline data_stream* get_stream() const { return m_pStream; }
      inline void set_stream(data_stream* pStream) { m_pStream = pStream; }

      inline bool get_error() { return m_pStream ? m_pStream->get_error() : false; }
      
      inline bool get_little_endian() const { return m_little_endian; }
      inline void set_little_endian(bool little_endian) { m_little_endian = little_endian; }
      
      inline bool write(const void* pBuf, uint len)
      {
         return m_pStream->write(pBuf, len) == len;
      }
                  
      inline bool read(void* pBuf, uint len)
      {
         return m_pStream->read(pBuf, len) == len;
      }
      
      inline bool write_chars(const char* pBuf, uint len)
      {
         return write(pBuf, len);
      }
      
      inline bool read_chars(char* pBuf, uint len)
      {
         return read(pBuf, len);
      }
      
      inline bool skip(uint len)
      {
         return m_pStream->skip(len) == len;
      }
      
      template<typename T>
      inline bool write_object(const T& obj)
      {
         if (m_little_endian == LZHAM_EX_LITTLE_ENDIAN_CPU)
            return write(&obj, sizeof(obj));
         else
         {
            uint8 buf[sizeof(T)];
            uint buf_size = sizeof(T);
            void* pBuf = buf;
            write_obj(obj, pBuf, buf_size, m_little_endian);
            
            return write(buf, sizeof(T));
         }
      }
      
      template<typename T>
      inline bool read_object(T& obj)
      {
         if (m_little_endian == LZHAM_EX_LITTLE_ENDIAN_CPU)
            return read(&obj, sizeof(obj));
         else
         {
            uint8 buf[sizeof(T)];
            if (!read(buf, sizeof(T)))
               return false;
            
            uint buf_size = sizeof(T);
            const void* pBuf = buf;
            read_obj(obj, pBuf, buf_size, m_little_endian);
            
            return true;
         }
      }
         
      template<typename T>
      inline bool write_value(T value)
      {
         return write_object(value);
      }
      
      template<typename T>
      inline T read_value(const T& on_error_value = T())
      {
         T result;
         if (!read_object(result))
            result = on_error_value;
         return result;
      }
      
      template<typename T>
      inline bool write_enum(T e)
      {
         int val = static_cast<int>(e);
         return write_object(val);
      }

      template<typename T>
      inline T read_enum()
      {
         return static_cast<T>(read_value<int>());
      }
      
      // Writes uint using a simple variable length code (VLC).
      inline bool write_uint_vlc(uint val)
      {
         do
         {
            uint8 c = static_cast<uint8>(val) & 0x7F;
            if (val <= 0x7F)
               c |= 0x80;
               
            if (!write_value(c))
               return false;
            
            val >>= 7;
         } while (val);
         
         return true;
      }
      
      // Reads uint using a simple variable length code (VLC).
      inline bool read_uint_vlc(uint& val)
      {
         val = 0;
         uint shift = 0;
         
         for ( ; ; )
         {
            if (shift >= 32)
            {
               val = 0;             
               return false;
            }
               
            uint8 c;
            if (!read_object(c))
            {
               val = 0;
               return false;
            }
            
            val |= ((c & 0x7F) << shift);
            shift += 7;
            
            if (c & 0x80)
               break;
         }
         
         return true;
      }
      
      inline bool write_c_str(const char* p)
      {
         uint len = static_cast<uint>(strlen(p));
         if (!write_uint_vlc(len))
            return false;
         
         return write_chars(p, len);
      }
      
      inline bool read_c_str(char* pBuf, uint buf_size)
      {
         uint len;
         if (!read_uint_vlc(len))
         {
            pBuf[0] = '\0';
            return false;
         }
         if ((len + 1) > buf_size)
         {
            pBuf[0] = '\0';
            return false;
         }
         
         pBuf[len] = '\0';
         
         if (!read_chars(pBuf, len))
         {
            pBuf[0] = '\0';
            return false;
         }

         return true;
      }
      
      inline bool write_string(const std::string& str)
      {
         if (!write_uint_vlc(static_cast<uint>(str.length())))
            return false;

         return write_chars(str.data(), static_cast<uint>(str.length()));
      }
      
      inline bool read_string(std::string& str)
      {
         uint len;
         if (!read_uint_vlc(len))
         {
            str.resize(0);
            return false;
         }
         
         str.resize(len);
         
         if (len)
         {
            if (!read_chars(&str[0], len))
            {
               str.resize(0);
               return false;
            }
         }
         
         return true;
      }
                  
      template<typename T>
      inline bool write_vector(const T& vec)
      {
         if (!write_uint_vlc(vec.size()))
            return false;
         
         for (uint i = 0; i < vec.size(); i++)
         {
            *this << vec[i];
            if (get_error())
               return false;
         }
         
         return true;            
      };
      
      template<typename T>
      inline bool read_vector(T& vec, uint num_expected = UINT_MAX)
      {
         uint size;
         if (!read_uint_vlc(size))
         {
            vec.resize(0);
            return false;
         }
         
         if ((size * sizeof(T::value_type)) >= 2U*1024U*1024U*1024U)
         {
            vec.resize(0);
            return false;
         }
         
         if ((num_expected != UINT_MAX) && (size != num_expected))
         {
            vec.resize(0);
            return false;
         }
       
         vec.resize(size);
         for (uint i = 0; i < vec.size(); i++)
         {
            *this >> vec[i];
            
            if (get_error())
            {
               vec.resize(i);
               return false;
            }
         }
         
         return true;
      }
      
      // Got this idea from the Molly Rocket forums.
      // fmt may contain the characters "1", "2", or "4".
      inline bool writef(char *fmt, ...) 
      { 
         va_list v; 
         va_start(v, fmt); 
         
         while (*fmt) 
         { 
            switch (*fmt++) 
            { 
               case '1': 
               { 
                  const uint8 x = static_cast<uint8>(va_arg(v, uint)); 
                  if (!write_value(x))
                     return false;
               } 
               case '2': 
               { 
                  const uint16 x = static_cast<uint16>(va_arg(v, uint)); 
                  if (!write_value(x))
                     return false;
               } 
               case '4': 
               { 
                  const uint32 x = static_cast<uint32>(va_arg(v, uint)); 
                  if (!write_value(x))
                     return false;
               } 
               case ' ':
               case ',':
               {
                  break;
               }
               default: 
               {
                  assert(0); 
                  return false;
               }                  
            } 
         } 
         
         va_end(v); 
         return true;
      } 
      
      // Got this idea from the Molly Rocket forums.
      // fmt may contain the characters "1", "2", or "4".
      inline bool readf(char *fmt, ...) 
      { 
         va_list v; 
         va_start(v, fmt); 

         while (*fmt) 
         { 
            switch (*fmt++) 
            { 
               case '1': 
               { 
                  uint8* x = va_arg(v, uint8*); 
                  assert(x);
                  if (!read_object(*x))
                     return false;
               } 
               case '2': 
               { 
                  uint16* x = va_arg(v, uint16*); 
                  assert(x);
                  if (!read_object(*x))
                     return false;
               } 
               case '4': 
               { 
                  uint32* x = va_arg(v, uint32*);
                  assert(x);
                  if (!read_object(*x))
                     return false;
               }
               case ' ':
               case ',':
               {
                  break;
               }
               default: 
               {
                  assert(0); 
                  return false;
               }                  
            } 
         } 

         va_end(v); 
         return true;
      } 
            
   private:
      data_stream* m_pStream;

      bool m_little_endian;
   };
   
   // Write operators
   inline data_stream_serializer& operator<< (data_stream_serializer& serializer, bool val)           { serializer.write_value(val); return serializer; }
   inline data_stream_serializer& operator<< (data_stream_serializer& serializer, int8 val)           { serializer.write_value(val); return serializer; }
   inline data_stream_serializer& operator<< (data_stream_serializer& serializer, uint8 val)          { serializer.write_value(val); return serializer; }
   inline data_stream_serializer& operator<< (data_stream_serializer& serializer, int16 val)          { serializer.write_value(val); return serializer; }
   inline data_stream_serializer& operator<< (data_stream_serializer& serializer, uint16 val)         { serializer.write_value(val); return serializer; }
   inline data_stream_serializer& operator<< (data_stream_serializer& serializer, int32 val)          { serializer.write_value(val); return serializer; }
   inline data_stream_serializer& operator<< (data_stream_serializer& serializer, uint32 val)         { serializer.write_uint_vlc(val); return serializer; }
   inline data_stream_serializer& operator<< (data_stream_serializer& serializer, int64 val)          { serializer.write_value(val); return serializer; } 
   inline data_stream_serializer& operator<< (data_stream_serializer& serializer, uint64 val)         { serializer.write_value(val); return serializer; } 
   inline data_stream_serializer& operator<< (data_stream_serializer& serializer, long val)           { serializer.write_value(val); return serializer; }
   inline data_stream_serializer& operator<< (data_stream_serializer& serializer, unsigned long val)  { serializer.write_value(val); return serializer; }
   inline data_stream_serializer& operator<< (data_stream_serializer& serializer, float val)          { serializer.write_value(val); return serializer; }
   inline data_stream_serializer& operator<< (data_stream_serializer& serializer, double val)         { serializer.write_value(val); return serializer; }
   inline data_stream_serializer& operator<< (data_stream_serializer& serializer, const char* p)      { serializer.write_c_str(p); return serializer; }
   
   inline data_stream_serializer& operator<< (data_stream_serializer& serializer, const std::string& str)
   {
      serializer.write_string(str);
      return serializer;
   }
   
   template<typename T>
   inline data_stream_serializer& operator<< (data_stream_serializer& serializer, const std::vector<T>& vec)
   {
      serializer.write_vector(vec);
      return serializer;
   }
   
   template<typename T>
   inline data_stream_serializer& operator<< (data_stream_serializer& serializer, const T* p)
   {
      serializer.write_object(*p);
      return serializer;
   }
   
   // Read operators
   inline data_stream_serializer& operator>> (data_stream_serializer& serializer, bool& val)           { val = false; serializer.read_object(val); return serializer; }
   inline data_stream_serializer& operator>> (data_stream_serializer& serializer, int8& val)           { val = 0; serializer.read_object(val); return serializer; }
   inline data_stream_serializer& operator>> (data_stream_serializer& serializer, uint8& val)          { val = 0; serializer.read_object(val); return serializer; }
   inline data_stream_serializer& operator>> (data_stream_serializer& serializer, int16& val)          { val = 0; serializer.read_object(val); return serializer; }
   inline data_stream_serializer& operator>> (data_stream_serializer& serializer, uint16& val)         { val = 0; serializer.read_object(val); return serializer; }
   inline data_stream_serializer& operator>> (data_stream_serializer& serializer, int32& val)          { val = 0; serializer.read_object(val); return serializer; }
   inline data_stream_serializer& operator>> (data_stream_serializer& serializer, uint32& val)         { val = 0; serializer.read_uint_vlc(val); return serializer; }
   inline data_stream_serializer& operator>> (data_stream_serializer& serializer, int64& val)          { val = 0; serializer.read_object(val); return serializer; } 
   inline data_stream_serializer& operator>> (data_stream_serializer& serializer, uint64& val)         { val = 0; serializer.read_object(val); return serializer; } 
   inline data_stream_serializer& operator>> (data_stream_serializer& serializer, long& val)           { val = 0; serializer.read_object(val); return serializer; }
   inline data_stream_serializer& operator>> (data_stream_serializer& serializer, unsigned long& val)  { val = 0; serializer.read_object(val); return serializer; }
   inline data_stream_serializer& operator>> (data_stream_serializer& serializer, float& val)          { val = 0; serializer.read_object(val); return serializer; }
   inline data_stream_serializer& operator>> (data_stream_serializer& serializer, double& val)         { val = 0; serializer.read_object(val); return serializer; }
      
   inline data_stream_serializer& operator>> (data_stream_serializer& serializer, std::string& str)
   {
      serializer.read_string(str);
      return serializer;
   }
   
   template<typename T>
   inline data_stream_serializer& operator>> (data_stream_serializer& serializer, std::vector<T>& vec)
   {
      serializer.read_vector(vec);
      return serializer;
   }
   
   template<typename T>
   inline data_stream_serializer& operator>> (data_stream_serializer& serializer, T* p)
   {
      serializer.read_object(*p);
      return serializer;
   }

} // namespace lzham_ex
