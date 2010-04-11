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

#include "zoolib/ZMIME.h"
#include "zoolib/ZDebug.h"

#include <ctype.h>

static const char CR = '\r';
static const char LF = '\n';

#define kDebug_MIME 2

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark * ZMIME

// Character classification.
bool ZMIME::sIs_LWS(char iChar)
	{
	return iChar == ' ' || iChar == '\t';
	}

bool ZMIME::sReadFieldName(const ZStreamR& iStream, std::string* oName, std::string* oNameExact)
	{
	if (oName)
		oName->resize(0);
	if (oNameExact)
		oNameExact->resize(0);

	bool gotAny = false;

	for (;;)
		{
		char readChar;
		if (!iStream.ReadChar(readChar))
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
#pragma mark -
#pragma mark * ZMIME::StreamR_Header

ZMIME::StreamR_Header::StreamR_Header(const ZStreamR& iStream)
:	fStreamR(iStream),
	fState(eNormal)
//#warning "check this"
// Should we *start* in eSeen_LF, so an empty header ends up in eSeen_LF_LF? Or should
// we have some other initial state?
	{}

void ZMIME::StreamR_Header::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	if (oCountRead)
		*oCountRead = 0;

	uint8* localDest = static_cast<uint8*>(oDest);
	uint8* localEnd = localDest + iCount;

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

		if (!fStreamR.ReadChar(fX))
			break;

		if (fX == CR)
			continue;

		switch (fState)
			{
			case eNormal:
				{
				if (fX == LF)
					fState = eSeen_LF;
				else
					*localDest++ = fX;
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

	if (oCountRead)
		*oCountRead = localDest - static_cast<uint8*>(oDest);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZMIME::StreamR_Line

ZMIME::StreamR_Line::StreamR_Line(const ZStreamR& iStream)
:	fStreamR(iStream),
	fState(eNormal),
	fEmpty(true)
	{}

void ZMIME::StreamR_Line::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	uint8* localDest = static_cast<uint8*>(oDest);
	uint8* localEnd = localDest + iCount;

	while (localDest < localEnd && fState != eSeen_LF)
		{
		if (fState == eReturn_X)
			{
			fEmpty = false;
			*localDest++ = fX;
			fState = eNormal;
			continue;
			}

		if (!fStreamR.ReadChar(fX))
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

	if (oCountRead)
		*oCountRead = localDest - static_cast<uint8*>(oDest);
	}

bool ZMIME::StreamR_Line::HitLF() const
	{ return fState == eSeen_LF; }

bool ZMIME::StreamR_Line::WasEmpty() const
	{ return fState == eSeen_LF && fEmpty; }

NAMESPACE_ZOOLIB_END
