// ZipAddCommon.cpp

#include "StdAfx.h"

#include "../../../../C/7zCrc.h"
#include "../../../../C/Alloc.h"

#include "../../../Windows/PropVariant.h"

#include "../../ICoder.h"
#include "../../IPassword.h"
#include "../../MyVersion.h"

#include "../../Common/CreateCoder.h"
#include "../../Common/StreamObjects.h"
#include "../../Common/StreamUtils.h"

#include "../../Compress/LzmaEncoder.h"
#include "../../Compress/PpmdZip.h"
#include "../../Compress/XzEncoder.h"
#include "../../Compress/ZstdEncoder.h"
#include "../../Compress/PKImplodeEncoder.h"

#include "../Common/InStreamWithCRC.h"

#include "ZipAddCommon.h"
#include "ZipHeader.h"

namespace NArchive {
namespace NZip {

using namespace NFileHeader;


static const UInt32 kLzmaPropsSize = 5;
static const UInt32 kLzmaHeaderSize = 4 + kLzmaPropsSize;

class CLzmaEncoder:
  public ICompressCoder,
  public ICompressSetCoderProperties,
  public ICompressSetCoderPropertiesOpt,
  public CMyUnknownImp
{
public:
  NCompress::NLzma::CEncoder *EncoderSpec;
  CMyComPtr<ICompressCoder> Encoder;
  Byte Header[kLzmaHeaderSize];

  STDMETHOD(Code)(ISequentialInStream *inStream, ISequentialOutStream *outStream,
      const UInt64 *inSize, const UInt64 *outSize, ICompressProgressInfo *progress);
  STDMETHOD(SetCoderProperties)(const PROPID *propIDs, const PROPVARIANT *props, UInt32 numProps);
  STDMETHOD(SetCoderPropertiesOpt)(const PROPID *propIDs, const PROPVARIANT *props, UInt32 numProps);

  MY_UNKNOWN_IMP2(
      ICompressSetCoderProperties,
      ICompressSetCoderPropertiesOpt)
};

STDMETHODIMP CLzmaEncoder::SetCoderProperties(const PROPID *propIDs, const PROPVARIANT *props, UInt32 numProps)
{
  if (!Encoder)
  {
    EncoderSpec = new NCompress::NLzma::CEncoder;
    Encoder = EncoderSpec;
  }
  CBufPtrSeqOutStream *outStreamSpec = new CBufPtrSeqOutStream;
  CMyComPtr<ISequentialOutStream> outStream(outStreamSpec);
  outStreamSpec->Init(Header + 4, kLzmaPropsSize);
  RINOK(EncoderSpec->SetCoderProperties(propIDs, props, numProps));
  RINOK(EncoderSpec->WriteCoderProperties(outStream));
  if (outStreamSpec->GetPos() != kLzmaPropsSize)
    return E_FAIL;
  Header[0] = MY_VER_MAJOR;
  Header[1] = MY_VER_MINOR;
  Header[2] = kLzmaPropsSize;
  Header[3] = 0;
  return S_OK;
}

STDMETHODIMP CLzmaEncoder::SetCoderPropertiesOpt(const PROPID *propIDs, const PROPVARIANT *props, UInt32 numProps)
{
  return EncoderSpec->SetCoderPropertiesOpt(propIDs, props, numProps);
}

STDMETHODIMP CLzmaEncoder::Code(ISequentialInStream *inStream, ISequentialOutStream *outStream,
    const UInt64 *inSize, const UInt64 *outSize, ICompressProgressInfo *progress)
{
  RINOK(WriteStream(outStream, Header, kLzmaHeaderSize));
  return Encoder->Code(inStream, outStream, inSize, outSize, progress);
}


CAddCommon::CAddCommon(const CCompressionMethodMode &options):
    _options(options),
    _copyCoderSpec(NULL),
    _cryptoStreamSpec(NULL),
    _buf(NULL),
    _isLzmaEos(false)
    {}

CAddCommon::~CAddCommon()
{
  MidFree(_buf);
}

static const UInt32 kBufSize = ((UInt32)1 << 16);

HRESULT CAddCommon::CalcStreamCRC(ISequentialInStream *inStream, UInt32 &resultCRC)
{
  if (!_buf)
  {
    _buf = (Byte *)MidAlloc(kBufSize);
    if (!_buf)
      return E_OUTOFMEMORY;
  }

  UInt32 crc = CRC_INIT_VAL;
  for (;;)
  {
    UInt32 processed;
    RINOK(inStream->Read(_buf, kBufSize, &processed));
    if (processed == 0)
    {
      resultCRC = CRC_GET_DIGEST(crc);
      return S_OK;
    }
    crc = CrcUpdate(crc, _buf, (size_t)processed);
  }
}


HRESULT CAddCommon::Set_Pre_CompressionResult(bool inSeqMode, bool outSeqMode, UInt64 unpackSize,
    CCompressingResult &opRes) const
{
  // We use Zip64, if unPackSize size is larger than 0xF8000000 to support
  // cases when compressed size can be about 3% larger than uncompressed size

  const UInt32 kUnpackZip64Limit = 0xF8000000;
  
  opRes.UnpackSize = unpackSize;
  opRes.PackSize = (UInt64)1 << 60; // we use big value to force Zip64 mode.

  if (unpackSize < kUnpackZip64Limit)
    opRes.PackSize = (UInt32)0xFFFFFFFF - 1; // it will not use Zip64 for that size

  if (opRes.PackSize < unpackSize)
    opRes.PackSize = unpackSize;

  Byte method = _options.MethodSequence[0];

  if (method == NCompressionMethod::kStore && !_options.PasswordIsDefined)
    opRes.PackSize = unpackSize;

  opRes.CRC = 0;

  opRes.LzmaEos = false;

  opRes.ExtractVersion = NCompressionMethod::kExtractVersion_Default;
  opRes.DescriptorMode = outSeqMode;
  
  if (_options.PasswordIsDefined)
  {
    opRes.ExtractVersion = NCompressionMethod::kExtractVersion_ZipCrypto;
    if (_options.IsAesMode)
      opRes.ExtractVersion = NCompressionMethod::kExtractVersion_Aes;
    else
    {
      if (inSeqMode)
        opRes.DescriptorMode = true;
    }
  }
  
  opRes.Method = method;
  Byte ver = 0;
  
  switch (method)
  {
    case NCompressionMethod::kStore: break;
    case NCompressionMethod::kDeflate: ver = NCompressionMethod::kExtractVersion_Deflate; break;
    case NCompressionMethod::kDeflate64: ver = NCompressionMethod::kExtractVersion_Deflate64; break;
    case NCompressionMethod::kXz   : ver = NCompressionMethod::kExtractVersion_Xz; break;
    case NCompressionMethod::kPPMd : ver = NCompressionMethod::kExtractVersion_PPMd; break;
    case NCompressionMethod::kBZip2: ver = NCompressionMethod::kExtractVersion_BZip2; break;
    case NCompressionMethod::kZstd: ver = NCompressionMethod::kExtractVersion_Zstd; break;
    case NCompressionMethod::kPKImploding: ver = NCompressionMethod::kExtractVersion_PKImploding; break;
    case NCompressionMethod::kLZMA :
    {
      ver = NCompressionMethod::kExtractVersion_LZMA;
      const COneMethodInfo *oneMethodMain = &_options._methods[0];
      opRes.LzmaEos = oneMethodMain->Get_Lzma_Eos();
      break;
    }
  }
  if (opRes.ExtractVersion < ver)
    opRes.ExtractVersion = ver;

  return S_OK;
}


HRESULT CAddCommon::Compress(
    DECL_EXTERNAL_CODECS_LOC_VARS
    ISequentialInStream *inStream, IOutStream *outStream,
    bool inSeqMode, bool outSeqMode,
    UInt32 fileTime, UInt64 expectedDataSize,
    ICompressProgressInfo *progress, CCompressingResult &opRes)
{
  opRes.LzmaEos = false;

  if (!inStream)
  {
    // We can create empty stream here. But it was already implemented in caller code in 9.33+
    return E_INVALIDARG;
  }

  CSequentialInStreamWithCRC *inSecCrcStreamSpec = new CSequentialInStreamWithCRC;
  CMyComPtr<ISequentialInStream> inCrcStream = inSecCrcStreamSpec;
  
  CMyComPtr<IInStream> inStream2;
  if (!inSeqMode)
  {
    inStream->QueryInterface(IID_IInStream, (void **)&inStream2);
    if (!inStream2)
    {
      // inSeqMode = true;
      // inSeqMode must be correct before
      return E_FAIL;
    }
  }

  inSecCrcStreamSpec->SetStream(inStream);
  inSecCrcStreamSpec->Init();

  unsigned numTestMethods = _options.MethodSequence.Size();

  bool descriptorMode = outSeqMode;
  if (!outSeqMode)
    if (inSeqMode && _options.PasswordIsDefined && !_options.IsAesMode)
      descriptorMode = true;
  opRes.DescriptorMode = descriptorMode;

  if (numTestMethods > 1)
    if (inSeqMode || outSeqMode || !inStream2)
      numTestMethods = 1;

  UInt32 crc = 0;
  bool crc_IsCalculated = false;
  
  Byte method = 0;
  CFilterCoder::C_OutStream_Releaser outStreamReleaser;
  opRes.ExtractVersion = NCompressionMethod::kExtractVersion_Default;
  
  for (unsigned i = 0; i < numTestMethods; i++)
  {
    opRes.LzmaEos = false;
    opRes.ExtractVersion = NCompressionMethod::kExtractVersion_Default;
    
    if (i != 0)
    {
      if (inStream2)
      {
        inSecCrcStreamSpec->Init();
        RINOK(inStream2->Seek(0, STREAM_SEEK_SET, NULL));
      }

      RINOK(outStream->SetSize(0));
      RINOK(outStream->Seek(0, STREAM_SEEK_SET, NULL));
    }
    
    if (_options.PasswordIsDefined)
    {
      opRes.ExtractVersion = NCompressionMethod::kExtractVersion_ZipCrypto;

      if (!_cryptoStream)
      {
        _cryptoStreamSpec = new CFilterCoder(true);
        _cryptoStream = _cryptoStreamSpec;
      }
      
      if (_options.IsAesMode)
      {
        opRes.ExtractVersion = NCompressionMethod::kExtractVersion_Aes;
        if (!_cryptoStreamSpec->Filter)
        {
          _cryptoStreamSpec->Filter = _filterAesSpec = new NCrypto::NWzAes::CEncoder;
          _filterAesSpec->SetKeyMode(_options.AesKeyMode);
          RINOK(_filterAesSpec->CryptoSetPassword((const Byte *)(const char *)_options.Password, _options.Password.Len()));
        }
        RINOK(_filterAesSpec->WriteHeader(outStream));
      }
      else
      {
        if (!_cryptoStreamSpec->Filter)
        {
          _cryptoStreamSpec->Filter = _filterSpec = new NCrypto::NZip::CEncoder;
          _filterSpec->CryptoSetPassword((const Byte *)(const char *)_options.Password, _options.Password.Len());
        }
        
        UInt32 check;
        
        if (descriptorMode)
        {
          // it's Info-ZIP modification for stream_mode descriptor_mode (bit 3 of the general purpose bit flag is set)
          check = (fileTime & 0xFFFF);
        }
        else
        {
          if (!crc_IsCalculated)
          {
            RINOK(CalcStreamCRC(inStream, crc));
            crc_IsCalculated = true;
            RINOK(inStream2->Seek(0, STREAM_SEEK_SET, NULL));
            inSecCrcStreamSpec->Init();
          }
          check = (crc >> 16);
        }
        
        RINOK(_filterSpec->WriteHeader_Check16(outStream, (UInt16)check));
      }
      
      RINOK(_cryptoStreamSpec->SetOutStream(outStream));
      RINOK(_cryptoStreamSpec->InitEncoder());
      outStreamReleaser.FilterCoder = _cryptoStreamSpec;
    }

    method = _options.MethodSequence[i];
    
    switch (method)
    {
      case NCompressionMethod::kStore:
      {
        if (descriptorMode)
        {
          // we still can create descriptor_mode archives with "Store" method, but they are not good for 100%
          return E_NOTIMPL;
        }

        if (!_copyCoderSpec)
        {
          _copyCoderSpec = new NCompress::CCopyCoder;
          _copyCoder = _copyCoderSpec;
        }
        CMyComPtr<ISequentialOutStream> outStreamNew;
        if (_options.PasswordIsDefined)
          outStreamNew = _cryptoStream;
        else
          outStreamNew = outStream;
        RINOK(_copyCoder->Code(inCrcStream, outStreamNew, NULL, NULL, progress));
        break;
      }
      
      default:
      {
        if (!_compressEncoder)
        {
          CLzmaEncoder *_lzmaEncoder = NULL;
          if (method == NCompressionMethod::kLZMA)
          {
            _compressExtractVersion = NCompressionMethod::kExtractVersion_LZMA;
            _lzmaEncoder = new CLzmaEncoder();
            _compressEncoder = _lzmaEncoder;
          }
          else if (method == NCompressionMethod::kZstd)
          {
            _compressExtractVersion = NCompressionMethod::kExtractVersion_Zstd;
            NCompress::NZSTD::CEncoder *encoder = new NCompress::NZSTD::CEncoder();
            _compressEncoder = encoder;
          }
          else if (method == NCompressionMethod::kXz)
          {
            _compressExtractVersion = NCompressionMethod::kExtractVersion_Xz;
            NCompress::NXz::CEncoder *encoder = new NCompress::NXz::CEncoder();
            _compressEncoder = encoder;
          }
          else if (method == NCompressionMethod::kPPMd)
          {
            _compressExtractVersion = NCompressionMethod::kExtractVersion_PPMd;
            NCompress::NPpmdZip::CEncoder *encoder = new NCompress::NPpmdZip::CEncoder();
            _compressEncoder = encoder;
          }
          else if (method == NCompressionMethod::kPKImploding)
          {
            _compressExtractVersion = NCompressionMethod::kExtractVersion_PKImploding;
            NCompress::NPKImplode::NEncoder::CEncoder *encoder = new NCompress::NPKImplode::NEncoder::CEncoder();
            _compressEncoder = encoder;
          }
          else
          {
          CMethodId methodId;
          switch (method)
          {
            case NCompressionMethod::kBZip2:
              methodId = kMethodId_BZip2;
              _compressExtractVersion = NCompressionMethod::kExtractVersion_BZip2;
              break;
            default:
              _compressExtractVersion = ((method == NCompressionMethod::kDeflate64) ?
                  NCompressionMethod::kExtractVersion_Deflate64 :
                  NCompressionMethod::kExtractVersion_Deflate);
              methodId = kMethodId_ZipBase + method;
              break;
          }
          RINOK(CreateCoder(
              EXTERNAL_CODECS_LOC_VARS
              methodId, true, _compressEncoder));
          if (!_compressEncoder)
            return E_NOTIMPL;

          if (method == NCompressionMethod::kDeflate ||
              method == NCompressionMethod::kDeflate64)
          {
          }
          else if (method == NCompressionMethod::kBZip2)
          {
          }
          }
          {
            CMyComPtr<ICompressSetCoderProperties> setCoderProps;
            _compressEncoder.QueryInterface(IID_ICompressSetCoderProperties, &setCoderProps);
            if (setCoderProps)
            {
              if (!_options._methods.IsEmpty())
              {
                COneMethodInfo *oneMethodMain = &_options._methods[0];

                RINOK(oneMethodMain->SetCoderProps(setCoderProps,
                    _options._dataSizeReduceDefined ? &_options._dataSizeReduce : NULL));
              }
            }
          }
          if (method == NCompressionMethod::kLZMA)
            _isLzmaEos = _lzmaEncoder->EncoderSpec->IsWriteEndMark();
        }

        if (method == NCompressionMethod::kLZMA)
          opRes.LzmaEos = _isLzmaEos;

        CMyComPtr<ISequentialOutStream> outStreamNew;
        if (_options.PasswordIsDefined)
          outStreamNew = _cryptoStream;
        else
          outStreamNew = outStream;
        if (_compressExtractVersion > opRes.ExtractVersion)
          opRes.ExtractVersion = _compressExtractVersion;

        {
          CMyComPtr<ICompressSetCoderPropertiesOpt> optProps;
          _compressEncoder->QueryInterface(IID_ICompressSetCoderPropertiesOpt, (void **)&optProps);
          if (optProps)
          {
            PROPID propID = NCoderPropID::kExpectedDataSize;
            NWindows::NCOM::CPropVariant prop = (UInt64)expectedDataSize;
            RINOK(optProps->SetCoderPropertiesOpt(&propID, &prop, 1));
          }
        }
        
        RINOK(_compressEncoder->Code(inCrcStream, outStreamNew, NULL, NULL, progress));
        break;
      }
    }

    if (_options.PasswordIsDefined)
    {
      RINOK(_cryptoStreamSpec->OutStreamFinish());
      
      if (_options.IsAesMode)
      {
        RINOK(_filterAesSpec->WriteFooter(outStream));
      }
    }
    
    RINOK(outStream->Seek(0, STREAM_SEEK_CUR, &opRes.PackSize));

    {
      opRes.CRC = inSecCrcStreamSpec->GetCRC();
      opRes.UnpackSize = inSecCrcStreamSpec->GetSize();
    }

    if (_options.PasswordIsDefined)
    {
      if (opRes.PackSize < opRes.UnpackSize +
          (_options.IsAesMode ? _filterAesSpec->GetAddPackSize() : NCrypto::NZip::kHeaderSize))
        break;
    }
    else if (opRes.PackSize < opRes.UnpackSize)
      break;
  }


  opRes.Method = method;
  return S_OK;
}

}}
