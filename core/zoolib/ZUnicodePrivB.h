/* -------------------------------------------------------------------------------------------------
Copyright (c) 2004 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZUnicodePrivB_h__
#define __ZUnicodePrivB_h__ 1
#include "zconfig.h"

#include "zoolib/ZUnicode.h"

namespace ZooLib {
namespace ZUnicode {

// =================================================================================================
// MARK: - ZUnicode::Functions_CountCU

template <class I>
size_t Functions_CountCU_T<I>::sCountCU(I iSource)
	{
	I localSource = iSource;
	while (0 != *localSource)
		++localSource;
	return localSource - iSource;
	}

// =================================================================================================
// MARK: - ZUnicode::Functions_Count

template <class I>
size_t Functions_Count_T<I>::sCountCP(I iSource)
	{
	size_t count = 0;
	while (Functions_Read_T<I>::sReadInc(iSource))
		++count;
	return count;
	}

template <class I>
void Functions_Count_T<I>::sCount(I iSource, size_t* oCountCU, size_t* oCountCP)
	{
	if (oCountCU)
		{
		if (oCountCP)
			{
			size_t count = 0;
			I localSource = iSource;
			while (Functions_Read_T<I>::sReadInc(localSource))
				++count;
			*oCountCU = localSource - iSource;
			*oCountCP = count;
			}
		else
			{
			*oCountCU = Functions_CountCU_T<I>::sCountCU(iSource);
			}
		}
	else
		{
		if (oCountCP)
			*oCountCP = sCountCP(iSource);
		}
	}

template <class I>
size_t Functions_Count_T<I>::sCPToCU(I iSource, size_t iCountCP)
	{
	I localSource = iSource;
	while (iCountCP--)
		Functions_Read_T<I>::sInc(localSource);
	return localSource - iSource;
	}

template <class I>
size_t Functions_Count_T<I>::sCPToCU(I iSource, size_t iCountCU, size_t iCountCP, size_t* oCountCP)
	{
	I localSource = iSource;
	I localEnd = iSource + iCountCU;
	size_t countRemaining = iCountCP;
	if (countRemaining)
		{
		++countRemaining;
		while (--countRemaining)
			{
			if (not Functions_Read_T<I>::sInc(localSource, localEnd))
				break;
			}
		}
	if (oCountCP)
		*oCountCP = iCountCP - countRemaining;
	return localSource - iSource;
	}

template <class I>
size_t Functions_Count_T<I>::sCPToCU(I iSource, I iEnd, size_t iCountCP, size_t* oCountCP)
	{
	I localSource = iSource;
	size_t countRemaining = iCountCP;
	if (countRemaining)
		{
		++countRemaining;
		while (--countRemaining)
			{
			if (not Functions_Read_T<I>::sInc(localSource, iEnd))
				break;
			}
		}
	if (oCountCP)
		*oCountCP = iCountCP - countRemaining;
	return localSource - iSource;
	}

template <class I>
size_t Functions_Count_T<I>::sCUToCP(I iSource, size_t iCountCU)
	{
	size_t countCP = 0;
	I localEnd = iSource + iCountCU;
	for (;;)
		{
		if (not Functions_Read_T<I>::sInc(iSource, localEnd))
			break;
		++countCP;
		}
	return countCP;
	}

template <class I>
size_t Functions_Count_T<I>::sCUToCP(I iSource, I iEnd)
	{
	size_t countCP = 0;
	for (;;)
		{
		if (not Functions_Read_T<I>::sInc(iSource, iEnd))
			break;
		++countCP;
		}
	return countCP;
	}

// =================================================================================================
// MARK: - ZUnicode::Functions_Read_T<I, UTF32>

template <class I>
struct Functions_Read_T<I, UTF32>
	{
	static void sAlign(I& ioCurrent);
	static void sAlign(I& ioCurrent, I iEnd);

	static void sInc(I& ioCurrent);
	static bool sInc(I& ioCurrent, I iEnd);

	static void sDec(I& ioCurrent);
	static bool sDec(I iStart, I& ioCurrent, I iEnd);

	static UTF32 sRead(I iCurrent);
	static bool sRead(I iCurrent, I iEnd, UTF32& oCP);

	static UTF32 sReadInc(I& ioCurrent);
	static bool sReadInc(I& ioCurrent, I iEnd, UTF32& oCP);
	static bool sReadInc(I& ioCurrent, I iEnd, UTF32& oCP, size_t& ioCountSkipped);

	static UTF32 sDecRead(I& ioCurrent);
	static bool sDecRead(I iStart, I& ioCurrent, I iEnd, UTF32& oCP);
	};

template <class I>
void Functions_Read_T<I, UTF32>::sAlign(I& ioCurrent)
	{
	for (;;)
		{
		if (sIsValidCP(*ioCurrent))
			break;
		++ioCurrent;
		}
	}

template <class I>
void Functions_Read_T<I, UTF32>::sAlign(I& ioCurrent, I iEnd)
	{
	while (ioCurrent < iEnd)
		{
		if (sIsValidCP(*ioCurrent))
			break;
		++ioCurrent;
		}
	}

template <class I>
void Functions_Read_T<I, UTF32>::sInc(I& ioCurrent)
	{
	for (;;)
		{
		if (sIsValidCP(*ioCurrent++))
			return;
		}
	}

template <class I>
bool Functions_Read_T<I, UTF32>::sInc(I& ioCurrent, I iEnd)
	{
	for (;;)
		{
		if (ioCurrent >= iEnd)
			{
			// We've run off the end.
			return false;
			}

		if (sIsValidCP(*ioCurrent++))
			return true;
		}
	}

template <class I>
void Functions_Read_T<I, UTF32>::sDec(I& ioCurrent)
	{
	for (;;)
		{
		if (sIsValidCP(*--ioCurrent))
			return;
		}
	}

template <class I>
bool Functions_Read_T<I, UTF32>::sDec(I iStart, I& ioCurrent, I iEnd)
	{
	for (;;)
		{
		if (iStart >= ioCurrent)
			{
			// We'd run off the start.
			return false;
			}
		if (sIsValidCP(*--ioCurrent))
			return true;
		}
	}

template <class I>
UTF32 Functions_Read_T<I, UTF32>::sRead(I iCurrent)
	{
	return sReadInc(iCurrent);
	}

template <class I>
bool Functions_Read_T<I, UTF32>::sRead(I iCurrent, I iEnd, UTF32& oCP)
	{
	return sReadInc(iCurrent, iEnd, oCP);
	}

template <class I>
UTF32 Functions_Read_T<I, UTF32>::sReadInc(I& ioCurrent)
	{
	for (;;)
		{
		uint32 theCU = *ioCurrent++;
		if (sIsValidCP(theCU))
			return theCU;
		}
	}

template <class I>
bool Functions_Read_T<I, UTF32>::sReadInc(I& ioCurrent, I iEnd, UTF32& oCP)
	{
	for (;;)
		{
		if (ioCurrent >= iEnd)
			{
			// We've run off the end.
			return false;
			}

		uint32 theCU = *ioCurrent++;
		if (sIsValidCP(theCU))
			{
			oCP = theCU;
			return true;
			}
		}
	}

template <class I>
bool Functions_Read_T<I, UTF32>::sReadInc(I& ioCurrent, I iEnd, UTF32& oCP, size_t& ioCountSkipped)
	{
	for (;;)
		{
		if (ioCurrent >= iEnd)
			{
			// We've run off the end.
			return false;
			}

		uint32 theCU = *ioCurrent++;
		if (sIsValidCP(theCU))
			{
			oCP = theCU;
			return true;
			}
		++ioCountSkipped;
		}
	}

template <class I>
UTF32 Functions_Read_T<I, UTF32>::sDecRead(I& ioCurrent)
	{
	for (;;)
		{
		uint32 theCP = *--ioCurrent;
		if (sIsValidCP(theCP))
			return theCP;
		}
	}

template <class I>
bool Functions_Read_T<I, UTF32>::sDecRead(I iStart, I& ioCurrent, I iEnd, UTF32& oCP)
	{
	for (;;)
		{
		if (iStart >= ioCurrent)
			{
			// We'd run off the start.
			return false;
			}
		uint32 theCP = *--ioCurrent;
		if (sIsValidCP(theCP))
			{
			oCP = theCP;
			return true;
			}
		}
	}

// =================================================================================================
// MARK: - ZUnicode::Functions_Write_T<I, UTF32>

template <class I>
struct Functions_Write_T<I, UTF32>
	{
	static bool sWrite(I oDest, I iEnd, UTF32 iCP);
	static bool sWriteInc(I& ioDest, I iEnd, UTF32 iCP);
	};

template <class I>
bool Functions_Write_T<I, UTF32>::sWrite(I oDest, I iEnd, UTF32 iCP)
	{
	return sWriteInc(oDest, iEnd, iCP);
	}

template <class I>
bool Functions_Write_T<I, UTF32>::sWriteInc(I& ioDest, I iEnd, UTF32 iCP)
	{
	if (sIsValidCP(iCP))
		{
		if (ioDest >= iEnd)
			return false;
		*ioDest++ = iCP;
		}
	return true;
	}

// =================================================================================================
// MARK: - ZUnicode::Functions_Read_T<I, UTF16>

template <class I>
struct Functions_Read_T<I, UTF16>
	{
	static void sAlign(I& ioCurrent);
	static void sAlign(I& ioCurrent, I iEnd);

	static void sInc(I& ioCurrent);
	static bool sInc(I& ioCurrent, I iEnd);

	static void sDec(I& ioCurrent);
	static bool sDec(I iStart, I& ioCurrent, I iEnd);

	static UTF32 sRead(I iCurrent);
	static bool sRead(I iCurrent, I iEnd, UTF32& oCP);

	static UTF32 sReadInc(I& ioCurrent);
	static bool sReadInc(I& ioCurrent, I iEnd, UTF32& oCP);
	static bool sReadInc(I& ioCurrent, I iEnd, UTF32& oCP, size_t& ioCountSkipped);

	static UTF32 sDecRead(I& ioCurrent);
	static bool sDecRead(I iStart, I& ioCurrent, I iEnd, UTF32& oCP);
	};

template <class I>
void Functions_Read_T<I, UTF16>::sAlign(I& ioCurrent)
	{
	for (;;)
		{
		uint16 theCU = *ioCurrent;
		if (sIsSmallNormal(theCU))
			{
			break;
			}
		else if (sIsSmallNormalOrHighSurrogate(theCU))
			{
			// Must be a high surrogate as it's not a small normal.
			if (sIsLowSurrogate(uint16(ioCurrent[1])))
				break;
			// It's not followed by a low surrogate, so move on.
			++ioCurrent;
			}
		else if (sIsBigNormalOrBeyond(theCU))
			{
			break;
			}
		else
			{
			// Must be an out of order low surrogate.
			++ioCurrent;
			}
		}
	}

template <class I>
void Functions_Read_T<I, UTF16>::sAlign(I& ioCurrent, I iEnd)
	{
	while (ioCurrent < iEnd)
		{
		uint16 theCU = *ioCurrent;
		if (sIsSmallNormal(theCU))
			{
			break;
			}
		else if (sIsSmallNormalOrHighSurrogate(theCU))
			{
			// Must be a high surrogate as it's not a small normal.
			if (ioCurrent + 1 >= iEnd)
				{
				// But we'd run off the end.
				break;
				}

			if (sIsLowSurrogate(uint16(ioCurrent[1])))
				break;

			// It's not followed by a low surrogate, so move on.
			++ioCurrent;
			}
		else if (sIsBigNormalOrBeyond(theCU))
			{
			break;
			}
		else
			{
			// Must be an out of order low surrogate.
			++ioCurrent;
			}
		}
	}

template <class I>
void Functions_Read_T<I, UTF16>::sInc(I& ioCurrent)
	{
	for (;;)
		{
		uint16 theCU = uint16(*ioCurrent++);
		if (sIsSmallNormal(theCU))
			{
			return;
			}
		else if (sIsSmallNormalOrHighSurrogate(theCU))
			{
			// Must be a high surrogate as it's not a small normal.
			if (sIsLowSurrogate(uint16(*ioCurrent++)))
				return;
			--ioCurrent;
			}
		else if (sIsBigNormalOrBeyond(theCU))
			{
			return;
			}
		else
			{
			// Must be an out of order low surrogate.
			}
		}
	}

template <class I>
bool Functions_Read_T<I, UTF16>::sInc(I& ioCurrent, I iEnd)
	{
	for (;;)
		{
		if (ioCurrent >= iEnd)
			{
			// We've run off the end.
			return false;
			}

		uint16 theCU = uint16(*ioCurrent++);
		if (sIsSmallNormal(theCU))
			{
			return true;
			}
		else if (sIsSmallNormalOrHighSurrogate(theCU))
			{
			// Must be a high surrogate as it's not a small normal.
			if (ioCurrent >= iEnd)
				{
				// But we'd run off the end. Restore the value of ioCurrent to indicate this.
				--ioCurrent;
				return false;
				}

			if (sIsLowSurrogate(uint16(*ioCurrent++)))
				return true;
			--ioCurrent;
			}
		else if (sIsBigNormalOrBeyond(theCU))
			{
			return true;
			}
		else
			{
			// Must be an out of order low surrogate.
			}
		}
	}

template <class I>
void Functions_Read_T<I, UTF16>::sDec(I& ioCurrent)
	{
	bool lastWasLowSurrogate = false;
	for (;;)
		{
		uint16 theCU = uint16(*--ioCurrent);
		if (sIsSmallNormal(theCU))
			{
			// We've moved on to a small normal.
			break;
			}
		else if (sIsSmallNormalOrHighSurrogate(theCU))
			{
			// It's not a small normal, so must be a high surrogate.
			if (lastWasLowSurrogate)
				break;
			lastWasLowSurrogate = false;
			}
		else if (sIsBigNormalOrBeyond(theCU))
			{
			// We've moved on to a big normal.
			break;
			}
		else
			{
			// We must be on a low surrogate. Remember the fact.
			lastWasLowSurrogate = true;
			}
		}
	}

template <class I>
bool Functions_Read_T<I, UTF16>::sDec(I iStart, I& ioCurrent, I iEnd)
	{
	bool lastWasLowSurrogate = false;
	for (;;)
		{
		if (iStart >= ioCurrent)
			{
			// We'd run off the start.
			return false;
			}

		uint16 theCU = uint16(*--ioCurrent);
		if (sIsSmallNormal(theCU))
			{
			// We've moved on to a small normal.
			return true;
			}
		else if (sIsSmallNormalOrHighSurrogate(theCU))
			{
			// It's not a small normal, so must be a high surrogate.
			if (lastWasLowSurrogate)
				return true;
			lastWasLowSurrogate = false;
			}
		else if (sIsBigNormalOrBeyond(theCU))
			{
			// We've moved on to a big normal.
			return true;
			}
		else
			{
			// We must be on a low surrogate. Remember the fact.
			lastWasLowSurrogate = true;
			}
		}
	}

template <class I>
UTF32 Functions_Read_T<I, UTF16>::sRead(I iCurrent)
	{
	return sReadInc(iCurrent);
	}

template <class I>
bool Functions_Read_T<I, UTF16>::sRead(I iCurrent, I iEnd, UTF32& oCP)
	{
	return sReadInc(iCurrent, iEnd, oCP);
	}

template <class I>
UTF32 Functions_Read_T<I, UTF16>::sReadInc(I& ioCurrent)
	{
	for (;;)
		{
		uint16 theCU = *ioCurrent++;
		if (sIsSmallNormal(theCU))
			{
			return theCU;
			}
		else if (sIsSmallNormalOrHighSurrogate(theCU))
			{
			// Must be a high surrogate as it's not a small normal.
			uint16 theCU2 = *ioCurrent++;
			if (sIsLowSurrogate(theCU2))
				return sUTF32FromSurrogates(theCU, theCU2);
			--ioCurrent;
			}
		else if (sIsBigNormalOrBeyond(theCU))
			{
			return theCU;
			}
		else
			{
			// Must be an out of order low surrogate.
			}
		}
	}

template <class I>
bool Functions_Read_T<I, UTF16>::sReadInc(I& ioCurrent, I iEnd, UTF32& oCP)
	{
	for (;;)
		{
		if (ioCurrent >= iEnd)
			{
			// We've run off the end.
			return false;
			}

		uint16 theCU = *ioCurrent++;
		if (sIsSmallNormal(theCU))
			{
			oCP = theCU;
			return true;
			}
		else if (sIsSmallNormalOrHighSurrogate(theCU))
			{
			// Must be a high surrogate as it's not a small normal.
			if (ioCurrent >= iEnd)
				{
				// But we'd run off the end. Restore the value of ioCurrent to indicate this.
				--ioCurrent;
				return false;
				}

			uint16 theCU2 = *ioCurrent++;
			if (sIsLowSurrogate(theCU2))
				{
				oCP = sUTF32FromSurrogates(theCU, theCU2);
				return true;
				}
			--ioCurrent;
			}
		else if (sIsBigNormalOrBeyond(theCU))
			{
			oCP = theCU;
			return true;
			}
		else
			{
			// Must be an out of order low surrogate.
			}
		}
	}

template <class I>
bool Functions_Read_T<I, UTF16>::sReadInc(I& ioCurrent, I iEnd, UTF32& oCP, size_t& ioCountSkipped)
	{
	for (;;)
		{
		if (ioCurrent >= iEnd)
			{
			// We've run off the end.
			return false;
			}

		uint16 theCU = *ioCurrent++;
		if (sIsSmallNormal(theCU))
			{
			oCP = theCU;
			return true;
			}
		else if (sIsSmallNormalOrHighSurrogate(theCU))
			{
			// Must be a high surrogate as it's not a small normal.
			if (ioCurrent >= iEnd)
				{
				// But we'd run off the end. Restore the value of ioCurrent to indicate this.
				--ioCurrent;
				return false;
				}

			uint16 theCU2 = *ioCurrent++;
			if (sIsLowSurrogate(theCU2))
				{
				oCP = sUTF32FromSurrogates(theCU, theCU2);
				return true;
				}
			// It's a high surrogate with no following low surrogate.
			++ioCountSkipped;
			--ioCurrent;
			}
		else if (sIsBigNormalOrBeyond(theCU))
			{
			oCP = theCU;
			return true;
			}
		else
			{
			// Must be an out of order low surrogate.
			++ioCountSkipped;
			}
		}
	}

template <class I>
UTF32 Functions_Read_T<I, UTF16>::sDecRead(I& ioCurrent)
	{
	uint16 priorLowSurrogate = 0;
	for (;;)
		{
		uint16 theCU = *--ioCurrent;
		if (sIsSmallNormal(theCU))
			{
			// We've moved on to a small normal.
			return theCU;
			}
		else if (sIsSmallNormalOrHighSurrogate(theCU))
			{
			// It's not a small normal, so must be a high surrogate.
			if (priorLowSurrogate)
				return sUTF32FromSurrogates(theCU, priorLowSurrogate);
			priorLowSurrogate = 0;
			}
		else if (sIsBigNormalOrBeyond(theCU))
			{
			return theCU;
			}
		else
			{
			// We must be on a low surrogate. Remember the fact.
			priorLowSurrogate = theCU;
			}
		}
	}

template <class I>
bool Functions_Read_T<I, UTF16>::sDecRead(I iStart, I& ioCurrent, I iEnd, UTF32& oCP)
	{
	uint16 priorLowSurrogate = 0;
	for (;;)
		{
		if (iStart >= ioCurrent)
			{
			// We'd run off the start.
			return false;
			}

		uint16 theCU = *--ioCurrent;
		if (sIsSmallNormal(theCU))
			{
			// We've moved on to a small normal.
			oCP = theCU;
			return true;
			}
		else if (sIsSmallNormalOrHighSurrogate(theCU))
			{
			// It's not a small normal, so must be a high surrogate.
			if (priorLowSurrogate)
				{
				oCP = sUTF32FromSurrogates(theCU, priorLowSurrogate);
				return true;
				}
			priorLowSurrogate = 0;
			}
		else if (sIsBigNormalOrBeyond(theCU))
			{
			oCP = theCU;
			return true;
			}
		else
			{
			// We must be on a low surrogate. Remember the fact.
			priorLowSurrogate = theCU;
			}
		}
	}

// =================================================================================================
// MARK: - ZUnicode::Functions_Write_T<I, UTF16>

template <class I>
struct Functions_Write_T<I, UTF16>
	{
	static bool sWrite(I oDest, I iEnd, UTF32 iCP);
	static bool sWriteInc(I& ioDest, I iEnd, UTF32 iCP);
	};

template <class I>
bool Functions_Write_T<I, UTF16>::sWrite(I oDest, I iEnd, UTF32 iCP)
	{
	return sWriteInc(oDest, iEnd, iCP);
	}

template <class I>
bool Functions_Write_T<I, UTF16>::sWriteInc(I& ioDest, I iEnd, UTF32 iCP)
	{
	if (sIsValidCP(iCP))
		{
		if (iCP > kCPMaxUCS2)
			{
			// It's beyond the BMP, so we need to write surrogates.
			if (ioDest + 1 >= iEnd)
				{
				// There's no room.
				return false;
				}
			iCP -= 0x10000;
			*ioDest++ = (iCP / 0x400) + kCPSurrogateHighBegin;
			*ioDest++ = (iCP & 0x3FF) + kCPSurrogateLowBegin;
			}
		else
			{
			if (ioDest >= iEnd)
				return false;
			*ioDest++ = iCP;
			}
		}
	return true;
	}

// =================================================================================================
// MARK: - ZUnicode::Functions_Read_T<I, UTF8>

template <class I>
struct Functions_Read_T<I, UTF8>
	{
	static void sAlign(I& ioCurrent);
	static void sAlign(I& ioCurrent, I iEnd);

	static void sInc(I& ioCurrent);
	static bool sInc(I& ioCurrent, I iEnd);

	static void sDec(I& ioCurrent);
	static bool sDec(I iStart, I& ioCurrent, I iEnd);

	static UTF32 sRead(I iCurrent);
	static bool sRead(I iCurrent, I iEnd, UTF32& oCP);

	static UTF32 sReadInc(I& ioCurrent);
	static bool sReadInc(I& ioCurrent, I iEnd, UTF32& oCP);
	static bool sReadInc(I& ioCurrent, I iEnd, UTF32& oCP, size_t& ioCountSkipped);

	static UTF32 sDecRead(I& ioCurrent);
	static bool sDecRead(I iStart, I& ioCurrent, I iEnd, UTF32& oCP);
	};

template <class I>
void Functions_Read_T<I, UTF8>::sAlign(I& ioCurrent)
	{
	for (;;)
		{
		I localSource = ioCurrent;
		const uint8 firstByte = *localSource++;
		size_t sequenceLength = sUTF8SequenceLength[firstByte];
		if (sequenceLength == 1)
			{
			// It's a standalone character, we can return.
			break;
			}
		else if (sequenceLength == 0)
			{
			// It's a continuation or illegal, ignore it.
			}
		else
			{
			bool okay = true;
			while (--sequenceLength)
				{
				if (not sIsContinuation(*localSource++))
					{
					--localSource;
					okay = false;
					break;
					}
				}
			if (okay)
				return;
			}
		ioCurrent = localSource;
		}
	}

template <class I>
void Functions_Read_T<I, UTF8>::sAlign(I& ioCurrent, I iEnd)
	{
	while (ioCurrent < iEnd)
		{
		I localSource = ioCurrent;
		const uint8 firstByte = *localSource++;
		size_t sequenceLength = sUTF8SequenceLength[firstByte];
		if (sequenceLength == 1)
			{
			// It's a standalone character, we can return.
			break;
			}
		else if (sequenceLength == 0)
			{
			// It's a continuation or illegal, ignore it.
			}
		else
			{
			bool okay = true;
			while (--sequenceLength && localSource < iEnd)
				{
				if (not sIsContinuation(*ioCurrent++))
					{
					--ioCurrent;
					okay = false;
					break;
					}
				}
			if (okay)
				return;
			}
		ioCurrent = localSource;
		}
	}

template <class I>
void Functions_Read_T<I, UTF8>::sInc(I& ioCurrent)
	{
	for (;;)
		{
		const uint8 firstByte = *ioCurrent++;
		size_t sequenceLength = sUTF8SequenceLength[firstByte];
		if (sequenceLength == 1)
			{
			return;
			}
		else if (sequenceLength == 0)
			{
			// It's a continuation or illegal, ignore it.
			}
		else
			{
			bool okay = true;
			while (--sequenceLength)
				{
				if (not sIsContinuation(*ioCurrent++))
					{
					// It's not a legal continuation byte.
					--ioCurrent;
					okay = false;
					break;
					}
				}
			if (okay)
				return;
			}
		}
	}

template <class I>
bool Functions_Read_T<I, UTF8>::sInc(I& ioCurrent, I iEnd)
	{
	for (;;)
		{
		if (ioCurrent >= iEnd)
			{
			// We've run off the end.
			return false;
			}

		const uint8 firstByte = *ioCurrent++;
		size_t sequenceLength = sUTF8SequenceLength[firstByte];
		if (sequenceLength == 1)
			{
			return true;
			}
		else if (sequenceLength == 0)
			{
			// It's a continuation or illegal, ignore it.
			}
		else
			{
			if (ioCurrent + sequenceLength - 1 > iEnd)
				{
				--ioCurrent;
				return false;
				}

			bool okay = true;
			while (--sequenceLength)
				{
				if (not sIsContinuation(*ioCurrent++))
					{
					// It's not a legal continuation byte.
					--ioCurrent;
					okay = false;
					break;
					}
				}
			if (okay)
				return true;
			}
		}
	}

template <class I>
void Functions_Read_T<I, UTF8>::sDec(I& ioCurrent)
	{
	size_t continuationCount = 0;
	bool seenNonContinuation = false;
	for (;;)
		{
		const uint8 firstByte = *--ioCurrent;
		size_t sequenceLength = sUTF8SequenceLength[firstByte];
		if (sequenceLength == 0)
			{
			if (sIsContinuation(firstByte))
				{
				// It's a continuation.
				++continuationCount;
				}
			else
				{
				// It's illegal.
				continuationCount = 0;
				seenNonContinuation = true;
				}
			}
		else
			{
			// It's a start byte or is normal.
			if (continuationCount + 1 >= sequenceLength)
				{
				// We've seen enough continuation bytes.
				return;
				}
			else
				{
				// We haven't seen enough continuation bytes.
				if (not seenNonContinuation)
					{
					// We've only seen continuation bytes.
					I current = ioCurrent + 1;
					while (--sequenceLength)
						{
						if (not sIsContinuation(*current++))
							break;
						}
					if (sequenceLength == 0)
						{
						// We found sequenceLength continuation bytes subsequent to the
						// start byte at ioCurrent.
						return;
						}
					}
				continuationCount = 0;
				seenNonContinuation = true;
				}
			}
		}
	}

template <class I>
bool Functions_Read_T<I, UTF8>::sDec(I iStart, I& ioCurrent, I iEnd)
	{
	size_t continuationCount = 0;
	bool seenNonContinuation = false;
	for (;;)
		{
		if (iStart >= ioCurrent)
			{
			// We'd run off the start.
			return false;
			}

		const uint8 firstByte = *--ioCurrent;
		size_t sequenceLength = sUTF8SequenceLength[firstByte];
		if (sequenceLength == 0)
			{
			if (sIsContinuation(firstByte))
				{
				// It's a continuation.
				++continuationCount;
				}
			else
				{
				// It's illegal.
				continuationCount = 0;
				seenNonContinuation = true;
				}
			}
		else
			{
			// It's a start byte or is normal.
			if (continuationCount + 1 >= sequenceLength)
				{
				// We've seen enough continuation bytes.
				return true;
				}
			else
				{
				// We haven't seen enough continuation bytes.
				if (not seenNonContinuation)
					{
					// We've only seen continuation bytes.
					if (ioCurrent + sequenceLength <= iEnd)
						{
						// If there were enough continuation bytes, they would not extend
						// beyond the end of the buffer.
						I current = ioCurrent + 1;
						while (--sequenceLength)
							{
							if (not sIsContinuation(*current++))
								break;
							}
						if (sequenceLength == 0)
							{
							// We found sequenceLength continuation bytes subsequent to the
							// start byte at ioCurrent.
							return true;
							}
						}
					}
				continuationCount = 0;
				seenNonContinuation = true;
				}
			}
		}
	}

template <class I>
UTF32 Functions_Read_T<I, UTF8>::sRead(I iCurrent)
	{
	return sReadInc(iCurrent);
	}

template <class I>
bool Functions_Read_T<I, UTF8>::sRead(I iCurrent, I iEnd, UTF32& oCP)
	{
	return sReadInc(iCurrent, iEnd, oCP);
	}

template <class I>
UTF32 Functions_Read_T<I, UTF8>::sReadInc(I& ioCurrent)
	{
	for (;;)
		{
		const uint8 firstByte = *ioCurrent++;
		size_t sequenceLength = sUTF8SequenceLength[firstByte];
		if (sequenceLength == 1)
			{
			return firstByte;
			}
		else if (sequenceLength == 0)
			{
			// It's a continuation or illegal, ignore it.
			}
		else
			{
			uint32 result = firstByte & sUTF8StartByteMask[sequenceLength];
			bool okay = true;
			while (--sequenceLength)
				{
				const uint8 curByte = *ioCurrent++;
				if (not sIsContinuation(curByte))
					{
					// It's not a legal continuation byte.
					--ioCurrent;
					okay = false;
					break;
					}
				sAppendContinuation(result, curByte);
				}
			if (okay)
				return result;
			}
		}
	}

template <class I>
bool Functions_Read_T<I, UTF8>::sReadInc(I& ioCurrent, I iEnd, UTF32& oCP)
	{
	for (;;)
		{
		if (ioCurrent >= iEnd)
			{
			// We've run off the end.
			return false;
			}

		const uint8 firstByte = *ioCurrent++;
		size_t sequenceLength = sUTF8SequenceLength[firstByte];
		if (sequenceLength == 1)
			{
			oCP = firstByte;
			return true;
			}
		else if (sequenceLength == 0)
			{
			// It's a continuation or illegal, ignore it.
			}
		else
			{
			if (ioCurrent + sequenceLength - 1 > iEnd)
				{
				--ioCurrent;
				return false;
				}

			uint32 result = firstByte & sUTF8StartByteMask[sequenceLength];
			bool okay = true;
			while (--sequenceLength)
				{
				const uint8 curByte = *ioCurrent++;
				if (not sIsContinuation(curByte))
					{
					// It's not a legal continuation byte.
					--ioCurrent;
					okay = false;
					break;
					}
				sAppendContinuation(result, curByte);
				}
			if (okay)
				{
				oCP = result;
				return true;
				}
			}
		}
	}

template <class I>
bool Functions_Read_T<I, UTF8>::sReadInc(I& ioCurrent, I iEnd, UTF32& oCP, size_t& ioCountSkipped)
	{
	for (;;)
		{
		if (ioCurrent >= iEnd)
			{
			// We've run off the end.
			return false;
			}

		const uint8 firstByte = *ioCurrent++;
		size_t sequenceLength = sUTF8SequenceLength[firstByte];
		if (sequenceLength == 1)
			{
			oCP = firstByte;
			return true;
			}
		else if (sequenceLength == 0)
			{
			// It's a continuation or illegal, ignore it.
			++ioCountSkipped;
			}
		else
			{
			if (ioCurrent + sequenceLength - 1 > iEnd)
				{
				--ioCurrent;
				return false;
				}

			uint32 result = firstByte & sUTF8StartByteMask[sequenceLength];
			bool okay = true;
			for (size_t countConsumed = 1; countConsumed < sequenceLength; ++countConsumed)
				{
				const uint8 curByte = *ioCurrent++;
				if (not sIsContinuation(curByte))
					{
					// It's not a legal continuation byte.
					--ioCurrent;
					okay = false;
					ioCountSkipped += countConsumed + 1;
					break;
					}
				sAppendContinuation(result, curByte);
				}
			if (okay)
				{
				oCP = result;
				return true;
				}
			}
		}
	}

template <class I>
UTF32 Functions_Read_T<I, UTF8>::sDecRead(I& ioCurrent)
	{
	for (;;)
		{
		const uint8 firstByte = *--ioCurrent;
		if (size_t sequenceLength = sUTF8SequenceLength[firstByte])
			{
			// It's a start byte or is normal.
			if (sequenceLength == 1)
				return firstByte;
			uint32 result = firstByte & sUTF8StartByteMask[sequenceLength];
			I localCurrent = ioCurrent;
			while (--sequenceLength)
				{
				const uint8 curByte = *++localCurrent;
				if (sIsContinuation(curByte))
					sAppendContinuation(result, curByte);
				else
					break;
				}
			if (sequenceLength == 0)
				return result;
			}
		}
	}

template <class I>
bool Functions_Read_T<I, UTF8>::sDecRead(I iStart, I& ioCurrent, I iEnd, UTF32& oCP)
	{
	for (;;)
		{
		if (iStart >= ioCurrent)
			{
			// We'd run off the start.
			return false;
			}

		const uint8 firstByte = *--ioCurrent;
		if (size_t sequenceLength = sUTF8SequenceLength[firstByte])
			{
			// It's a start byte or is normal.
			if (ioCurrent + sequenceLength <= iEnd)
				{
				// And the sequence fits within the available space.
				if (sequenceLength == 1)
					{
					// Special case single bytes.
					oCP = firstByte;
					return true;
					}

				uint32 result = firstByte & sUTF8StartByteMask[sequenceLength];
				I localCurrent = ioCurrent;
				while (--sequenceLength)
					{
					const uint8 curByte = *++localCurrent;
					if (sIsContinuation(curByte))
						sAppendContinuation(result, curByte);
					else
						break;
					}
				if (sequenceLength == 0)
					{
					// We found enough continuations.
					oCP = result;
					return true;
					}
				}
			}
		}
	}

// =================================================================================================
// MARK: - ZUnicode::Functions_Write_T<I, UTF8>

template <class I>
struct Functions_Write_T<I, UTF8>
	{
	static bool sWrite(I oDest, I iEnd, UTF32 iCP);
	static bool sWriteInc(I& ioDest, I iEnd, UTF32 iCP);
	};

template <class I>
bool Functions_Write_T<I, UTF8>::sWrite(I oDest, I iEnd, UTF32 iCP)
	{
	if (sIsValidCP(iCP))
		{
		size_t bytesToWrite;
		if (iCP < 0x80)
			{
			if (oDest <= iEnd)
				{
				*oDest = iCP;
				return true;
				}
			return false;
			}
		else if (iCP < 0x800) bytesToWrite = 2;
		else if (iCP < 0x10000) bytesToWrite = 3;
		else if (iCP < 0x200000) bytesToWrite = 4;
		else if (iCP < 0x4000000) bytesToWrite = 5;
		else bytesToWrite = 6;

		oDest += bytesToWrite;
		if (oDest >= iEnd)
			return false;

		const UTF32 byteMask = 0xBF;
		const UTF32 byteMark = 0x80;
		switch (bytesToWrite)
			{
			// note: code falls through cases
			case 6:	*--oDest = (iCP | byteMark) & byteMask; iCP >>= 6;
			case 5:	*--oDest = (iCP | byteMark) & byteMask; iCP >>= 6;
			case 4:	*--oDest = (iCP | byteMark) & byteMask; iCP >>= 6;
			case 3:	*--oDest = (iCP | byteMark) & byteMask; iCP >>= 6;
			case 2:	*--oDest = (iCP | byteMark) & byteMask; iCP >>= 6;
			}
		*--oDest = iCP | sUTF8StartByteMark[bytesToWrite];
		}
	return true;
	}

template <class I>
bool Functions_Write_T<I, UTF8>::sWriteInc(I& ioDest, I iEnd, UTF32 iCP)
	{
	if (sIsValidCP(iCP))
		{
		size_t bytesToWrite;
		if (iCP < 0x80)
			{
			if (ioDest <= iEnd)
				{
				*ioDest++ = iCP;
				return true;
				}
			return false;
			}
		else if (iCP < 0x800) bytesToWrite = 2;
		else if (iCP < 0x10000) bytesToWrite = 3;
		else if (iCP < 0x200000) bytesToWrite = 4;
		else if (iCP < 0x4000000) bytesToWrite = 5;
		else bytesToWrite = 6;

		I localDest = ioDest + bytesToWrite;
		if (localDest >= iEnd)
			return false;
		ioDest = localDest;

		const UTF32 byteMask = 0xBF;
		const UTF32 byteMark = 0x80;
		switch (bytesToWrite)
			{
			// note: code falls through cases
			case 6:	*--localDest = (iCP | byteMark) & byteMask; iCP >>= 6;
			case 5:	*--localDest = (iCP | byteMark) & byteMask; iCP >>= 6;
			case 4:	*--localDest = (iCP | byteMark) & byteMask; iCP >>= 6;
			case 3:	*--localDest = (iCP | byteMark) & byteMask; iCP >>= 6;
			case 2:	*--localDest = (iCP | byteMark) & byteMask; iCP >>= 6;
			}
		*--localDest = iCP | sUTF8StartByteMark[bytesToWrite];
		}
	return true;
	}

// =================================================================================================
// MARK: - ZUnicode::Functions_Convert

template <class I>
string32 Functions_Convert_T<I>::sAsUTF32(I iSource)
	{
	string32 result;
	for (;;)
		{
		if (UTF32 theCP = sReadInc(iSource))
			result += theCP;
		else
			break;
		}
	return result;
	}

template <class I>
string32 Functions_Convert_T<I>::sAsUTF32(I iSource, size_t iCountCU)
	{
#if 1
	string32 result;
	result.resize(iCountCU);

	string32::iterator iter = result.begin();
	I theEnd = iSource + iCountCU;
	for (UTF32 theCP; sReadInc(iSource, theEnd, theCP); /*no inc*/)
		*iter++ = theCP;
	result.resize(iter - result.begin());
	return result;
#else
	string32 result;
	result.reserve(iCountCU);

	I theEnd = iSource + iCountCU;
	for (UTF32 theCP; sReadInc(iSource, theEnd, theCP); result += theCP)
		{}
	return result;
#endif
	}

template <class I>
string32 Functions_Convert_T<I>::sAsUTF32(I iSource, I iEnd)
	{
	string32 result;
	for (UTF32 theCP; sReadInc(iSource, iEnd, theCP); result += theCP)
		{}
	return result;
	}

template <class I>
string16 Functions_Convert_T<I>::sAsUTF16(I iSource)
	{
	string16 result;
	for (;;)
		{
		if (UTF32 theCP = sReadInc(iSource))
			result += theCP;
		else
			break;
		}
	return result;
	}

template <class I>
string16 Functions_Convert_T<I>::sAsUTF16(I iSource, size_t iCountCU)
	{
#if 1
	string16 result;
	// V. conservative, for UTF32->UTF16
	result.resize(iCountCU * 2);
	string16::iterator iter = result.begin();
	I theEnd = iSource + iCountCU;
	for (UTF32 theCP; sReadInc(iSource, theEnd, theCP); /*no inc*/)
		{
		uint32 realCP = theCP;
		if (realCP <= uint32(ZUnicode::kCPMaxUCS2))
			{
			*iter++ = UTF16(realCP);
			}
		else
			{
			realCP -= 0x10000;
			*iter++ = UTF16((realCP / 0x400) + ZUnicode::kCPSurrogateHighBegin);
			*iter++ += UTF16((realCP & 0x3FF) + ZUnicode::kCPSurrogateLowBegin);
			}
		}
	result.resize(iter - result.begin());
	return result;
#else
	string16 result;
	result.reserve(iCountCU);

	I theEnd = iSource + iCountCU;
	for (UTF32 theCP; sReadInc(iSource, theEnd, theCP); result += theCP)
		{}
	return result;
#endif
	}

template <class I>
string16 Functions_Convert_T<I>::sAsUTF16(I iSource, I iEnd)
	{
	string16 result;
	for (UTF32 theCP; sReadInc(iSource, iEnd, theCP); result += theCP)
		{}
	return result;
	}

template <class I>
string8 Functions_Convert_T<I>::sAsUTF8(I iSource)
	{
	string8 result;
	for (;;)
		{
		if (UTF32 theCP = sReadInc(iSource))
			result += theCP;
		else
			break;
		}
	return result;
	}

template <class I>
string8 Functions_Convert_T<I>::sAsUTF8(I iSource, size_t iCountCU)
	{
	string8 result;
	result.reserve(iCountCU);

	I theEnd = iSource + iCountCU;
	for (UTF32 theCP; sReadInc(iSource, theEnd, theCP); result += theCP)
		{}

	return result;
	}

template <class I>
string8 Functions_Convert_T<I>::sAsUTF8(I iSource, I iEnd)
	{
	string8 result;
	for (UTF32 theCP; sReadInc(iSource, iEnd, theCP); result += theCP)
		{}
	return result;
	}

} // namespace ZUnicode
} // namespace ZooLib

#endif // __ZUnicodePrivB_h__
