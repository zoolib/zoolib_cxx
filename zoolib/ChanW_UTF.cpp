/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------------------------- */

#include "zoolib/ChanW_UTF.h"
#include "zoolib/ZDebug.h"
#include "zoolib/ZUnicode.h"

namespace ZooLib {

static void sThrowEndOfChanW()
	{
	throw std::range_error("sThrowEndOfChanW, UTF");
	}

// =================================================================================================
// MARK: -

static
void spWriteMust(const UTF32* iSource, size_t iCountCU, const ChanW_UTF& iChanW)
	{
	const size_t countWritten = sWriteFully(iSource, iCountCU, iChanW);
	if (countWritten != iCountCU)
		sThrowEndOfChanW();
	}

static
void spWriteMust(const UTF16* iSource, size_t iCountCU, const ChanW_UTF& iChanW)
	{
	if (iCountCU)
		{
		const size_t cuConsumed = sWrite(iSource, iCountCU, iChanW);
		if (cuConsumed != iCountCU)
			{
			// See comments in the UTF-8 variant.
			if (0 != ZUnicode::sCUToCP(iSource + cuConsumed, iSource + iCountCU))
				sThrowEndOfChanW();
			}
		}
	}

static
void spWriteMust(const UTF8* iSource, size_t iCountCU, const ChanW_UTF& iChanW)
	{
	if (iCountCU)
		{
		const size_t cuConsumed = sWrite(iSource, iCountCU, iChanW);
		if (cuConsumed != iCountCU)
			{
			// We weren't able to write the whole string.
			// It may be that the final code units are a valid prefix and thus
			// were not consumed by Imp_WriteUTF8 because it's expecting
			// that the caller will pass the prefix again, this time with enough
			// following code units to make up a complete code point,
			// or with invalid code units which will be consumed and ignored.
			// For string8 (and string16 and string32) we know there's
			// nothing more to come. So if the string contains no more
			// complete valid code points between source + cuConsumed and the
			// end then we can just return. If there are one or more valid
			// code points then the short write must be because we've reached
			// our end, and we throw the end of strim exception.
			if (0 != ZUnicode::sCUToCP(iSource + cuConsumed, iSource + iCountCU))
				sThrowEndOfChanW();
			}
		}
	}

// =================================================================================================
// MARK: -

bool sQWriteCP(UTF32 iCP, const ChanW_UTF& iChanW)
	{ return 1 == sWrite(&iCP, iChanW); }

void sWrite(const UTF32* iSource,
	size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP,
	const ChanW_UTF& iChanW)
	{
	size_t actualCU;
	if (iCountCP >= iCountCU)
		{
		// iCountCP is the same or larger than iCountCU, and thus the limit
		// on what can be written is simply iCountCU because a single code unit
		// maps to at most one code point.
		actualCU = iCountCU;
		}
	else
		{
		// We're being asked to write fewer CPs than we have CUs. So we have to
		// map from CPs to CUs.
		actualCU = std::min(iCountCU, ZUnicode::sCPToCU(iSource, iCountCU, iCountCP, nullptr));
		}

	const size_t cuWritten = sWrite(iSource, actualCU, iChanW);
	if (oCountCU)
		*oCountCU = cuWritten;
	if (oCountCP)
		*oCountCP = ZUnicode::sCUToCP(iSource, cuWritten);
	}

void sWrite(const UTF16* iSource,
	size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP,
	const ChanW_UTF& iChanW)
	{
	size_t actualCU;
	if (iCountCP >= iCountCU)
		{
		// iCountCP is the same or larger than iCountCU, and thus the limit
		// on what can be written is simply iCountCU because a single code unit
		// maps to at most one code point.
		actualCU = iCountCU;
		}
	else
		{
		// We're being asked to write fewer CPs than we have CUs. So we have to
		// map from CPs to CUs.
		actualCU = std::min(iCountCU, ZUnicode::sCPToCU(iSource, iCountCU, iCountCP, nullptr));
		}

	const size_t cuWritten = sWrite(iSource, actualCU, iChanW);
	if (oCountCU)
		*oCountCU = cuWritten;
	if (oCountCP)
		*oCountCP = ZUnicode::sCUToCP(iSource, cuWritten);
	}

void sWrite(const UTF8* iSource,
	size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP,
	const ChanW_UTF& iChanW)
	{
	size_t actualCU;
	if (iCountCP >= iCountCU)
		{
		// iCountCP is the same or larger than iCountCU, and thus the limit
		// on what can be written is simply iCountCU because a single code unit
		// maps to at most one code point.
		actualCU = iCountCU;
		}
	else
		{
		// We're being asked to write fewer CPs than we have CUs. So we have to
		// map from CPs to CUs.
		actualCU = std::min(iCountCU, ZUnicode::sCPToCU(iSource, iCountCU, iCountCP, nullptr));
		}

	const size_t cuWritten = sWrite(iSource, actualCU, iChanW);
	if (oCountCU)
		*oCountCU = cuWritten;
	if (oCountCP)
		*oCountCP = ZUnicode::sCUToCP(iSource, cuWritten);
	}

// ---

size_t sWrite(const UTF16* iSource, size_t iCountCU, const ChanW_UTF& iChanW)
	{ return sNonConst(iChanW).WriteUTF16(iSource, iCountCU); }


size_t sWrite(const UTF8* iSource, size_t iCountCU, const ChanW_UTF& iChanW)
	{ return sNonConst(iChanW).WriteUTF8(iSource, iCountCU); }

// ---

void sWrite(const UTF32* iString, const ChanW_UTF& iChanW)
	{
	if (size_t countCU = ZUnicode::sCountCU(iString))
		spWriteMust(iString, countCU, iChanW);
	}

void sWrite(const UTF16* iString, const ChanW_UTF& iChanW)
	{
	if (size_t countCU = ZUnicode::sCountCU(iString))
		spWriteMust(iString, countCU, iChanW);
	}

void sWrite(const UTF8* iString, const ChanW_UTF& iChanW)
	{
	if (size_t countCU = ZUnicode::sCountCU(iString))
		spWriteMust(iString, countCU, iChanW);
	}

// ---

void sWrite(const string32& iString, const ChanW_UTF& iChanW)
	{
	// Some non-conformant string implementations (MS) hate it if you try to access the buffer
	// underlying an empty string. They don't event return zero, they crash or throw an exception.
	if (const size_t countCU = iString.size())
		spWriteMust(iString.data(), countCU, iChanW);
	}

void sWrite(const string16& iString, const ChanW_UTF& iChanW)
	{
	if (const size_t countCU = iString.size())
		spWriteMust(iString.data(), countCU, iChanW);
	}

void sWrite(const string8& iString, const ChanW_UTF& iChanW)
	{
	if (const size_t countCU = iString.size())
		spWriteMust(iString.data(), countCU, iChanW);
	}


// -----

void sWritef(const ChanW_UTF& iChanW, const UTF8* iString, ...);

void sWritef(const ChanW_UTF& iChanW, size_t* oCountCU, size_t* oWritten, const UTF8* iString, ...);

void sWritev(const ChanW_UTF& iChanW, const UTF8* iString, va_list iArgs);

void sWritev(const ChanW_UTF& iChanW, size_t* oCountCU, size_t* oWritten,
	const UTF8* iString, va_list iArgs);


} // namespace ZooLib
