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

#include "zoolib/ZStrim_Limited.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZMemory.h"
#include "zoolib/ZUnicode.h"

using std::max;
using std::min;

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimR_Limited

ZStrimR_Limited::ZStrimR_Limited(size_t iLimitCP, const ZStrimR& iSource)
:	fRemainingCP(iLimitCP),
	fSource(iSource)
	{}

void ZStrimR_Limited::Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount)
	{
	size_t countRead;
	fSource.Read(oDest, min(fRemainingCP, iCount), &countRead);
	fRemainingCP -= countRead;
	if (oCount)
		*oCount = countRead;
	}

void ZStrimR_Limited::Imp_CopyToDispatch(const ZStrimW& iStrimW, size_t iCountCP,
	size_t* oCountCPRead, size_t* oCountCPWritten)
	{
	size_t countCPRead;
	fSource.CopyTo(iStrimW, min(iCountCP, fRemainingCP), &countCPRead, oCountCPWritten);
	fRemainingCP -= countCPRead;
	if (oCountCPRead)
		*oCountCPRead = countCPRead;
	}

void ZStrimR_Limited::Imp_CopyTo(const ZStrimW& iStrimW, size_t iCountCP,
	size_t* oCountCPRead, size_t* oCountCPWritten)
	{
	size_t countCPRead;
	fSource.CopyTo(iStrimW, min(iCountCP, fRemainingCP), &countCPRead, oCountCPWritten);
	fRemainingCP -= countCPRead;
	if (oCountCPRead)
		*oCountCPRead = countCPRead;
	}

void ZStrimR_Limited::Imp_Skip(size_t iCountCP, size_t* oCountCPSkipped)
	{
	size_t countCPSkipped;
	fSource.Skip(min(iCountCP, fRemainingCP), &countCPSkipped);
	fRemainingCP -= countCPSkipped;
	if (oCountCPSkipped)
		*oCountCPSkipped = countCPSkipped;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimW_Limited

ZStrimW_Limited::ZStrimW_Limited(size_t iLimitCP, const ZStrimW& iSink)
:	fRemainingCP(iLimitCP),
	fSink(iSink)
	{}

void ZStrimW_Limited::Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU)
	{
	size_t countCUWritten;
	fSink.Write(iSource, min(fRemainingCP, iCountCU), &countCUWritten);
	fRemainingCP -= ZUnicode::sCUToCP(iSource, countCUWritten);
	if (oCountCU)
		*oCountCU = countCUWritten;
	}

void ZStrimW_Limited::Imp_Flush()
	{ fSink.Flush(); }

} // namespace ZooLib
