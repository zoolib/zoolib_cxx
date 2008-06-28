/* -------------------------------------------------------------------------------------------------
Copyright (c) 2001 Andrew Green and Learning in Motion, Inc.
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

// This code is increasingly _not_ derived from Taligent's ConvertUCS4toUTF8 and
// ConvertUTF8toUCS4 etc. But tiny fragments remain and are covered by
// the following copyright:
/* ================================================================ */
/*
File:	ConvertUTF.C
Author: Mark E. Davis
Copyright (C) 1994 Taligent, Inc. All rights reserved.

This code is copyrighted. Under the copyright laws, this code may not
be copied, in whole or part, without prior written consent of Taligent. 

Taligent grants the right to use or reprint this code as long as this
ENTIRE copyright notice is reproduced in the code or reproduction.
The code is provided AS-IS, AND TALIGENT DISCLAIMS ALL WARRANTIES,
EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN
NO EVENT WILL TALIGENT BE LIABLE FOR ANY DAMAGES WHATSOEVER (INCLUDING,
WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS
INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR OTHER PECUNIARY
LOSS) ARISING OUT OF THE USE OR INABILITY TO USE THIS CODE, EVEN
IF TALIGENT HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
BECAUSE SOME STATES DO NOT ALLOW THE EXCLUSION OR LIMITATION OF
LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES, THE ABOVE
LIMITATION MAY NOT APPLY TO YOU.

RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the
government is subject to restrictions as set forth in subparagraph
(c)(l)(ii) of the Rights in Technical Data and Computer Software
clause at DFARS 252.227-7013 and FAR 52.227-19.

This code may be protected by one or more U.S. and International
Patents.

TRADEMARKS: Taligent and the Taligent Design Mark are registered
trademarks of Taligent, Inc.
*/
/* ================================================================ */

/**
\defgroup group_Unicode Unicode
\sa ZUnicode

If you're unfamiliar with Unicode I recommend reading Markus Kuhn's excellent backgrounder
<a href="http://www.cl.cam.ac.uk/~mgk25/unicode.html">UTF-8 and Unicode FAQ for Unix/Linux</a>.

"The Unicode Standard Version 3.0" (Addison-Wesley, ISBN 0-201-61633-5) is an essential reference.

In working with Unicode data it is crucial to understand the distinction
between code points and code units. A clear discussion is at
<a href="http://www.unicode.org/unicode/reports/tr17/">Unicode Technical Report
#17, Character Encoding Model</a>. In ZooLib a Unicode code point is an integer in the
range 0-0xD7FF or 0xE000-0x10FFFF that represents the corresponding character as
defined by Unicode and ISO 10646. This requires at least 21 bits of space,
and is most conveniently manipulated in a 32 bit integer.

A code unit may be large enough to hold an arbitrary code point, but generally more
than one code unit may be required.
*/

/**
\namespace ZUnicode
\brief The ZUnicode namespace defines a collection of data types and functions for
working with Unicode data.
\ingroup group_Unicode
\sa group_Unicode

ZUnicode does not directly address internationalization and localization issues, but it
is an important building block in support of such work.

ZUnicode defines three integer types used to hold code units:
	- \c UTF32. A 32 bit integer. On platforms where \c wchar_t is 32 bits in size, \c UTF32 \e is
a \c wchar_t, otherwise it is a \c uint32.
	- \c UTF16. A 16 bit integer. On platforms where \c wchar_t is 16 bits in size, \c UTF16 \e is
a \c wchar_t, otherwise it is a \c uint16.
	- \c UTF8. An 8 bit integer. It is a typedef of \c char.

and three corresponding string types:
	- \c string32.
	- \c string16.
	- \c string8. This will be equivalent to \c std::string.

If you need to work with individual code points use \c UTF32. In general \c UTF16 and \c UTF8
can hold only individual code units of their type. It may be that the UTF-16 or UTF-8
representation of a particular code point can be represented in a single code unit, but should
never be relied upon in your code.

<h3>Legal code points</h3>

ISO 10646 formally defines a 31 bit character set, the range of legal code points is
thus from 0 to 0x7FFFFFFF. This range is considered to be divided into 32768 planes of
65536 code points. As of this writing characters have only been assigned to plane 0,
the so-called Basic Multilingual Plane (BMP). There is a committment to never assign 
characters beyond plane 16. So the range of legal code points is restricted to
0 to 0x10FFFF (1,114,112 distinct code points). UCS-4 and UTF-8 can represent the
entire 31 bit range, but UTF-16 is unable to represent code points beyond plane 16.

<h3>Illegal code units and code unit sequences</h3>

There are two illegal code units in UTF-8, 0xFE and 0xFF. It's also possible to have
sequences of UTF-8 code units which are illegal, that is which do not map to a code point.
For example if a continuation byte is not preceded by a start byte, or if a start byte
is not immediately followed by enough continuation bytes.

UTF-16 also has illegal code unit sequences, in particular a high surrogate that
is not followed by a low surrogate, or a low surrogate without a preceding high surrogate.

\c UTF32 is a 32 bit integer, and thus can represent values outside the 31 bit ISO 10646
range. So it's clearly possible to have \c UTF32 code units that do not map to a code point. Given
that Unicode and ISO 10646 will never assign characters beyond plane 16, we further restrict
valid UTF-32 code units to be in the range 0 to 0x10FFFF. Finally, code units from the
high and low surrgate blocks (0xDC00 to 0xDFFF) are also illegal. Strictly speaking U+FFFF
and all code points of the form U+xxFFFF are illegal, but we don't filter them out.

The Unicode standard recommends that illegal code units are each decoded as U+FFFD, the
replacement character. That's a good strategy when decoding a body of material in
one hit, but it's ambiguous when randomly accessing data in memory. ZUnicode's
convention is that illegal code units and code unit sequences are skipped.
They contribute to any count of code units, but do not generate code points and do not
contribute to any count of code points. So a pointer into a sequence of code units
is considered to reference the first valid code point starting at or subsequent to the
pointer. Conversely, illegal code points, those outside the ranges 0-0xD7FF and 0xE000-0x10FFFF,
are treated as being of zero length. They will not cause generation of code units nor
contribute to counts of code points.

An example of mapping between offsets and code points is perhaps in order. The following
represents ten UTF-8 code units, with offsets of contained bytes from zero to nine and
offset ten being the end of the buffer:

\verbatim
Offset  Description
------  -----------
 0      Start of 3 byte sequence, with two continuation bytes following
 3      Single byte character
 4      Single byte character
 5      An out of order continuation byte (illegal)
 6      Start of 2 byte sequence, with single continuation byte following
 8      Single byte character
 9      Start byte of two byte sequence without continuation byte (illegal)
10      End of the buffer

Offset    01234567890
Value     3CCNNC2CN2
Illegal   -----X---X

The table below shows for each offset which offset will be returned/used when
decremented, accessed or incremented. You should note that illegal byte sequences
are effectively transparent to those operations.

Offset  Dec  Acc  Inc
 0       -    0    3
 1       0    3    4
 2       0    3    4
 3       0    3    4
 4       3    4    6
 5       4    6    8
 6       4    6    8
 7       6    8    -
 8       6    8    -
 9       8    -    -
10       8    -    -
\endverbatim



\bug
CodeWarrior building for Mach-o defines wchar_t to be a 32 bits in size. The CW debugger,
as of 8.3 at least, always treats wchar_t as being 16 bits in size, which can make
things very confusing. You can have the compiler define wchar_t to be 16 bits in size
by using <code>#pragma ushort_wchar_t on</code>
*/

// =================================================================================================

#include "zoolib/ZUnicode.h"
#include "zoolib/ZDebug.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(ICU)
#	include "unicode/uchar.h"
#endif

#include <cctype>

ZAssertCompile(sizeof(UTF32) == 4);
ZAssertCompile(sizeof(UTF16) == 2);
ZAssertCompile(sizeof(UTF8) == 1);

#if defined(__GNUC__) && __GNUC__ == 3 && __GNUC_MINOR__ >= 2 && __MACH__
	// MacOS X's 3.2 std C++ library has out of line definitions only for basic_string<char>
	// methods, but we also need definitions for UTF16 and UTF32 variants, hence
	// these explicit instantiations.
	template class std::basic_string<UTF16>;
	template class std::basic_string<UTF32>;
#endif

// =================================================================================================

string16 operator+(UTF32 iCP, const string16& iString)
	{
	string16 temp = ZUnicode::sAsUTF16(&iCP, 1);
	return temp += iString;
	}

string16& operator+=(string16& ioString, UTF32 iCP)
	{
	uint32 realCP = iCP;
	if (realCP <= ZUnicode::kCPMaxUCS2)
		{
		ioString += UTF16(realCP);
		}
	else
		{
		realCP -= 0x10000;
		ioString += UTF16(realCP / 0x400 + ZUnicode::kCPSurrogateHighBegin);
		ioString += UTF16(realCP & 0x3FF + ZUnicode::kCPSurrogateLowBegin);
		}
	return ioString;
	}

string8 operator+(UTF32 iCP, const string8& iString)
	{
	string8 temp = ZUnicode::sAsUTF8(&iCP, 1);
	return temp += iString;
	}

string8& operator+=(string8& ioString, UTF32 iCP)
	{
	uint32 realCP = iCP;
	if (realCP < 0x80)
		{
		ioString += UTF8(realCP);
		}
	else
		{
		size_t bytesToWrite;
		if (realCP < 0x800) bytesToWrite = 2;
		else if (realCP < 0x10000) bytesToWrite = 3;
		else if (realCP < 0x200000) bytesToWrite = 4;
		else if (realCP < 0x4000000) bytesToWrite = 5;
		else bytesToWrite = 6;
		ioString.resize(ioString.size() + bytesToWrite);
		string8::iterator iter = ioString.end();
		const UTF32 byteMask = 0xBF;
		const UTF32 byteMark = 0x80;
		switch (bytesToWrite)
			{
			// note: code falls through cases
			case 6:	*--iter = (realCP | byteMark) & byteMask; realCP >>= 6;
			case 5:	*--iter = (realCP | byteMark) & byteMask; realCP >>= 6;
			case 4:	*--iter = (realCP | byteMark) & byteMask; realCP >>= 6;
			case 3:	*--iter = (realCP | byteMark) & byteMask; realCP >>= 6;
			case 2:	*--iter = (realCP | byteMark) & byteMask; realCP >>= 6;
			}
		*--iter = realCP | ZUnicode::sUTF8StartByteMark[bytesToWrite];	
		}
	return ioString;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Helper inlines and lookup tables

namespace ZUnicode {

const uint8 sUTF8SequenceLength[256] =
	{
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // Standalone
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // Standalone
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // Standalone
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // Standalone
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // Continuation
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // Continuation
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // Start, 2 byte
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3, // Start, 3 byte
	4,4,4,4,4,4,4,4, // Start, 4 byte
	5,5,5,5, // Start, 5 byte
	6,6, // Start, 6 byte
	0,0 // 0xFE and 0xFF are illegal UTF8 code units.
	}; 

// These arrays are indexed by sequence length. So the element 0 is never
// referenced. Element 1 is what would be used for standalone characters,
// but isn't because it's special cased. The rest for sequences of overall
// length from 2 to 6 bytes.
const uint8 sUTF8StartByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };
const uint8 sUTF8StartByteMask[7] = { 0x00, 0x00, 0x3F, 0x1F, 0x0F, 0x07, 0x03 };

} // namespace ZUnicode

// =================================================================================================
#pragma mark -
#pragma mark * Explicit instantiations of the template code

#include "zoolib/ZUnicodePrivB.h"

namespace ZUnicode {

template struct Functions_CountCU_T<string32::const_iterator>;
template struct Functions_CountCU_T<string16::const_iterator>;
template struct Functions_CountCU_T<string8::const_iterator>;

template struct Functions_CountCU_T<string32::iterator>;
template struct Functions_CountCU_T<string16::iterator>;
template struct Functions_CountCU_T<string8::iterator>;


template struct Functions_Count_T<const UTF32*>;
template struct Functions_Count_T<const UTF16*>;
template struct Functions_Count_T<const UTF8*>;

template struct Functions_Count_T<UTF32*>;
template struct Functions_Count_T<UTF16*>;
template struct Functions_Count_T<UTF8*>;

template struct Functions_Count_T<string32::const_iterator>;
template struct Functions_Count_T<string16::const_iterator>;
template struct Functions_Count_T<string8::const_iterator>;

template struct Functions_Count_T<string32::iterator>;
template struct Functions_Count_T<string16::iterator>;
template struct Functions_Count_T<string8::iterator>;


template struct Functions_Read_T<const UTF32*>;
template struct Functions_Read_T<const UTF16*>;
template struct Functions_Read_T<const UTF8*>;

template struct Functions_Read_T<UTF32*>;
template struct Functions_Read_T<UTF16*>;
template struct Functions_Read_T<UTF8*>;

template struct Functions_Read_T<string32::const_iterator>;
template struct Functions_Read_T<string16::const_iterator>;
template struct Functions_Read_T<string8::const_iterator>;

template struct Functions_Read_T<string32::iterator>;
template struct Functions_Read_T<string16::iterator>;
template struct Functions_Read_T<string8::iterator>;


template struct Functions_Write_T<UTF32*>;
template struct Functions_Write_T<UTF16*>;
template struct Functions_Write_T<UTF8*>;

template struct Functions_Write_T<string32::iterator>;
template struct Functions_Write_T<string16::iterator>;
template struct Functions_Write_T<string8::iterator>;


template struct Functions_Convert_T<const UTF32*>;
template struct Functions_Convert_T<const UTF16*>;
template struct Functions_Convert_T<const UTF8*>;

template struct Functions_Convert_T<UTF32*>;
template struct Functions_Convert_T<UTF16*>;
template struct Functions_Convert_T<UTF8*>;

template struct Functions_Convert_T<string32::const_iterator>;
template struct Functions_Convert_T<string16::const_iterator>;
template struct Functions_Convert_T<string8::const_iterator>;

template struct Functions_Convert_T<string32::iterator>;
template struct Functions_Convert_T<string16::iterator>;
template struct Functions_Convert_T<string8::iterator>;

} // namespace ZUnicode

// =================================================================================================
#pragma mark -
#pragma mark * Converting between different serializations

/**Read UTF32 code units from \a iSource, convert them into valid
code points and store them as UTF8 code units starting at \a iDest. Do not read more
than \a iSourceCount UTF32 code units, and do not store more than \a
iDestCU UTF8 code units. Report the counts read and written in \a oSourceCount
and \a oDestCU.
*/
void ZUnicode::sUTF32ToUTF8(const UTF32* iSource, size_t iSourceCount,
							size_t* oSourceCount,
							UTF8* iDest, size_t iDestCU,
							size_t* oDestCU, size_t* oCountCP)
	{
	sUTF32ToUTF8(iSource, iSourceCount, oSourceCount, nil, iDest, iDestCU, oDestCU, oCountCP);
	}

void ZUnicode::sUTF32ToUTF8(const UTF32* iSource, size_t iSourceCount,
							size_t* oSourceCount, size_t* oSourceCountSkipped,
							UTF8* iDest, size_t iDestCU,
							size_t* oDestCU, size_t* oCountCP)
	{
	const UTF32* localSource = iSource;
	const UTF32* localSourceEnd = iSource + iSourceCount;

	UTF8* localDest = iDest;
	UTF8* localDestEnd = iDest + iDestCU;

	size_t localCP = 0;

	if (oSourceCountSkipped)
		{
		*oSourceCountSkipped = 0;
		while (localSource < localSourceEnd && localDest < localDestEnd)
			{
			const UTF32* priorLocalSource = localSource;
			UTF32 theCP;
			size_t countSkipped = 0;
			if (!sReadInc(localSource, localSourceEnd, theCP, countSkipped))
				{
				// Although we test for localSource < localSourceEnd above,
				// it's still possible for there to be no valid UTF32 characters
				// between localSource and localSourceEnd, in which case this is what we'll hit.
				break;
				}

			if (!sWriteInc(localDest, localDestEnd, theCP))
				{
				// Insufficient space to write the CP. Restore localSource so
				// we'll correctly report the number of CUs consumed to generate
				// the output we were able to generate.
				localSource = priorLocalSource;
				break;
				}
			*oSourceCountSkipped += countSkipped;
			++localCP;
			}
		}
	else
		{
		while (localSource < localSourceEnd && localDest < localDestEnd)
			{
			const UTF32* priorLocalSource = localSource;
			UTF32 theCP;
			if (!sReadInc(localSource, localSourceEnd, theCP))
				{
				// Although we test for localSource < localSourceEnd above,
				// it's still possible for there to be no valid UTF32 characters
				// between localSource and localSourceEnd, in which case this is what we'll hit.
				break;
				}

			if (!sWriteInc(localDest, localDestEnd, theCP))
				{
				// Insufficient space to write the CP. Restore localSource so
				// we'll correctly report the number of CUs consumed to generate
				// the output we were able to generate.
				localSource = priorLocalSource;
				break;
				}
			++localCP;
			}
		}

	if (oSourceCount)
		*oSourceCount = localSource - iSource;
	if (oDestCU)
		*oDestCU = localDest - iDest;
	if (oCountCP)
		*oCountCP = localCP;
	}

/**Read UTF8 code units from \a iSource, convert them into valid
code points and store them as UTF32 code units starting at \a iDest. Do not read more
than \a iSourceCU UTF8 code units, and do not store more than \a iDestCount
UTF32 code units. Report the counts read and written in \a oSourceCU
and \a oDestCount. Return false if fewer than \a iSourceCU UTF8 code units were
read because there was a valid prefix at the end of the buffer that could
represent a valid code point if more data were provided.
*/
bool ZUnicode::sUTF8ToUTF32(const UTF8* iSource, size_t iSourceCU,
							size_t* oSourceCU,
							UTF32* iDest, size_t iDestCount,
							size_t* oDestCount)
	{
	return sUTF8ToUTF32(iSource, iSourceCU, oSourceCU, nil, iDest, iDestCount, oDestCount);
	}

bool ZUnicode::sUTF8ToUTF32(const UTF8* iSource, size_t iSourceCU,
							size_t* oSourceCU, size_t* oSourceCUSkipped,
							UTF32* iDest, size_t iDestCount,
							size_t* oDestCount)
	{
	const UTF8* localSource = iSource;
	const UTF8* localSourceEnd = iSource + iSourceCU;

	UTF32* localDest = iDest;
	UTF32* localDestEnd = iDest + iDestCount;

	bool sourceComplete = true;

	if (oSourceCUSkipped)
		{
		*oSourceCUSkipped = 0;
		while (localSource < localSourceEnd && localDest < localDestEnd)
			{
			const UTF8* priorLocalSource = localSource;
			UTF32 theCP;
			size_t cuSkipped = 0;
			if (!sReadInc(localSource, localSourceEnd, theCP, cuSkipped))
				{
				if (localSource < localSourceEnd)
					sourceComplete = false;
				break;
				}

			if (!sWriteInc(localDest, localDestEnd, theCP))
				{
				localSource = priorLocalSource;
				break;
				}
			*oSourceCUSkipped += cuSkipped;
			}
		}
	else
		{
		while (localSource < localSourceEnd && localDest < localDestEnd)
			{
			const UTF8* priorLocalSource = localSource;
			UTF32 theCP;
			if (!sReadInc(localSource, localSourceEnd, theCP))
				{
				if (localSource < localSourceEnd)
					sourceComplete = false;
				break;
				}

			if (!sWriteInc(localDest, localDestEnd, theCP))
				{
				localSource = priorLocalSource;
				break;
				}
			}
		}

	if (oSourceCU)
		*oSourceCU = localSource - iSource;
	if (oDestCount)
		*oDestCount = localDest - iDest;
	return sourceComplete;
	}

/**Read UTF32 code units from \a iSource, convert them into valid
code points and store them as UTF16 code units starting at \a iDest. Do not read more
than \a iSourceCount UTF32 code units, and do not store more than \a iDestCU
UTF16 code units. Report the code units read and written in \a oSourceCount
and \a oDestCU, and the code points written in oCountCP.
*/
void ZUnicode::sUTF32ToUTF16(const UTF32* iSource, size_t iSourceCount,
							size_t* oSourceCount,
							UTF16* iDest, size_t iDestCU,
							size_t* oDestCU, size_t* oCountCP)
	{
	sUTF32ToUTF16(iSource, iSourceCount, oSourceCount, nil, iDest, iDestCU, oDestCU, oCountCP);
	}

void ZUnicode::sUTF32ToUTF16(const UTF32* iSource, size_t iSourceCount,
							size_t* oSourceCount, size_t* oSourceCountSkipped,
							UTF16* iDest, size_t iDestCU,
							size_t* oDestCU, size_t* oCountCP)
	{
	const UTF32* localSource = iSource;
	const UTF32* localSourceEnd = iSource + iSourceCount;

	UTF16* localDest = iDest;
	UTF16* localDestEnd = iDest + iDestCU;

	size_t localCP = 0;

	if (oSourceCountSkipped)
		{
		*oSourceCountSkipped = 0;
		while (localSource < localSourceEnd && localDest < localDestEnd)
			{
			const UTF32* priorLocalSource = localSource;
			UTF32 theCP;
			size_t countSkipped = 0;
			if (!sReadInc(localSource, localSourceEnd, theCP, countSkipped))
				break;

			if (!sWriteInc(localDest, localDestEnd, theCP))
				{
				localSource = priorLocalSource;
				break;
				}
			*oSourceCountSkipped += countSkipped;
			++localCP;
			}
		}
	else
		{
		while (localSource < localSourceEnd && localDest < localDestEnd)
			{
			const UTF32* priorLocalSource = localSource;
			UTF32 theCP;
			if (!sReadInc(localSource, localSourceEnd, theCP))
				break;

			if (!sWriteInc(localDest, localDestEnd, theCP))
				{
				localSource = priorLocalSource;
				break;
				}
			++localCP;
			}
		}

	if (oSourceCount)
		*oSourceCount = localSource - iSource;
	if (oDestCU)
		*oDestCU = localDest - iDest;
	if (oCountCP)
		*oCountCP = localCP;
	}

/**Read UTF16 code units from \a iSource, convert them into valid
code points and store them as UTF32 code units starting at \a iDest. Do not read more
than \a iSourceCU UTF16 code units, and do not store more than \a iDestCount
UTF32 code units. Report the counts read and written in \a oSourceCU
and \a oDestCount. Return false if fewer than \a iSourceCU UTF8 code units were
read because there was a valid prefix at the end of the buffer that could
represent a valid code point if more data were provided.
*/
bool ZUnicode::sUTF16ToUTF32(const UTF16* iSource, size_t iSourceCU,
							size_t* oSourceCU,
							UTF32* iDest, size_t iDestCount,
							size_t* oDestCount)
	{
	return sUTF16ToUTF32(iSource, iSourceCU, oSourceCU, nil, iDest, iDestCount, oDestCount);
	}

bool ZUnicode::sUTF16ToUTF32(const UTF16* iSource, size_t iSourceCU,
							size_t* oSourceCU, size_t* oSourceCUSkipped,
							UTF32* iDest, size_t iDestCount,
							size_t* oDestCount)
	{
	const UTF16* localSource = iSource;
	const UTF16* localSourceEnd = iSource + iSourceCU;

	UTF32* localDest = iDest;
	UTF32* localDestEnd = iDest + iDestCount;

	bool sourceComplete = true;

	if (oSourceCUSkipped)
		{
		*oSourceCUSkipped = 0;
		while (localSource < localSourceEnd && localDest < localDestEnd)
			{
			const UTF16* priorLocalSource = localSource;
			UTF32 theCP;
			size_t cuSkipped = 0;
			if (!sReadInc(localSource, localSourceEnd, theCP, cuSkipped))
				{
				if (localSource < localSourceEnd)
					sourceComplete = false;
				break;
				}

			if (!sWriteInc(localDest, localDestEnd, theCP))
				{
				localSource = priorLocalSource;
				break;
				}
			*oSourceCUSkipped += cuSkipped;
			}
		}
	else
		{
		while (localSource < localSourceEnd && localDest < localDestEnd)
			{
			const UTF16* priorLocalSource = localSource;
			UTF32 theCP;
			if (!sReadInc(localSource, localSourceEnd, theCP))
				{
				if (localSource < localSourceEnd)
					sourceComplete = false;
				break;
				}

			if (!sWriteInc(localDest, localDestEnd, theCP))
				{
				localSource = priorLocalSource;
				break;
				}
			}
		}

	if (oSourceCU)
		*oSourceCU = localSource - iSource;
	if (oDestCount)
		*oDestCount = localDest - iDest;
	return sourceComplete;
	}


/**Read UTF16 code units from \a iSource, convert them into valid
code points and store them as UTF8 code units starting at \a iDest. Do not read more
than \a iSourceCU UTF16 code units, and do not store more than \a iDestCU
UTF8 code units. Do not consume/generate more than \a iMaxCP code points.
Report the counts read and written in \a oSourceCU and \a oDestCU, and
the number of code points in \a oCountCP. Return false if fewer than \a iSourceCU
UTF16 code units were read because there was a valid prefix at the end of the buffer that could
represent a valid code point if more data were provided.
*/
bool ZUnicode::sUTF16ToUTF8(const UTF16* iSource, size_t iSourceCU,
							size_t* oSourceCU,
							UTF8* iDest, size_t iDestCU,
							size_t* oDestCU, size_t iMaxCP, size_t* oCountCP)
	{
	return sUTF16ToUTF8(iSource, iSourceCU, oSourceCU, nil,
					iDest, iDestCU, oDestCU,
					iMaxCP, oCountCP);
	}

bool ZUnicode::sUTF16ToUTF8(const UTF16* iSource, size_t iSourceCU,
							size_t* oSourceCU, size_t* oSourceCUSkipped,
							UTF8* iDest, size_t iDestCU,
							size_t* oDestCU, size_t iMaxCP, size_t* oCountCP)
	{
	const UTF16* localSource = iSource;
	const UTF16* localSourceEnd = iSource + iSourceCU;

	UTF8* localDest = iDest;
	UTF8* localDestEnd = iDest + iDestCU;

	size_t localCP = iMaxCP;
	bool sourceComplete = true;

	if (oSourceCUSkipped)
		{
		*oSourceCUSkipped = 0;
		while (localSource < localSourceEnd && localDest < localDestEnd && localCP)
			{
			const UTF16* priorLocalSource = localSource;
			UTF32 theCP;
			size_t cuSkipped = 0;
			if (!sReadInc(localSource, localSourceEnd, theCP, cuSkipped))
				{
				if (localSource < localSourceEnd)
					sourceComplete = false;
				break;
				}

			if (!sWriteInc(localDest, localDestEnd, theCP))
				{
				localSource = priorLocalSource;
				break;
				}
			--localCP;
			*oSourceCUSkipped += cuSkipped;
			}
		}
	else
		{
		while (localSource < localSourceEnd && localDest < localDestEnd && localCP)
			{
			const UTF16* priorLocalSource = localSource;
			UTF32 theCP;
			if (!sReadInc(localSource, localSourceEnd, theCP))
				{
				if (localSource < localSourceEnd)
					sourceComplete = false;
				break;
				}

			if (!sWriteInc(localDest, localDestEnd, theCP))
				{
				localSource = priorLocalSource;
				break;
				}
			--localCP;
			}
		}

	if (oSourceCU)
		*oSourceCU = localSource - iSource;
	if (oDestCU)
		*oDestCU = localDest - iDest;
	if (oCountCP)
		*oCountCP = iMaxCP - localCP;
	return sourceComplete;
	}

/**Read UTF8 code units from \a iSource, convert them into valid code points and store
them as UTF16 code units starting at \a iDest. Do not read more than \a iSourceCU UTF8
code units, and do not store more than \a iDestCU UTF16 code units. Do not
consume/generate more than \a iMaxCP code points. Report the counts read and written
in \a oSourceCU and \a oDestCU, and the number of code points in \a oCountCP. Return
false if fewer than \a iSourceCU UTF8 code units were read because there was a valid
prefix at the end of the buffer that could represent a valid code point if more data
were provided.
*/
bool ZUnicode::sUTF8ToUTF16(const UTF8* iSource, size_t iSourceCU,
							size_t* oSourceCU,
							UTF16* iDest, size_t iDestCU,
							size_t* oDestCU, size_t iMaxCP, size_t* oCountCP)
	{
	return sUTF8ToUTF16(iSource, iSourceCU, oSourceCU, nil,
			iDest, iDestCU, oDestCU,
			iMaxCP, oCountCP);
	}

bool ZUnicode::sUTF8ToUTF16(const UTF8* iSource, size_t iSourceCU,
							size_t* oSourceCU, size_t* oSourceCUSkipped,
							UTF16* iDest, size_t iDestCU,
							size_t* oDestCU, size_t iMaxCP, size_t* oCountCP)
	{
	const UTF8* localSource = iSource;
	const UTF8* localSourceEnd = iSource + iSourceCU;

	UTF16* localDest = iDest;
	UTF16* localDestEnd = iDest + iDestCU;

	size_t localCP = iMaxCP;
	bool sourceComplete = true;

	if (oSourceCUSkipped)
		{
		*oSourceCUSkipped = 0;
		while (localSource < localSourceEnd && localDest < localDestEnd && localCP)
			{
			const UTF8* priorLocalSource = localSource;
			UTF32 theCP;
			size_t cuSkipped = 0;
			if (!sReadInc(localSource, localSourceEnd, theCP, cuSkipped))
				{
				if (localSource < localSourceEnd)
					sourceComplete = false;
				break;
				}

			if (!sWriteInc(localDest, localDestEnd, theCP))
				{
				localSource = priorLocalSource;
				break;
				}
			--localCP;
			*oSourceCUSkipped += cuSkipped;
			}
		}
	else
		{
		while (localSource < localSourceEnd && localDest < localDestEnd && localCP)
			{
			const UTF8* priorLocalSource = localSource;
			UTF32 theCP;
			if (!sReadInc(localSource, localSourceEnd, theCP))
				{
				if (localSource < localSourceEnd)
					sourceComplete = false;
				break;
				}

			if (!sWriteInc(localDest, localDestEnd, theCP))
				{
				localSource = priorLocalSource;
				break;
				}
			--localCP;
			}
		}

	if (oSourceCU)
		*oSourceCU = localSource - iSource;
	if (oDestCU)
		*oDestCU = localDest - iDest;
	if (oCountCP)
		*oCountCP = iMaxCP - localCP;
	return sourceComplete;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Basic characterization of code points

bool ZUnicode::sIsValid(UTF32 iCP)
	{
	return sIsValidCP(iCP);
	}

bool ZUnicode::sIsAlpha(UTF32 iCP)
	{
	#if ZCONFIG_SPI_Enabled(ICU)

		return u_isalpha(iCP);

	#else

		return uint32(iCP) < 0x80 && isalpha(iCP);

	#endif
	}

bool ZUnicode::sIsDigit(UTF32 iCP)
	{
	#if ZCONFIG_SPI_Enabled(ICU)

		return u_isdigit(iCP);

	#else

		return uint32(iCP) < 0x80 && isdigit(iCP);

	#endif
	}

bool ZUnicode::sIsAlphaDigit(UTF32 iCP)
	{
	#if ZCONFIG_SPI_Enabled(ICU)

		return u_isalnum(iCP);

	#else

		return uint32(iCP) < 0x80 ? isalnum(iCP) : iCP < 0xFF;

	#endif
	}

bool ZUnicode::sIsWhitespace(UTF32 iCP)
	{
	#if ZCONFIG_SPI_Enabled(ICU)

		return u_isWhitespace(iCP);

	#else

		return uint32(iCP) < 0x80 && isspace(iCP);

	#endif
	}

bool ZUnicode::sIsEOL(UTF32 iCP)
	{
	switch (iCP)
		{
		case '\n':
		case '\r':
		case 0x2028:
		case 0x2029:
			return true;
		}
	return false;
	}

UTF32 ZUnicode::sToUpper(UTF32 iCP)
	{
	#if ZCONFIG_SPI_Enabled(ICU)

		return u_toupper(iCP);

	#else

		return uint32(iCP) < 0x80 ? toupper(iCP) : iCP;

	#endif
	}

UTF32 ZUnicode::sToLower(UTF32 iCP)
	{
	#if ZCONFIG_SPI_Enabled(ICU)

		return u_tolower(iCP);

	#else

		return uint32(iCP) < 0x80 ? tolower(iCP) : iCP;

	#endif
	}

string8 ZUnicode::sToLower(const string8& iString)
	{
	string8 result;
	result.reserve(iString.size());

	string8::const_iterator current = iString.begin();
	string8::const_iterator end = iString.end();
	for (;;)
		{
		UTF32 theUTF32;
		if (!sReadInc(current, end, theUTF32))
			break;
		result += sToLower(theUTF32);
		}
	return result;
	}

string8 ZUnicode::sToUpper(const string8& iString)
	{
	string8 result;
	result.reserve(iString.size());

	string8::const_iterator current = iString.begin();
	string8::const_iterator end = iString.end();
	for (;;)
		{
		UTF32 theUTF32;
		if (!sReadInc(current, end, theUTF32))
			break;
		result += sToUpper(theUTF32);
		}
	return result;
	}

int ZUnicode::sHexValue(UTF32 iCP)
	{
	if (iCP >= '0' && iCP <= '9')
		return iCP - '0';

	if (iCP >= 'a' && iCP <= 'f')
		return iCP - 'a' + 10;

	if (iCP >= 'A' && iCP <= 'F')
		return iCP - 'A' + 10;

	return -1;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Optimized implementations of sAsUTFXX

string16 ZUnicode::sAsUTF16(const string8& iString)
	{
	if (iString.empty())
		return string16();

	string16 result(iString.size(), 0);
	UTF16* dest = const_cast<UTF16*>(result.data());

	const char* source = iString.data();
	const char* sourceEnd = source + iString.size();

	for (;;)
		{
		if (source >= sourceEnd)
			break;

		uint8 firstByte = *source++;
		if (firstByte <= 0x7F)
			{
			*dest++ = firstByte;
			}
		else
			{
			size_t sequenceLength = sUTF8SequenceLength[firstByte];
			if (sequenceLength == 0)
				{
				// It's a continuation or illegal, ignore it.
				}
			else
				{
				if (source + sequenceLength - 1 > sourceEnd)
					break;

				uint32 result = firstByte & sUTF8StartByteMask[sequenceLength];
				bool okay = true;
				while (--sequenceLength)
					{
					const uint8 curByte = *source++;
					if (!sIsContinuation(curByte))
						{
						// It's not a legal continuation byte.
						--source;
						okay = false;
						break;
						}
					sAppendContinuation(result, curByte);
					}

				if (okay)
					{
					if (result > kCPMaxUCS2)
						{
						result -= 0x10000;
						*dest++ = result / 0x400 + kCPSurrogateHighBegin;
						*dest++ = result & 0x3FF + kCPSurrogateLowBegin;
						}
					else
						{
						*dest++ = result;
						}
					}
				}
			}
		}
	result.resize(dest - result.data());
	return result;
	}
