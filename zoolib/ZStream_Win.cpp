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

#include "zoolib/ZStream_Win.h"

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZCompat_algorithm.h" // For lower_bound, min
#include "zoolib/ZDebug.h"
#include "zoolib/ZMemory.h" // For ZMemCopy
#include "zoolib/ZStream_Memory.h" // For ZStreamRPos_Memory
#include "zoolib/ZUnicode.h"
#include "zoolib/ZUtil_Win.h"

using std::min;
using std::runtime_error;
using std::string;
using std::vector;

namespace ZooLib {

// =================================================================================================
// MARK: - ZStreamRPos_Win_MultiResource

static HRSRC spFindResource(HMODULE iHMODULE, const string& iName, const string& iType)
	{
	if (ZUtil_Win::sUseWAPI())
		{
		return ::FindResourceW(iHMODULE,
			ZUnicode::sAsUTF16(iName).c_str(), ZUnicode::sAsUTF16(iType).c_str());
		}
	else
		{
		return ::FindResourceA(iHMODULE, iName.c_str(), iType.c_str());
		}
	}

static string spReadZeroTerminatedString(const ZStreamR& iStream)
	{
	string theString;
	while (true)
		{
		char theChar = iStream.ReadInt8();
		if (theChar == 0)
			break;
		theString += theChar;
		}
	return theString;
	}

ZStreamRPos_Win_MultiResource::ZStreamRPos_Win_MultiResource
	(HMODULE iHMODULE, const string& iType, const string& iName)
	{
	ZAssertStop(2, iHMODULE);
	fHMODULE = iHMODULE;
	fHGLOBAL_Current = nullptr;
	fLPVOID_Current = nullptr;
	fPosition = 0;

	// Load the descriptor resource
	if (HRSRC descHRSRC = spFindResource(fHMODULE, iName, iType))
		{
		if (HGLOBAL descHGLOBAL = ::LoadResource(fHMODULE, descHRSRC))
			{
			try
				{
				LPVOID descAddress = ::LockResource(descHGLOBAL);
				ZStreamRPos_Memory theSIM(descAddress, SizeofResource(fHMODULE, descHRSRC));

				size_t accumulatedSize = 0;
				size_t resourceCount = theSIM.ReadUInt16();
				string resourceType = spReadZeroTerminatedString(theSIM);
				for (size_t x = 0; x < resourceCount; ++x)
					{
					string resourceName = spReadZeroTerminatedString(theSIM);
					if (HRSRC currentHRSRC = spFindResource(fHMODULE, resourceName, resourceType))
						{
						fVector_HRSRC.push_back(currentHRSRC);
						accumulatedSize += ::SizeofResource(fHMODULE, currentHRSRC);
						fVector_Ends.push_back(accumulatedSize);
						}
					else
						{
						throw runtime_error
							("ZStreamRPos_Win_MultiResource, couldn't load resource");
						}
					}
				}
			catch (...)
				{
				::FreeResource(descHGLOBAL);
				throw;
				}
			::FreeResource(descHGLOBAL);
			}
		}
	fIndex = fVector_Ends.size();
	}

ZStreamRPos_Win_MultiResource::~ZStreamRPos_Win_MultiResource()
	{
	if (fHGLOBAL_Current)
		::FreeResource(fHGLOBAL_Current);
	}

void ZStreamRPos_Win_MultiResource::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	char* localDest = reinterpret_cast<char*>(oDest);
	while (iCount)
		{
		vector<size_t>::iterator theIter =
			upper_bound(fVector_Ends.begin(), fVector_Ends.end(), fPosition);
		if (theIter == fVector_Ends.end())
			break;

		size_t theIndex = theIter - fVector_Ends.begin();
		if (fIndex != theIndex)
			{
			fIndex = theIndex;
			if (fHGLOBAL_Current)
				::FreeResource(fHGLOBAL_Current);
			fHGLOBAL_Current = ::LoadResource(fHMODULE, fVector_HRSRC[fIndex]);
			fLPVOID_Current = ::LockResource(fHGLOBAL_Current);
			if (fIndex == 0)
				fBegin = 0;
			else
				fBegin = fVector_Ends[fIndex - 1];
			fEnd = fVector_Ends[fIndex];
			}

		size_t countToMove = ZStream::sClampedSize(iCount, fEnd, fPosition);
		if (countToMove == 0)
			break;

		ZMemCopy(localDest,
			reinterpret_cast<char*>(fLPVOID_Current) + fPosition - fBegin, countToMove);

		fPosition += countToMove;
		localDest += countToMove;
		iCount -= countToMove;
		}
	if (oCountRead)
		*oCountRead = localDest - reinterpret_cast<char*>(oDest);
	}

void ZStreamRPos_Win_MultiResource::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	uint64 realSkip = ZStream::sClampedCount(iCount, fVector_Ends.back(), fPosition);
	fPosition += realSkip;
	if (oCountSkipped)
		*oCountSkipped = realSkip;
	}

uint64 ZStreamRPos_Win_MultiResource::Imp_GetPosition()
	{ return fPosition; }

void ZStreamRPos_Win_MultiResource::Imp_SetPosition(uint64 iPosition)
	{ fPosition = iPosition; }

uint64 ZStreamRPos_Win_MultiResource::Imp_GetSize()
	{ return fVector_Ends.back(); }

// =================================================================================================
// MARK: - ZStreamerRPos_Win_MultiResource

ZStreamerRPos_Win_MultiResource::ZStreamerRPos_Win_MultiResource
	(HMODULE iHMODULE, const string& iType, const string& iName)
:	fStream(iHMODULE, iType, iName)
	{}

ZStreamerRPos_Win_MultiResource::~ZStreamerRPos_Win_MultiResource()
	{}

const ZStreamRPos& ZStreamerRPos_Win_MultiResource::GetStreamRPos()
	{ return fStream; }

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)
