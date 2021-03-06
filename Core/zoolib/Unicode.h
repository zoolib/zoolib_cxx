// Copyright (c) 2001 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZooLib_Unicode_h__
#define __ZooLib_Unicode_h__ 1
#include "zconfig.h"

#include "zoolib/StdInt.h"
#include "zoolib/UnicodePriv.h"

#include "zoolib/ZTypes.h"

#include <string>

namespace ZooLib {
namespace Unicode {

// =================================================================================================
#pragma mark - ZooLib::Unicode, code unit stuff

/** \name Useful constants and lookup tables
*///@{
extern const uint8 sUTF8SequenceLength[256];
extern const uint8 sUTF8StartByteMark[7];
extern const uint8 sUTF8StartByteMask[7];

/// In-band indication of an end of stream. Not actually used by ZooLib::Unicode.
const UTF32 kCPEOF = UTF32(-1);

/// The maximum CP that's valid and representable in UCS-2.
const UTF32 kCPMaxUCS2 = 0xFFFFul;

/// The maximum CP that's valid and representable in UTF-32, UTF-16 and UTF-8.
const UTF32 kCPMaxUTF = 0x10FFFFul;

/// The maximum CP that's representable in UCS-4 and UTF-8.
const UTF32 kCPMaxUCS4 = 0x7FFFFFFFul;

/** The CP to be emitted by a decoder when Unicode cannot represent the source CP.
Not used/produced by ZooLib::Unicode */
const UTF32 kCPReplacement = 0xFFFDul;

/// UTF-16 surrogate pair ranges.
const UTF32 kCPSurrogateHighBegin = 0xD800u;
const UTF32 kCPSurrogateHighEnd = 0xDC00u;
const UTF32 kCPSurrogateLowBegin = 0xDC00u;
const UTF32 kCPSurrogateLowEnd = 0xE000u;
//@}

/*
We've got four ranges to deal with:
 0x0000 -   0xD7FF Small normal
 0xD800 -   0xDBFF High surrogate
 0xDC00 -   0xDFFF Low surrogate
 0xE000 -   0xFFFF Big normal
0x10000 - 0x10FFFF Big normal, but requiring more than 16 bits to represent.

We also have to beware that UTF16 and UTF32 may be signed types, so these methods take
uint32s and coerce the boundary constants to ensure they're also uint32. Callers
should work with uint32s, being careful to prevent sign-extension when they load
from smaller sources.
*/

inline bool sIsValidCP(uint32 iCP)
	{
	return iCP < uint32(kCPSurrogateHighBegin)
		|| (iCP >= uint32(kCPSurrogateLowEnd) && iCP <= uint32(kCPMaxUTF));
	}

inline UTF32 sUTF32FromSurrogates(uint32 hi, uint32 lo)
	{
	const int kSurrogateShift = 10;
	const uint32 kSurrogateBase = 0x10000ul;
	return kSurrogateBase
		+ ((hi - uint32(kCPSurrogateHighBegin)) << kSurrogateShift)
		+ (lo - uint32(kCPSurrogateLowBegin));
	}

inline bool sIsLowSurrogate(uint32 iCU)
	{ return iCU >= uint32(kCPSurrogateLowBegin) && iCU < uint32(kCPSurrogateLowEnd); }

inline bool sIsSmallNormal(uint32 iCU)
	{ return iCU < uint32(kCPSurrogateHighBegin); }

inline bool sIsSmallNormalOrHighSurrogate(uint32 iCU)
	{ return iCU < uint32(kCPSurrogateLowBegin); }

inline bool sIsBigNormalOrBeyond(uint32 iCU)
	{ return iCU >= uint32(kCPSurrogateLowEnd); }

inline bool sIsContinuation(uint8 iCU)
	{ return (iCU & 0xC0) == 0x80; }

inline void sAppendContinuation(uint32& ioCP, uint8 iContinuation)
	{ ioCP = (ioCP << 6) + (iContinuation & 0x3F); }

// =================================================================================================
#pragma mark - ZooLib::Unicode, counting and iterators

/* These are all template functions, they'll take anything that behaves like a string::iterator
or a pointer. They call through to static member functions of template structs declared in
UnicodePriv.h. The actual code for the three different code unit types is in UnicodePrivB.h,
and we do explicit template instantiations of the structs in Unicode.cpp. So if you have a new
kind of iterator you'll need to do something similar in your code. */

/** \name Counting code units or code points, looking for zero terminator.
*///@{
/** Return the number of code units between \a iSource
and the first ocurrence of a zero code unit. */
template <class I>
inline size_t sCountCU(I iSource)
	{ return Functions_CountCU_T<I>::sCountCU(iSource); }

/** Return the number of correctly encoded code points between
\a iSource and the first ocurrence of a zero code unit. */
template <class I>
inline size_t sCountCP(I iSource)
	{ return Functions_Count_T<I>::sCountCP(iSource); }

/** Return both the number of code units and the number of correctly encoded
code points between \a iSource and the first occurrence of a zero code unit. */
template <class I>
inline void sCount(I iSource, size_t* oCountCU, size_t* oCountCP)
	{ Functions_Count_T<I>::sCount(iSource, oCountCU, oCountCP); }
//@}


/** \name Mapping offsets between code points and code units.
*///@{
/** Return the number of code units that must be traversed to generate
\a iCountCP valid code points in the string buffer starting at \a iSource. */
template <class I>
inline size_t sCPToCU(I iSource, size_t iCountCP)
	{ return Functions_Count_T<I>::sCPToCU(iSource, iCountCP); }

/** Return the number of code units that must be traversed to generate \a iCountCP valid code
points in the string buffer starting at \a iSource and extending to \a iSource + \a iCountCU.
Return the number of code points actually traversed in \a oCountCP. */
template <class I>
inline size_t sCPToCU(I iSource, size_t iCountCU, size_t iCountCP, size_t* oCountCP)
	{ return Functions_Count_T<I>::sCPToCU(iSource, iCountCU, iCountCP, oCountCP); }

/** Return the number of code units that must be traversed to generate \a iCountCP valid code
points in the string buffer starting at \a iSource and extending to \a iEnd. Return the number
of code points actually traversed in \a oCountCP. */
template <class I>
inline size_t sCPToCU(I iSource, I iEnd, size_t iCountCP, size_t* oCountCP)
	{ return Functions_Count_T<I>::sCPToCU(iSource, iSource, iCountCP, oCountCP); }

/** Return the number of valid code points represented by
the code units between \a iSource and \a iSource + \a iCountCU. */
template <class I>
inline size_t sCUToCP(I iSource, size_t iCountCU)
	{ return Functions_Count_T<I>::sCUToCP(iSource, iCountCU); }

/** Return the number of valid code points represented
by the code units between \a iSource and \a iEnd. */
template <class I>
inline size_t sCUToCP(I iSource, I iEnd)
	{ return Functions_Count_T<I>::sCUToCP(iSource, iEnd); }
//@}


/** \name Ensure a pointer is aligned with the first code unit of a valid code point.
*///@{
/** If \a ioCurrent references the first code unit of a valid code point
then leave it unchanged. Otherwise advance it until it does. */
template <class I>
inline void sAlign(I& ioCurrent)
	{ Functions_Read_T<I>::sAlign(ioCurrent); }

/** If \a ioCurrent references the first code unit of a valid code point
then leave it unchanged. Otherwise advance it until it does or until it equals \a iEnd. */
template <class I>
inline void sAlign(I& ioCurrent, I iEnd)
	{ Functions_Read_T<I>::sAlign(ioCurrent, iEnd); }
//@}


/** \name Iterating, reading and writing indvidual code points.
*///@{
/** Update \a ioCurrent to take it past the current valid code point. */
template <class I>
inline void sInc(I& ioCurrent)
	{ return Functions_Read_T<I>::sInc(ioCurrent); }

/**Update \a ioCurrent to take it past the current valid code point.
If that would move \a ioCurrent past \a iEnd then return false,
otherwise return true. */
template <class I>
inline bool sInc(I& ioCurrent, I iEnd)
	{ return Functions_Read_T<I>::sInc(ioCurrent, iEnd); }

/** Decrement \a ioCurrent until it references a valid code point. */
template <class I>
inline void sDec(I& ioCurrent)
	{ return Functions_Read_T<I>::sDec(ioCurrent); }

/** Decrement \a ioCurrent until it references a valid code point. If that would move
\a ioCurrent past \a iStart then return false, otherwise return true. \a iEnd is passed
to ensure that the function does not attempt to read beyond the end of the buffer (only
actually an issue for UTF-8). */
template <class I>
inline bool sDec(I iStart, I& ioCurrent, I iEnd)
	{ return Functions_Read_T<I>::sDec(iStart, ioCurrent, iEnd); }

/** Return the first valid code point at or after \a iCurrent. */
template <class I>
inline UTF32 sRead(I iCurrent)
	{ return Functions_Read_T<I>::sRead(iCurrent); }

/** Return in \a oCP the first valid code point at or after \a iCurrent. If there is no valid
code point between \a iCurrent and \a iEnd then return false. */
template <class I>
inline bool sRead(I iCurrent, I iEnd, UTF32& oCP)
	{ return Functions_Read_T<I>::sRead(iCurrent, iEnd, oCP); }

/** Return the first valid code point at or after \a ioCurrent, and
update \a ioCurrent to point just past its final code unit (not necessarily
at the first code unit of the next valid code point). */
template <class I>
inline UTF32 sReadInc(I& ioCurrent)
	{ return Functions_Read_T<I>::sReadInc(ioCurrent); }

/** Put in \a oCP the first valid code point at or after \a ioCurrent, and
update ioCurrent to point just past its final code unit (not necessarily
at the first code unit of the next valid code point). If there is no valid
code point between \a ioCurrent and \a iEnd then return false. */
template <class I>
inline bool sReadInc(I& ioCurrent, I iEnd, UTF32& oCP)
	{ return Functions_Read_T<I>::sReadInc(ioCurrent, iEnd, oCP); }

/** Put in \a oCP the first valid code point at or after \a ioCurrent, and
update ioCurrent to point just past its final code unit (not necessarily
at the first code unit of the next valid code point). If there is no valid
code point between \a ioCurrent and \a iEnd then return false.
Additionally, add to \a ioCountSkipped the number of code units that were skipped.
*/
template <class I>
inline bool sReadInc(I& ioCurrent, I iEnd, UTF32& oCP, size_t& ioCountSkipped)
	{ return Functions_Read_T<I>::sReadInc(ioCurrent, iEnd, oCP, ioCountSkipped); }

/** Return the first valid code point starting prior to \a ioCurrent. */
template <class I>
inline UTF32 sDecRead(I& ioCurrent)
	{ return Functions_Read_T<I>::sDecRead(ioCurrent); }

/** Put in \a oCP the first valid code point starting prior to \a ioCurrent.
If there is no valid code point between \a iState and \a ioCurrent then return false. */
template <class I>
inline bool sDecRead(I iStart, I& ioCurrent, I iEnd, UTF32& oCP)
	{ return Functions_Read_T<I>::sDecRead(iStart, ioCurrent, iEnd, oCP); }

/** If \a ICP is a valid code point then write it to \a oDest. If there is insufficient space
to hold the code units then return false. Writing an invalid code point will return true,
as invalid code points require zero code units to represent them. */
template <class I>
inline bool sWrite(I oDest, I iEnd, UTF32 iCP)
	{ return Functions_Write_T<I>::sWrite(oDest, iEnd, iCP); }

/** If \a ICP is a valid code point then write it to \a ioDest and advance \a ioDest
appropriately. If there is insufficient space to hold the code units then return false.
Writing an invalid code point will return true, as invalid code points require zero
code units to represent them. */
template <class I>
inline bool sWriteInc(I& ioDest, I iEnd, UTF32 iCP)
	{ return Functions_Write_T<I>::sWriteInc(ioDest, iEnd, iCP); }
//@}

// =================================================================================================
#pragma mark - ZooLib::Unicode, string conversion

/** \name Converting strings to UTF32.
*///@{
template <class I>
inline string32 sAsUTF32(I iSource, size_t iCountCU)
	{ return Functions_Convert_T<I>::sAsUTF32(iSource, iCountCU); }

template <class I>
inline string32 sAsUTF32(I iSource, I iEnd)
	{ return Functions_Convert_T<I>::sAsUTF32(iSource, iEnd); }

inline string32 sAsUTF32(const UTF32* iString)
	{
	if (iString)
		return string32(iString);
	return string32();
	}

inline string32 sAsUTF32(const UTF32* iString, size_t iCountCU)
	{ return string32(iString, iCountCU); }

inline string32 sAsUTF32(const string32& iString)
	{ return iString; }

inline string32 sAsUTF32(const string16& iString)
	{
	return Functions_Convert_T<string16::const_iterator>::sAsUTF32(iString.begin(), iString.size());
	}

inline string32 sAsUTF32(const string8& iString)
	{
	return Functions_Convert_T<string8::const_iterator>::sAsUTF32(iString.begin(), iString.size());
	}
//@}


/** \name Converting strings to UTF16.
*///@{
template <class I>
inline string16 sAsUTF16(I iSource, size_t iCountCU)
	{ return Functions_Convert_T<I>::sAsUTF16(iSource, iCountCU); }

template <class I>
inline string16 sAsUTF16(I iSource, I iEnd)
	{ return Functions_Convert_T<I>::sAsUTF16(iSource, iEnd); }

inline string16 sAsUTF16(const UTF16* iString)
	{
	if (iString)
		return string16(iString);
	return string16();
	}

inline string16 sAsUTF16(const UTF16* iString, size_t iCountCU)
	{ return string16(iString, iCountCU); }

inline string16 sAsUTF16(const string32& iString)
	{
	return Functions_Convert_T<string32::const_iterator>::sAsUTF16(iString.begin(), iString.size());
	}

inline string16 sAsUTF16(const string16& iString)
	{ return iString; }

#if 1
// This one has an optimized implementation in Unicode.cpp
string16 sAsUTF16(const string8& iString);
#else
inline string16 sAsUTF16(const string8& iString)
	{
	return Functions_Convert_T<string8::const_iterator>::sAsUTF16(iString.begin(), iString.size());
	}
#endif
//@}


/** \name Converting strings to UTF8.
*///@{
template <class I>
inline string8 sAsUTF8(I iSource, size_t iCountCU)
	{ return Functions_Convert_T<I>::sAsUTF8(iSource, iCountCU); }

template <class I>
inline string8 sAsUTF8(I iSource, I iEnd)
	{ return Functions_Convert_T<I>::sAsUTF8(iSource, iEnd); }

inline string8 sAsUTF8(const UTF8* iString)
	{
	if (iString)
		return string8(iString);
	return string8();
	}

inline string8 sAsUTF8(const UTF8* iString, size_t iCountCU)
	{ return string8(iString, iCountCU); }

inline string8 sAsUTF8(const string32& iString)
	{
	return Functions_Convert_T<string32::const_iterator>::sAsUTF8(iString.begin(), iString.size());
	}

inline string8 sAsUTF8(const string16& iString)
	{
	return Functions_Convert_T<string16::const_iterator>::sAsUTF8(iString.begin(), iString.size());
	}

inline string8 sAsUTF8(const UTF16* iString)
	{
	if (iString)
		return sAsUTF8(string16(iString));
	return string8();
	}

inline string8 sAsUTF8(const string8& iString)
	{ return iString; }
//@}

// =================================================================================================
#pragma mark - ZooLib::Unicode, buffer conversion

/** \name Converting buffers between UTF32, UTF16 and UTF8
*///@{
void sUTF32ToUTF32(
	const UTF32* iSource, size_t iSourceCount,
	size_t* oSourceCount, size_t* oSourceCountSkipped,
	UTF32* oDest, size_t iDestCount,
	size_t* oDestCount);

void sUTF32ToUTF16(
	const UTF32* iSource, size_t iSourceCount,
	size_t* oSourceCount, size_t* oSourceCountSkipped,
	UTF16* oDest, size_t iDestCU,
	size_t* oDestCU,
	size_t iMaxCP, size_t* oCountCP);

void sUTF32ToUTF8(
	const UTF32* iSource, size_t iSourceCount,
	size_t* oSourceCount, size_t* oSourceCountSkipped,
	UTF8* oDest, size_t iDestCU,
	size_t* oDestCU,
	size_t iMaxCP, size_t* oCountCP);

// -----

bool sUTF16ToUTF32(
	const UTF16* iSource, size_t iSourceCU,
	size_t* oSourceCU, size_t* oSourceCUSkipped,
	UTF32* oDest, size_t iDestCount,
	size_t* oDestCount);

bool sUTF16ToUTF16(
	const UTF16* iSource, size_t iSourceCU,
	size_t* oSourceCU, size_t* oSourceCUSkipped,
	UTF16* oDest, size_t iDestCU,
	size_t* oDestCU,
	size_t iMaxCP, size_t* oCountCP);

bool sUTF16ToUTF8(
	const UTF16* iSource, size_t iSourceCU,
	size_t* oSourceCU, size_t* oSourceCUSkipped,
	UTF8* oDest, size_t iDestCU,
	size_t* oDestCU,
	size_t iMaxCP, size_t* oCountCP);

// -----

bool sUTF8ToUTF32(
	const UTF8* iSource, size_t iSourceCU,
	size_t* oSourceCU, size_t* oSourceCUSkipped,
	UTF32* oDest, size_t iDestCount,
	size_t* oDestCount);

bool sUTF8ToUTF16(
	const UTF8* iSource, size_t iSourceCU,
	size_t* oSourceCU, size_t* oSourceCUSkipped,
	UTF16* oDest, size_t iDestCU,
	size_t* oDestCU,
	size_t iMaxCP, size_t* oCountCP);

bool sUTF8ToUTF8(
	const UTF8* iSource, size_t iSourceCU,
	size_t* oSourceCU, size_t* oSourceCUSkipped,
	UTF8* oDest, size_t iDestCU,
	size_t* oDestCU,
	size_t iMaxCP, size_t* oCountCP);

//@}

// =================================================================================================
#pragma mark - ZooLib::Unicode, wstring conversion

/** \name Converting from UTF8 in a std::string to UTFXX in a std::wstring.
*///@{

#if ZCONFIG_CPP >= 2011
	// Not available for now.
#else
	inline void sAsWString(const string8& iString8, string16& oString16)
		{ oString16 = sAsUTF16(iString8); }

	inline void sAsWString(const string8& iString8, string32& oString32)
		{ oString32 = sAsUTF32(iString8); }

	inline std::wstring sAsWString(const std::string& iString)
		{
		std::wstring result;
		sAsWString(iString, result);
		return result;
		}
#endif

//@}

// =================================================================================================
#pragma mark - ZooLib::Unicode, characterization

/** \name Simple characterization.
*///@{
bool sIsValid(UTF32 iCP);
bool sIsAlpha(UTF32 iCP);
bool sIsDigit(UTF32 iCP);
bool sIsAlphaDigit(UTF32 iCP);
bool sIsWhitespace(UTF32 iCP);
bool sIsEOL(UTF32 iCP);
//@}

// =================================================================================================
#pragma mark - ZooLib::Unicode, simple case conversion

/** \name Case and other conversions.
*///@{
UTF32 sToUpper(UTF32 iCP);
UTF32 sToLower(UTF32 iCP);

string8 sToUpper(const string8& iString);
string8 sToLower(const string8& iString);

int sHexValue(UTF32 iCP);
//@}

} // namespace Unicode
} // namespace ZooLib

#endif // __ZooLib_Unicode_h__
