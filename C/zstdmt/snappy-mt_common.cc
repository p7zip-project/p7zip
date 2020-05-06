
#include "snappy-mt.h"

/* ****************************************
 * SNAPPY Error Management
 ******************************************/

/**
 * SNAPPYMT_isError() - tells if a return value is an error code
 */
unsigned SNAPPYMT_isError(size_t code)
{
	return (code > MT_ERROR(maxCode));
}

/**
 * SNAPPYMT_getErrorString() - give our error code string of result
 */
const char *SNAPPYMT_getErrorString(size_t code)
{
	static const char *noErrorCode = "Unspecified brotli error code";

	switch ((SNAPPYMT_ErrorCode) (0 - code)) {
	case PREFIX(no_error):
		return "No error detected";
	case PREFIX(memory_allocation):
		return "Allocation error : not enough memory";
	case PREFIX(read_fail):
		return "Read failure";
	case PREFIX(write_fail):
		return "Write failure";
	case PREFIX(data_error):
		return "Malformed input";
	case PREFIX(frame_compress):
		return "Could not compress frame at once";
	case PREFIX(frame_decompress):
		return "Could not decompress frame at once";
	case PREFIX(compressionParameter_unsupported):
		return "Compression parameter is out of bound";
	case PREFIX(maxCode):
	default:
		return noErrorCode;
	}
}
