// Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#include "zoolib/HTTP/MIME.h"

#include "zoolib/ZDebug.h"

#include <ctype.h>

static const char CR = '\r';
static const char LF = '\n';

#define kDebug_MIME 2

namespace ZooLib {
namespace MIME {

// =================================================================================================
#pragma mark - ZMIME

// Character classification.
bool sIs_LWS(char iChar)
	{
	return iChar == ' ' || iChar == '\t';
	}

// =================================================================================================
#pragma mark - ChanR_Bin_Header

ChanR_Bin_Header::ChanR_Bin_Header(const ChanR_Bin& iChanR)
:	fChanR(iChanR),
	fState(eInitial)
	{}

size_t ChanR_Bin_Header::Read(byte* oDest, size_t iCount)
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

		if (NotQ<UTF32> theCPQ = sQRead(fChanR))
			{
			break;
			}
		else
			{
			fX = *theCPQ;

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
		}

	return localDest - static_cast<byte*>(oDest);
	}

void ChanR_Bin_Header::Reset()
	{ fState = eInitial; }

// =================================================================================================
#pragma mark - ChanR_Bin_Line

ChanR_Bin_Line::ChanR_Bin_Line(const ChanR_Bin& iChanR)
:	fChanR(iChanR),
	fEmpty(true),
	fState(eNormal)
	{}

size_t ChanR_Bin_Line::Read(byte* oDest, size_t iCount)
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

		if (NotQ<UTF32> theCPQ = sQRead(fChanR))
			break;
		else
			fX = *theCPQ;

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
