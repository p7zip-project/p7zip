// Archive/ZipItem.cpp

#if (!defined _WIN32) && (!defined __CYGWIN__) && (!defined __APPLE__)
#include <iconv.h>
#include <locale.h>
#endif

#include "StdAfx.h"

#include "../../../../C/CpuArch.h"
#include "../../../../C/7zCrc.h"

#include "../../../Common/IntToString.h"
#include "../../../Common/MyLinux.h"
#include "../../../Common/StringConvert.h"

#include "../../../Windows/PropVariantUtils.h"

#include "../Common/ItemNameUtils.h"

#include "ZipItem.h"

namespace NArchive {
namespace NZip {

using namespace NFileHeader;

static const CUInt32PCharPair g_ExtraTypes[] =
{
  { NExtraID::kZip64, "Zip64" },
  { NExtraID::kNTFS, "NTFS" },
  { NExtraID::kStrongEncrypt, "StrongCrypto" },
  { NExtraID::kUnixTime, "UT" },
  { NExtraID::kUnixExtra, "UX" },
  { NExtraID::kIzUnicodeComment, "uc" },
  { NExtraID::kIzUnicodeName, "up" },
  { NExtraID::kWzAES, "WzAES" }
};

void CExtraSubBlock::PrintInfo(AString &s) const
{
  for (unsigned i = 0; i < ARRAY_SIZE(g_ExtraTypes); i++)
  {
    const CUInt32PCharPair &pair = g_ExtraTypes[i];
    if (pair.Value == ID)
    {
      s += pair.Name;
      return;
    }
  }
  {
    char sz[32];
    sz[0] = '0';
    sz[1] = 'x';
    ConvertUInt32ToHex(ID, sz + 2);
    s += sz;
  }
}


void CExtraBlock::PrintInfo(AString &s) const
{
  if (Error)
    s.Add_OptSpaced("Extra_ERROR");

  if (MinorError)
    s.Add_OptSpaced("Minor_Extra_ERROR");

  if (IsZip64 || IsZip64_Error)
  {
    s.Add_OptSpaced("Zip64");
    if (IsZip64_Error)
      s += "_ERROR";
  }

  FOR_VECTOR (i, SubBlocks)
  {
    s.Add_Space_if_NotEmpty();
    SubBlocks[i].PrintInfo(s);
  }
}


bool CExtraSubBlock::ExtractNtfsTime(unsigned index, FILETIME &ft) const
{
  ft.dwHighDateTime = ft.dwLowDateTime = 0;
  UInt32 size = (UInt32)Data.Size();
  if (ID != NExtraID::kNTFS || size < 32)
    return false;
  const Byte *p = (const Byte *)Data;
  p += 4; // for reserved
  size -= 4;
  while (size > 4)
  {
    UInt16 tag = GetUi16(p);
    unsigned attrSize = GetUi16(p + 2);
    p += 4;
    size -= 4;
    if (attrSize > size)
      attrSize = size;
    
    if (tag == NNtfsExtra::kTagTime && attrSize >= 24)
    {
      p += 8 * index;
      ft.dwLowDateTime = GetUi32(p);
      ft.dwHighDateTime = GetUi32(p + 4);
      return true;
    }
    p += attrSize;
    size -= attrSize;
  }
  return false;
}

bool CExtraSubBlock::ExtractUnixTime(bool isCentral, unsigned index, UInt32 &res) const
{
  res = 0;
  UInt32 size = (UInt32)Data.Size();
  if (ID != NExtraID::kUnixTime || size < 5)
    return false;
  const Byte *p = (const Byte *)Data;
  Byte flags = *p++;
  size--;
  if (isCentral)
  {
    if (index != NUnixTime::kMTime ||
        (flags & (1 << NUnixTime::kMTime)) == 0 ||
        size < 4)
      return false;
    res = GetUi32(p);
    return true;
  }
  for (unsigned i = 0; i < 3; i++)
    if ((flags & (1 << i)) != 0)
    {
      if (size < 4)
        return false;
      if (index == i)
      {
        res = GetUi32(p);
        return true;
      }
      p += 4;
      size -= 4;
    }
  return false;
}


bool CExtraSubBlock::ExtractUnixExtraTime(unsigned index, UInt32 &res) const
{
  res = 0;
  const size_t size = Data.Size();
  unsigned offset = index * 4;
  if (ID != NExtraID::kUnixExtra || size < offset + 4)
    return false;
  const Byte *p = (const Byte *)Data + offset;
  res = GetUi32(p);
  return true;
}


bool CExtraBlock::GetNtfsTime(unsigned index, FILETIME &ft) const
{
  FOR_VECTOR (i, SubBlocks)
  {
    const CExtraSubBlock &sb = SubBlocks[i];
    if (sb.ID == NFileHeader::NExtraID::kNTFS)
      return sb.ExtractNtfsTime(index, ft);
  }
  return false;
}

bool CExtraBlock::GetUnixTime(bool isCentral, unsigned index, UInt32 &res) const
{
  {
    FOR_VECTOR (i, SubBlocks)
    {
      const CExtraSubBlock &sb = SubBlocks[i];
      if (sb.ID == NFileHeader::NExtraID::kUnixTime)
        return sb.ExtractUnixTime(isCentral, index, res);
    }
  }
  
  switch (index)
  {
    case NUnixTime::kMTime: index = NUnixExtra::kMTime; break;
    case NUnixTime::kATime: index = NUnixExtra::kATime; break;
    default: return false;
  }
  
  {
    FOR_VECTOR (i, SubBlocks)
    {
      const CExtraSubBlock &sb = SubBlocks[i];
      if (sb.ID == NFileHeader::NExtraID::kUnixExtra)
        return sb.ExtractUnixExtraTime(index, res);
    }
  }
  return false;
}


bool CLocalItem::IsDir() const
{
  return NItemName::HasTailSlash(Name, GetCodePage());
}

bool CItem::IsDir() const
{
  if (NItemName::HasTailSlash(Name, GetCodePage()))
    return true;
  
  Byte hostOS = GetHostOS();

  if (Size == 0 && PackSize == 0 && !Name.IsEmpty() && Name.Back() == '\\')
  {
    // do we need to use CharPrevExA?
    // .NET Framework 4.5 : System.IO.Compression::CreateFromDirectory() probably writes backslashes to headers?
    // so we support that case
    switch (hostOS)
    {
      case NHostOS::kFAT:
      case NHostOS::kNTFS:
      case NHostOS::kHPFS:
      case NHostOS::kVFAT:
        return true;
    }
  }

  if (!FromCentral)
    return false;
  
  UInt16 highAttrib = (UInt16)((ExternalAttrib >> 16 ) & 0xFFFF);

  switch (hostOS)
  {
    case NHostOS::kAMIGA:
      switch (highAttrib & NAmigaAttrib::kIFMT)
      {
        case NAmigaAttrib::kIFDIR: return true;
        case NAmigaAttrib::kIFREG: return false;
        default: return false; // change it throw kUnknownAttributes;
      }
    case NHostOS::kFAT:
    case NHostOS::kNTFS:
    case NHostOS::kHPFS:
    case NHostOS::kVFAT:
      return ((ExternalAttrib & FILE_ATTRIBUTE_DIRECTORY) != 0);
    case NHostOS::kAtari:
    case NHostOS::kMac:
    case NHostOS::kVMS:
    case NHostOS::kVM_CMS:
    case NHostOS::kAcorn:
    case NHostOS::kMVS:
      return false; // change it throw kUnknownAttributes;
    case NHostOS::kUnix:
      return MY_LIN_S_ISDIR(highAttrib);
    default:
      return false;
  }
}

UInt32 CItem::GetWinAttrib() const
{
  UInt32 winAttrib = 0;
  switch (GetHostOS())
  {
    case NHostOS::kFAT:
    case NHostOS::kNTFS:
      if (FromCentral)
        winAttrib = ExternalAttrib;
      break;
    case NHostOS::kUnix:
      // do we need to clear 16 low bits in this case?
      if (FromCentral)
      {
        /*
          Some programs write posix attributes in high 16 bits of ExternalAttrib
          Also some programs can write additional marker flag:
            0x8000 - p7zip
            0x4000 - Zip in MacOS
            no marker - Info-Zip

          Client code has two options to detect posix field:
            1) check 0x8000 marker. In that case we must add 0x8000 marker here.
            2) check that high 4 bits (file type bits in posix field) of attributes are not zero.
        */
        
        winAttrib = ExternalAttrib & 0xFFFF0000;
        
        // #ifndef _WIN32
        winAttrib |= 0x8000; // add posix mode marker
        // #endif
      }
      break;
  }
  if (IsDir()) // test it;
    winAttrib |= FILE_ATTRIBUTE_DIRECTORY;
  return winAttrib;
}

bool CItem::GetPosixAttrib(UInt32 &attrib) const
{
  // some archivers can store PosixAttrib in high 16 bits even with HostOS=FAT.
  if (FromCentral && GetHostOS() == NHostOS::kUnix)
  {
    attrib = ExternalAttrib >> 16;
    return (attrib != 0);
  }
  attrib = 0;
  if (IsDir())
    attrib = MY_LIN_S_IFDIR;
  return false;
}

void CItem::GetUnicodeString(UString &res, const AString &s, bool isComment, bool useSpecifiedCodePage, UINT codePage) const
{
  bool isUtf8 = IsUtf8();
  bool ignore_Utf8_Errors = true;
  
  if (!isUtf8)
  {
    {
      const unsigned id = isComment ?
          NFileHeader::NExtraID::kIzUnicodeComment:
          NFileHeader::NExtraID::kIzUnicodeName;
      const CObjectVector<CExtraSubBlock> &subBlocks = GetMainExtra().SubBlocks;
      
      FOR_VECTOR (i, subBlocks)
      {
        const CExtraSubBlock &sb = subBlocks[i];
        if (sb.ID == id)
        {
          AString utf;
          if (sb.ExtractIzUnicode(CrcCalc(s, s.Len()), utf))
            if (ConvertUTF8ToUnicode(utf, res))
              return;
          break;
        }
      }
    }
    
    if (useSpecifiedCodePage)
      isUtf8 = (codePage == CP_UTF8);
    #ifdef _WIN32
    else if (GetHostOS() == NFileHeader::NHostOS::kUnix)
    {
      /* Some ZIP archives in Unix use UTF-8 encoding without Utf8 flag in header.
         We try to get name as UTF-8.
         Do we need to do it in POSIX version also? */
      isUtf8 = true;
      ignore_Utf8_Errors = false;
    }
    #endif
  }
  
  #if (!defined _WIN32) && (!defined __CYGWIN__) && (!defined __APPLE__)
  // Convert OEM char set to UTF-8 if needed
  // Use system locale to select code page

  Byte hostOS = GetHostOS();
  if (!isUtf8 && ((hostOS == NFileHeader::NHostOS::kFAT) || (hostOS == NFileHeader::NHostOS::kNTFS))) {

    const char *oemcp;
    oemcp = getenv("OEMCP");
    if (!oemcp) {
      oemcp = "CP437\0"; // CP name is 6 chars max

      const char *lc_to_cp_table[] = {
      "af_ZA", "CP850", "ar_SA", "CP720", "ar_LB", "CP720", "ar_EG", "CP720",
      "ar_DZ", "CP720", "ar_BH", "CP720", "ar_IQ", "CP720", "ar_JO", "CP720",
      "ar_KW", "CP720", "ar_LY", "CP720", "ar_MA", "CP720", "ar_OM", "CP720",
      "ar_QA", "CP720", "ar_SY", "CP720", "ar_TN", "CP720", "ar_AE", "CP720",
      "ar_YE", "CP720","ast_ES", "CP850", "az_AZ", "CP866", "az_AZ", "CP857",
      "be_BY", "CP866", "bg_BG", "CP866", "br_FR", "CP850", "ca_ES", "CP850",
      "zh_CN", "CP936", "zh_TW", "CP950", "kw_GB", "CP850", "cs_CZ", "CP852",
      "cy_GB", "CP850", "da_DK", "CP850", "de_AT", "CP850", "de_LI", "CP850",
      "de_LU", "CP850", "de_CH", "CP850", "de_DE", "CP850", "el_GR", "CP737",
      "en_AU", "CP850", "en_CA", "CP850", "en_GB", "CP850", "en_IE", "CP850",
      "en_JM", "CP850", "en_BZ", "CP850", "en_PH", "CP437", "en_ZA", "CP437",
      "en_TT", "CP850", "en_US", "CP437", "en_ZW", "CP437", "en_NZ", "CP850",
      "es_PA", "CP850", "es_BO", "CP850", "es_CR", "CP850", "es_DO", "CP850",
      "es_SV", "CP850", "es_EC", "CP850", "es_GT", "CP850", "es_HN", "CP850",
      "es_NI", "CP850", "es_CL", "CP850", "es_MX", "CP850", "es_ES", "CP850",
      "es_CO", "CP850", "es_ES", "CP850", "es_PE", "CP850", "es_AR", "CP850",
      "es_PR", "CP850", "es_VE", "CP850", "es_UY", "CP850", "es_PY", "CP850",
      "et_EE", "CP775", "eu_ES", "CP850", "fa_IR", "CP720", "fi_FI", "CP850",
      "fo_FO", "CP850", "fr_FR", "CP850", "fr_BE", "CP850", "fr_CA", "CP850",
      "fr_LU", "CP850", "fr_MC", "CP850", "fr_CH", "CP850", "ga_IE", "CP437",
      "gd_GB", "CP850", "gv_IM", "CP850", "gl_ES", "CP850", "he_IL", "CP862",
      "hr_HR", "CP852", "hu_HU", "CP852", "id_ID", "CP850", "is_IS", "CP850",
      "it_IT", "CP850", "it_CH", "CP850", "iv_IV", "CP437", "ja_JP", "CP932",
      "kk_KZ", "CP866", "ko_KR", "CP949", "ky_KG", "CP866", "lt_LT", "CP775",
      "lv_LV", "CP775", "mk_MK", "CP866", "mn_MN", "CP866", "ms_BN", "CP850",
      "ms_MY", "CP850", "nl_BE", "CP850", "nl_NL", "CP850", "nl_SR", "CP850",
      "nn_NO", "CP850", "nb_NO", "CP850", "pl_PL", "CP852", "pt_BR", "CP850",
      "pt_PT", "CP850", "rm_CH", "CP850", "ro_RO", "CP852", "ru_RU", "CP866",
      "sk_SK", "CP852", "sl_SI", "CP852", "sq_AL", "CP852", "sr_RS", "CP855",
      "sr_RS", "CP852", "sv_SE", "CP850", "sv_FI", "CP850", "sw_KE", "CP437",
      "th_TH", "CP874", "tr_TR", "CP857", "tt_RU", "CP866", "uk_UA", "CP866",
      "ur_PK", "CP720", "uz_UZ", "CP866", "uz_UZ", "CP857", "vi_VN", "CP1258",
      "wa_BE", "CP850", "zh_HK", "CP950", "zh_SG", "CP936"};
      int table_len = sizeof(lc_to_cp_table) / sizeof(char *);
      int lc_len, i;

      char *lc = setlocale(LC_CTYPE, "");

      if (lc && lc[0]) {
          // Compare up to the dot, if it exists, e.g. en_US.UTF-8
          for (lc_len = 0; lc[lc_len] != '.' && lc[lc_len] != '\0'; ++lc_len)
              ;
          for (i = 0; i < table_len; i += 2)
              if (strncmp(lc, lc_to_cp_table[i], lc_len) == 0)
                  oemcp = lc_to_cp_table[i + 1];
      }
    }

    iconv_t cd;
    if ((cd = iconv_open("UTF-8", oemcp)) != (iconv_t)-1) {

      AString s_utf8;
      const char* src = s.Ptr();
      size_t slen = s.Len();
      size_t dlen = slen * 4;
      const char* dest = s_utf8.GetBuf_SetEnd(dlen + 1); // (source length * 4) + null termination

      size_t done = iconv(cd, (char**)&src, &slen, (char**)&dest, &dlen);
      bzero((size_t*)dest + done, 1);

      iconv_close(cd);

      if (ConvertUTF8ToUnicode(s_utf8, res) || ignore_Utf8_Errors)
        return;
    }    
  }
  #endif
  
  if (isUtf8)
    if (ConvertUTF8ToUnicode(s, res) || ignore_Utf8_Errors)
      return;
  
  MultiByteToUnicodeString2(res, s, useSpecifiedCodePage ? codePage : GetCodePage());
}

}}
