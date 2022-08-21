// File: comp_stream.cpp
// See Copyright Notice and license at the end of include/lzham.h
#include "comp_stream.h"

namespace lzham_ex
{
   compression_stream::compression_stream() :
      data_stream(),
      m_pComp_state(NULL),
      m_total_uncomp_size(0),
      m_total_comp_size(0),
      m_total_promised_uncomp_size(DATA_STREAM_SIZE_UNKNOWN),
      m_header_stream_ofs(0),
      m_pOutput_stream(NULL),
      m_opened(false)
   {
      m_hdr.clear();
   }
   
   compression_stream::compression_stream(data_stream *pOutput_stream, const lzham_compress_params *pComp_params, const char* pName, uint window_size, lzham_compress_level level, bool multithreading, uint64 source_stream_size) :
      data_stream(),
      m_pComp_state(NULL),
      m_total_uncomp_size(0),
      m_total_comp_size(0),
      m_total_promised_uncomp_size(DATA_STREAM_SIZE_UNKNOWN),
      m_header_stream_ofs(0),
      m_pOutput_stream(NULL),
      m_opened(false)
   {
      m_hdr.clear();
      
      open(pOutput_stream, pComp_params, pName, window_size, level, multithreading, source_stream_size);
   }
   
   compression_stream::~compression_stream()
   {
      close();
   }

   bool compression_stream::open(data_stream *pOutput_stream, const lzham_compress_params *pComp_params, const char* pName, uint window_size, lzham_compress_level level, bool multithreading, uint64 source_stream_size)
   {
      if (m_opened)
         return false;
         
      if (!pOutput_stream)
         return false;
      
      m_name = pName ? pName : "comp_stream";
      m_attribs = cDataStreamWritable;
      
      m_pOutput_stream = pOutput_stream;
      m_total_promised_uncomp_size = source_stream_size;
      lzham_compress_params lzham_params;
      zero_object(lzham_params);
      
      if (!pComp_params)
      {
         lzham_params.m_struct_size = sizeof(lzham_params);
         lzham_params.m_max_helper_threads = multithreading ? -1 : 0;
         lzham_params.m_dict_size_log2 = window_size;
         lzham_params.m_level = level;
         pComp_params = &lzham_params;
      }
      m_pComp_state = lzham_compress_init(pComp_params);
      if (!m_pComp_state)
      {
         clear();
         return false;
      }
      
      m_buf.resize(cBufSize);
      
      m_header_stream_ofs = m_pOutput_stream->get_ofs();
      
      m_hdr.clear();
      m_hdr.m_comp_size = DATA_STREAM_SIZE_UNKNOWN;
      m_hdr.m_uncomp_size = source_stream_size;
      m_hdr.m_method = compressed_stream_header::cCompMethodLZHAM;
      m_hdr.m_window_size = static_cast<uint8>(pComp_params->m_dict_size_log2);
      m_hdr.compute_crc();
      
      if (pOutput_stream->write(&m_hdr, sizeof(m_hdr)) != sizeof(m_hdr))
      {
         clear();
         return false;  
      }
      
      m_opened = true;
         
      return true;
   }
   
   bool compression_stream::flush_output_buf(uint out_buf_size)
   {
      if (!out_buf_size)
         return true;
         
      assert(out_buf_size <= UINT16_MAX);
      assert(cBufSize <= UINT16_MAX);

      if (!m_pOutput_stream->is_seekable())
      {
         if (!m_pOutput_stream->write_byte(static_cast<uint8>(out_buf_size))) return false;
         if (!m_pOutput_stream->write_byte(static_cast<uint8>(out_buf_size >> 8))) return false;
         m_total_comp_size += 2;
      }

      uint64 bytes_written = m_pOutput_stream->write(&m_buf[0], static_cast<uint>(out_buf_size));
      m_total_comp_size += bytes_written;

      return bytes_written == out_buf_size;
   }
   
   bool compression_stream::close()
   {
      if (!m_opened)
         return true;
                  
      if ( (m_error) || 
           ((m_total_promised_uncomp_size != DATA_STREAM_SIZE_UNKNOWN) && (m_total_uncomp_size != m_total_promised_uncomp_size)) )
      {
         clear();
         data_stream::close();
         return false;
      }
      
      bool success = true;
      lzham_compress_status_t status;
      do
      {
         size_t in_buf_size = 0;
         size_t out_buf_size = cBufSize;
         
         status = lzham_compress(m_pComp_state, NULL, &in_buf_size, &m_buf[0], &out_buf_size, true);
                  
         if (out_buf_size)
         {
            if (!flush_output_buf(static_cast<uint>(out_buf_size)))
            {
               success = false;
               break;
            }
         }
         
      } while (status < LZHAM_COMP_STATUS_FIRST_SUCCESS_OR_FAILURE_CODE);
      
      if ((success) && (status == LZHAM_COMP_STATUS_SUCCESS))
      {
         if (!m_pOutput_stream->is_seekable())
         {
            if (!m_pOutput_stream->write_byte(0)) success = false;
            if (!m_pOutput_stream->write_byte(0)) success = false;
            m_total_comp_size += 2;
         }
         else
         {
            uint64 cur_ofs = m_pOutput_stream->get_ofs();
            if (!m_pOutput_stream->seek(m_header_stream_ofs, false))
            {
               success = false;
            }
            else
            {
               m_hdr.m_comp_size = m_total_comp_size;
               m_hdr.m_uncomp_size = m_total_uncomp_size;
               m_hdr.compute_crc();
               assert(m_hdr.validate());
                              
               if (m_pOutput_stream->write(&m_hdr, sizeof(m_hdr)) != sizeof(m_hdr))
               {
                  success = false;
               }
               
               m_pOutput_stream->seek(cur_ofs, false);
            }
         }
      }
      else
      {
         success = false;
      }
                  
      clear();
      data_stream::close();
      
      return success;
   }
      
   uint compression_stream::read(void* pBuf, uint len)
   {
      pBuf, len;
      return 0;
   }
     
   uint compression_stream::write(const void* pBuf, uint len)
   {
      if ((!m_opened) || (!len) || (m_error))
         return 0;
            
      if (m_total_promised_uncomp_size != DATA_STREAM_SIZE_UNKNOWN) 
      {
         if ((m_total_uncomp_size + len) > m_total_promised_uncomp_size)
         {
            m_error = true;
            return 0;
         }
      }
      
      m_total_uncomp_size += len;
      
      lzham_compress_status_t status;
      uint buf_ofs = 0;
      
      do
      {
         size_t in_buf_size = len - buf_ofs;
         size_t out_buf_size = cBufSize;

         status = lzham_compress(m_pComp_state, static_cast<const uint8*>(pBuf) + buf_ofs, &in_buf_size, &m_buf[0], &out_buf_size, false);
                                    
         if (out_buf_size)
         {
            if (!flush_output_buf(static_cast<uint>(out_buf_size)))
            {
               m_error = true;
               return 0;
            }
         }
                  
         buf_ofs += static_cast<uint>(in_buf_size);
         
      } while ((buf_ofs < len) && (status < LZHAM_COMP_STATUS_FIRST_SUCCESS_OR_FAILURE_CODE));
      
      if (status >= LZHAM_COMP_STATUS_FIRST_SUCCESS_OR_FAILURE_CODE)
      {
         m_error = true;
         return 0;
      }
      
      return len;
   }
   
   bool compression_stream::flush()
   {
      if ((!m_opened) || (m_error))
         return false;
      return true;
   }

   uint64 compression_stream::get_size()
   {
      if (!m_opened)
         return 0;
      return m_pOutput_stream->get_size();
   }
   
   uint64 compression_stream::get_remaining()
   {
      return 0;
   }
   
   // Should this return the number of original/source bytes? Or the current output stream's size?
   uint64 compression_stream::get_ofs()
   {
      if (!m_opened)
         return 0; 
      //return m_pOutput_stream->get_ofs();
      return m_total_uncomp_size;
   }

   bool compression_stream::seek(int64 ofs, bool relative)
   {
      ofs, relative;
      return false;
   }
   
   void compression_stream::clear()
   {
      if (m_pComp_state)
      {
         lzham_compress_deinit(m_pComp_state);
         m_pComp_state = NULL;
      }

      m_buf.clear();
      
      m_total_uncomp_size = 0;
      m_total_comp_size = 0;
      m_total_promised_uncomp_size = DATA_STREAM_SIZE_UNKNOWN;
      m_header_stream_ofs = 0;
      m_pOutput_stream = NULL;
      m_hdr.clear();
      
      m_opened = false;
   }
   
   //------------------------------------------------------------------------------------------------------------------
   
   decompression_stream::decompression_stream() :
      m_pInput_stream(NULL),
      m_pDecomp_state(NULL),
      m_buf_ofs(0),
      m_chunk_size(0),
      m_no_more_input_bytes(0),
      m_total_bytes_read(0),
      m_total_bytes_unpacked(0),
      m_decomp_status(LZHAM_DECOMP_STATUS_NOT_FINISHED),
      m_decomp_completed(false),
      m_comp_size_known(false),
      m_opened(false)
   {
      m_hdr.clear();
   }
   
   decompression_stream::decompression_stream(data_stream *pInput_stream, const char* pName) :
      m_pInput_stream(NULL),
      m_pDecomp_state(NULL),
      m_buf_ofs(0),
      m_chunk_size(0),
      m_no_more_input_bytes(0),
      m_total_bytes_read(0),
      m_total_bytes_unpacked(0),
      m_decomp_status(LZHAM_DECOMP_STATUS_NOT_FINISHED),
      m_decomp_completed(false),
      m_comp_size_known(false),
      m_opened(false)
   {
      open(pInput_stream, pName);
   }
   
   decompression_stream::~decompression_stream()
   {
      close();
   }
   
   bool decompression_stream::open(data_stream *pInput_stream, const char* pName)
   {
      if (m_opened)
         return false;
      
      if (!pInput_stream)
         return false;

      m_name = pName ? pName : "comp_stream";
      m_attribs = cDataStreamWritable;

      m_pInput_stream = pInput_stream;
      if (m_pInput_stream->get_remaining() < sizeof(compressed_stream_header))
      {
         clear();
         return false;
      }
      
      if (m_pInput_stream->read(&m_hdr, sizeof(m_hdr)) != sizeof(m_hdr))
      {
         clear();
         return false;
      }
      
      if (!m_hdr.validate())
      {
         clear();
         return false;
      }
      
      if ((!m_hdr.m_comp_size) || (m_hdr.m_method != compressed_stream_header::cCompMethodLZHAM))
      {
         clear();
         return false;
      }
      
      if ((m_hdr.m_window_size < LZHAM_MIN_DICT_SIZE_LOG2) || (m_hdr.m_window_size > LZHAM_MAX_DICT_SIZE_LOG2_X64))
      {
         clear();
         return false;
      }
      
      m_comp_size_known = (m_hdr.m_comp_size != DATA_STREAM_SIZE_UNKNOWN);
      if (m_comp_size_known)
      {
         if (m_pInput_stream->get_remaining() < m_hdr.m_comp_size)
         {
            clear();
            return false;
         }
      }
                       
      m_buf.resize(UINT16_MAX);
      
      m_opened = true;

      return true;
   }
   
   bool decompression_stream::close()
   {
      if (!m_opened)
         return true;
      
      bool status = !m_error;
   
      clear();
      data_stream::close();
      
      return status;
   }
   
   bool decompression_stream::refill_input_buffer()
   {
      m_buf_ofs = 0;
      m_chunk_size = 0;
      
      if (m_no_more_input_bytes) 
         return true;
            
      if (m_comp_size_known)
      {
         uint64 comp_bytes_remaining = m_hdr.m_comp_size - m_total_bytes_read;
         m_chunk_size = static_cast<uint>(LZHAM_EX_MIN(comp_bytes_remaining, m_buf.size()));
         
         if (m_hdr.m_comp_size == (m_total_bytes_read + m_chunk_size))
         {
            m_no_more_input_bytes = true;
         }
      }
      else
      {
         if (m_pInput_stream->get_remaining() < sizeof(uint16))
         {
            return false;
         }
         
         m_chunk_size = m_pInput_stream->read_byte();
         m_chunk_size |= (m_pInput_stream->read_byte() << 8);
         
         if (!m_chunk_size)
         {
            m_no_more_input_bytes = true;
         }
      }
          
      if (m_chunk_size)
      {
         if (m_pInput_stream->get_remaining() < m_chunk_size)
         {
            return false;
         }
         
         assert(m_buf.size() >= m_chunk_size);
         
         if (m_pInput_stream->read(&m_buf[0], m_chunk_size) != m_chunk_size)
         {
            return false;
         }
         
         m_total_bytes_read += m_chunk_size;
      }
      
      return true;
   }
   
   bool decompression_stream::term_stream()
   {
      if (m_decomp_status != LZHAM_DECOMP_STATUS_SUCCESS)
      {
         // Decompression failed.
         return false;
      }

      if (get_remaining())
      {
         // Decompressor says we're done, but there are still bytes left to decompress.
         return false;
      }

      if (m_buf_ofs < m_chunk_size)
      {
         // We haven't consumed the entire expected input stream.
         return false;
      }

      if (!m_no_more_input_bytes)
      {
         // We haven't consumed the entire input stream.
         if (m_comp_size_known)
         {
            if (m_total_bytes_read != m_hdr.m_comp_size)
            {
               return false;
            }
         }
         else
         {
            // Read last chunk's size, which should be 0.
            if (m_pInput_stream->get_remaining() < sizeof(uint16))
            {
               return false;
            }

            m_chunk_size = m_pInput_stream->read_byte();
            m_chunk_size |= (m_pInput_stream->read_byte() << 8);
            if (m_chunk_size)
            {
               // We should be at the end of the compressed stream, but there's more bytes following - this is an error.
               return false;
            }
         }
      }
      
      return true;
   }

   uint decompression_stream::read(void* pBuf, uint len)
   {
      if ((!m_opened) || (m_error) || (!len))
         return 0;
      
      len = static_cast<uint>(LZHAM_EX_MIN(len, get_remaining()));
      if (!len)
         return 0;
      
      if (!m_pDecomp_state)
      {
         lzham_decompress_params lzham_params;
         zero_object(lzham_params);
         lzham_params.m_struct_size = sizeof(lzham_params);
         lzham_params.m_dict_size_log2 = m_hdr.m_window_size;
         lzham_params.m_decompress_flags = LZHAM_DECOMP_FLAG_COMPUTE_ADLER32;
                     
         if ((m_hdr.m_uncomp_size != DATA_STREAM_SIZE_UNKNOWN) && (len >= m_hdr.m_uncomp_size))
         {
            lzham_params.m_decompress_flags |= LZHAM_DECOMP_FLAG_OUTPUT_UNBUFFERED;
         }
         
         m_pDecomp_state = lzham_decompress_init(&lzham_params);
         if (!m_pDecomp_state)
         {
            m_error = true;
            return 0;
         }
      }
      
      uint out_buf_ofs = 0;
      
      while (out_buf_ofs < len)
      {
         if ((m_buf_ofs >= m_chunk_size) && (!m_no_more_input_bytes))
         {
            if (!refill_input_buffer())
            {
               m_error = true;
               return out_buf_ofs;
            }
         }
            
         size_t in_buf_size = m_chunk_size - m_buf_ofs;
         size_t out_buf_size = len - out_buf_ofs;
         
         m_decomp_status = lzham_decompress(m_pDecomp_state, 
            &m_buf[0] + m_buf_ofs, &in_buf_size,
            static_cast<uint8*>(pBuf) + out_buf_ofs, &out_buf_size,
            m_no_more_input_bytes);
         
         m_buf_ofs += static_cast<uint>(in_buf_size);
         out_buf_ofs += static_cast<uint>(out_buf_size);
         
         m_total_bytes_unpacked += out_buf_size;
         
         if ((m_hdr.m_uncomp_size != DATA_STREAM_SIZE_UNKNOWN) && (m_total_bytes_unpacked > m_hdr.m_uncomp_size))
         {
            // Decompressor's output was too large.
            m_error = true;
            return out_buf_ofs;  
         }
                  
         if (m_decomp_status >= LZHAM_DECOMP_STATUS_FIRST_SUCCESS_OR_FAILURE_CODE)
         {
            if (!term_stream())
            {
               m_error = true;
               return out_buf_ofs;
            }
                                                
            break;
         }
      }
      
      return out_buf_ofs;
   }
   
   uint decompression_stream::write(const void* pBuf, uint len)
   {
      pBuf, len;
      return 0;
   }
   
   bool decompression_stream::flush()
   {
      if ((!m_opened) || (m_error))
         return false;
      return true;
   }

   uint64 decompression_stream::get_size()
   {
      if ((!m_opened) || (m_error))
         return false;
      
      uint64 uncomp_size = m_hdr.m_uncomp_size;
      if ((uncomp_size == DATA_STREAM_SIZE_UNKNOWN) && (m_decomp_status == LZHAM_DECOMP_STATUS_SUCCESS))
      {
         return m_total_bytes_unpacked;
      }

      return uncomp_size;
   }
   
   uint64 decompression_stream::get_remaining()
   {
      if ((!m_opened) || (m_error))
         return 0;
      
      uint64 uncomp_size = m_hdr.m_uncomp_size;
      if ((uncomp_size == DATA_STREAM_SIZE_UNKNOWN) && (m_decomp_status == LZHAM_DECOMP_STATUS_SUCCESS))
      {
         return 0;
      }
           
      return uncomp_size - m_total_bytes_unpacked;
   }
   
   uint64 decompression_stream::get_ofs()
   {
      if ((!m_opened) || (m_error))
         return false;
      return m_total_bytes_unpacked;
   }

   bool decompression_stream::seek(int64 ofs, bool relative)
   {
      ofs, relative;
      return false;
   }
   
   void decompression_stream::clear()
   {
      m_pInput_stream = NULL;

      m_hdr.clear();

      if ( m_pDecomp_state )
      {
         lzham_decompress_deinit( m_pDecomp_state );
         m_pDecomp_state = NULL;
      }

      m_buf.clear();
      m_buf_ofs = 0;

      m_chunk_size = 0;
      m_no_more_input_bytes = false;

      m_total_bytes_read = 0;
      m_total_bytes_unpacked = 0;
      
      m_decomp_status = LZHAM_DECOMP_STATUS_NOT_FINISHED;
      
      m_comp_size_known = false;
      m_opened = false;
   }

} // namespace lzham_ex

