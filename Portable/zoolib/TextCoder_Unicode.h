// Copyright (c) 2002 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZooLib_TextCoder_Unicode_h__
#define __ZooLib_TextCoder_Unicode_h__ 1
#include "zconfig.h"

#include "zoolib/TextCoder.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - 

ZP<TextDecoder> sMake_TextDecoder_Unicode(const std::string& iSourceName);

ZP<TextEncoder> sMake_TextEncoder_Unicode(const std::string& iDestName);

// =================================================================================================
#pragma mark - TextDecoder_Unicode_AutoDetect

/** \brief A TextDecoder that treats the source data as being UTF-8, UTF-16BE or UTF-16LE based
on the detection of a BOM (Byte Order Mark) at the beginning of the data. */

class TextDecoder_Unicode_AutoDetect : public TextDecoder
	{
public:
	TextDecoder_Unicode_AutoDetect();

	// From TextDecoder (Callable)
	virtual ZQ<bool> QCall(
		const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
		UTF32* oDest, size_t iDestCU, size_t* oDestCU);

	// Our protocol
	void Reset();

private:
	enum EType { eFresh, eUTF8, eUTF16BE, eUTF16LE };
	EType fType;
	};

} // namespace ZooLib

#endif // __ZooLib_TextCoder_Unicode_h__
