// File: comp_stream.h
// See Copyright Notice and license at the end of include/lzham.h
#include "data_stream.h"

#include "lzham.h"

namespace lzham_ex
{
   template<typename T>
   class packed_value
   {
   public:
      packed_value() { }
      packed_value(T val) { *this = val; }

      inline operator T() const
      {
         T result = 0;
         for (int i = sizeof(T) - 1; i >= 0; i--)
            result = static_cast<T>((result << 8) | m_bytes[i]);
         return result;
      }

      packed_value& operator= (T val)
      {
         for (int i = 0; i < sizeof(T); i++)
         {
            m_bytes[i] = static_cast<uint8>(val);
            val >>= 8;
         }
         return *this;
      }            

   private:
      uint8 m_bytes[sizeof(T)];
   };

#pragma pack(push)
#pragma pack(1) 
   struct compressed_stream_header
   {
      enum { cSig = 0x41485a4c };
      packed_value<uint32> m_sig;
      
      packed_value<uint64> m_uncomp_size;
      packed_value<uint64> m_comp_size;
      
      enum 
      {
         cCompMethodLZHAM = 0,
      };
      packed_value<uint8> m_method;
      packed_value<uint8> m_window_size;
      
      packed_value<uint32> m_header_crc;
      
      void clear() { memset(this, 0, sizeof(*this)); }
      void compute_crc() { m_sig = (uint32)cSig; m_header_crc = lzham_z_crc32(LZHAM_Z_CRC32_INIT, (uint8*)this, sizeof(*this) - sizeof(uint32)); }
      bool validate() const { return (m_sig == (uint32)cSig) && (m_header_crc == lzham_z_crc32(LZHAM_Z_CRC32_INIT, (uint8*)this, sizeof(*this) - sizeof(uint32))); }
   };
#pragma pack(pop)   

   class compression_stream : public data_stream
   {
   public:
      compression_stream();
      compression_stream(data_stream *pOutput_stream, const lzham_compress_params *pComp_params = NULL, const char* pName = "compression_stream", uint window_size = 19, lzham_compress_level level = LZHAM_COMP_LEVEL_DEFAULT, bool multithreading = false, uint64 source_stream_size = DATA_STREAM_SIZE_UNKNOWN);
      virtual ~compression_stream();
      
      virtual data_stream *get_output_stream() { return m_pOutput_stream; }

      virtual bool open(data_stream *pOutput_stream, const lzham_compress_params *pComp_params = NULL, const char* pName = "compression_stream", uint window_size = 19, lzham_compress_level level = LZHAM_COMP_LEVEL_DEFAULT, bool multithreading = false, uint64 source_stream_size = DATA_STREAM_SIZE_UNKNOWN);
      virtual bool close();
   
      virtual uint read(void* pBuf, uint len);
      virtual uint write(const void* pBuf, uint len);
      virtual bool flush();

      virtual uint64 get_size();
      virtual uint64 get_remaining();
      virtual uint64 get_ofs();
      
      virtual bool seek(int64 ofs, bool relative);
   
   private:
      data_stream *m_pOutput_stream;
      
      lzham_compress_state_ptr m_pComp_state;
      
      uint64 m_total_uncomp_size;
      uint64 m_total_comp_size;
      
      uint64 m_total_promised_uncomp_size;
      
      uint64 m_header_stream_ofs;
      
      enum { cBufSize = 65535 };
      std::vector<uint8> m_buf;
            
      compressed_stream_header m_hdr;
      
      bool m_opened;
      
      void clear();
      bool flush_output_buf(uint out_buf_size);
   };
   
   class decompression_stream : public data_stream
   {
   public:
      decompression_stream();
      decompression_stream(data_stream *pInput_stream, const char* pName = "decompression_stream");
      virtual ~decompression_stream();

      virtual data_stream *get_input_stream() { return m_pInput_stream; }

      virtual bool open(data_stream *pInput_stream, const char* pName = "decompression_stream");
      virtual bool close();

      virtual uint read(void* pBuf, uint len);
      virtual uint write(const void* pBuf, uint len);
      virtual bool flush();

      virtual uint64 get_size();
      virtual uint64 get_remaining();
      virtual uint64 get_ofs();

      virtual bool seek(int64 ofs, bool relative);
   
   private:
      data_stream *m_pInput_stream;
   
      compressed_stream_header m_hdr;
                  
      lzham_decompress_state_ptr m_pDecomp_state;
      
      std::vector<uint8> m_buf;
      uint m_buf_ofs;
      
      uint m_chunk_size;
      bool m_no_more_input_bytes;
                  
      uint64 m_total_bytes_read;
      uint64 m_total_bytes_unpacked;
      
      lzham_decompress_status_t m_decomp_status;
      
      bool m_decomp_completed;
      bool m_comp_size_known;
      bool m_opened;
      
      void clear();
      bool refill_input_buffer();
      bool term_stream();
   };

} // namespace lzham_ex
