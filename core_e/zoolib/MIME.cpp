/* -------------------------------------------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/MIME.h"

#include "zoolib/ZDebug.h"

#include <ctype.h>

static const char CR = '\r';
static const char LF = '\n';

#define kDebug_MIME 2

namespace ZooLib {
namespace MIME {

// =================================================================================================
// MARK: - ZMIME

// Character classification.
bool sIs_LWS(char iChar)
	{
	return iChar == ' ' || iChar == '\t';
	}

bool sReadFieldName(const ChanR_Bin& iChanR, std::string* oName, std::string* oNameExact)
	{
	if (oName)
		oName->resize(0);
	if (oNameExact)
		oNameExact->resize(0);

	bool gotAny = false;

	for (;;)
		{
		byte readChar;
		if (not sQRead(readChar, iChanR))
			return gotAny;

		gotAny = true;

		if ((readChar >= 0 && readChar <= 32) || readChar == 127)
			{
			// Control characters, space and DEL (127) are illegal and make for a
			// malformed field name. We return true to indicate that the line was
			// not completely empty, but resize oNameXX to indicate that a
			// well-formed field name was not found.
			if (oName)
				oName->resize(0);
			if (oNameExact)
				oNameExact->resize(0);
			return true;
			}

		if (readChar == ':')
			{
			// ':' terminates a field name. Should we suck up the space that always seems
			// to follow? Or is that considered part of the field body, as RFC822 indicates?
			return true;
			}

		if (oName)
			oName->append(1, char(tolower(readChar)));
		if (oNameExact)
			oNameExact->append(1, readChar);
		}
	}

// =================================================================================================
// MARK: - ChanR_Bin_Header

ChanR_Bin_Header::ChanR_Bin_Header(const ChanR_Bin& iChanR)
:	fChanR(iChanR),
	fState(eInitial)
	{}

size_t ChanR_Bin_Header::QRead(byte* oDest, size_t iCount)
	{
	byte* localDest = static_cast<byte*>(oDest);
	byte* localEnd = localDest + iCount;

	while (localDest < localEnd && fState != eSeen_LF_LF)
		{
		switch (fState)
			{
			case eReturn_LF_X:
				{
				*localDest++ = LF;
				fState = eReturn_X;
				continue;
				}
			case eReturn_X:
				{
				*localDest++ = fX;
				fState = eNormal;
				continue;
				}
			default:
				break;
			}

		if (not sQRead(fX, fChanR))
			break;

		if (fX == CR)
			continue;

		switch (fState)
			{
			case eInitial:
				{
				if (fX == LF)
					fState = eSeen_LF_LF;
				else
					fState = eReturn_X;
				break;
				}
			case eNormal:
				{
				if (fX == LF)
					fState = eSeen_LF;
				else
					fState = eReturn_X;
				break;
				}
			case eSeen_LF:
				{
				if (fX == LF)
					fState = eSeen_LF_LF;
				else if (sIs_LWS(fX)) // Unfolding
					fState = eReturn_X;
				else
					fState = eReturn_LF_X;
				break;
				}
			default:
				ZUnimplemented();
			}
		}

	return localDest - static_cast<byte*>(oDest);
	}

void ChanR_Bin_Header::Reset()
	{ fState = eInitial; }

// =================================================================================================
// MARK: - ChanR_Bin_Line

ChanR_Bin_Line::ChanR_Bin_Line(const ChanR_Bin& iChanR)
:	fChanR(iChanR),
	fEmpty(true),
	fState(eNormal)
	{}

size_t ChanR_Bin_Line::QRead(byte* oDest, size_t iCount)
	{
	byte* localDest = static_cast<byte*>(oDest);
	byte* localEnd = localDest + iCount;

	while (localDest < localEnd && fState != eSeen_LF)
		{
		if (fState == eReturn_X)
			{
			fEmpty = false;
			*localDest++ = fX;
			fState = eNormal;
			continue;
			}

		if (not sQRead(fX, fChanR))
			break;

		if (fState == eNormal)
			{
			if (fX == LF)
				fState = eSeen_LF;
			else
				fState = eReturn_X;
			}
		else
			{
			ZUnimplemented();
			}
		}

	return localDest - static_cast<byte*>(oDest);
	}

void ChanR_Bin_Line::Reset()
	{
	fState = eNormal;
	fEmpty = true;
	}

bool ChanR_Bin_Line::HitLF() const
	{ return fState == eSeen_LF; }

bool ChanR_Bin_Line::WasEmpty() const
	{ return fState == eSeen_LF && fEmpty; }

} // namespace MIME
} // namespace ZooLib
