// Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#include "zoolib/ZStrimR_Rewind.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZStream.h"
#include "zoolib/ZStrim_Stream.h"

#define kDebug_StrimR_Rewind 1

namespace ZooLib {

// =================================================================================================
#pragma mark - ZStrimR_Rewind

/**
\class ZStrimR_Rewind
\ingroup strim
*/

ZStrimR_Rewind::ZStrimR_Rewind(const ZStrimR& iStrimSource, const ZStreamRWPos& iBuffer)
:	fStrimSource(iStrimSource),
	fBuffer(iBuffer)
	{}

ZStrimR_Rewind::~ZStrimR_Rewind()
	{}

void ZStrimR_Rewind::Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount)
	{
	UTF32* localDest = oDest;
	size_t countRemaining = iCount;
	while (countRemaining)
		{
		size_t countRead;
		ZStrimR_StreamUTF8(fBuffer).Read(localDest, countRemaining, &countRead);
		if (countRead == 0)
			{
			size_t priorPosition = fBuffer.GetPosition();
			// Our buffer's reached the end. Try topping it up.
			uint64 countCopied;
			ZStrimW_StreamUTF8(fBuffer).CopyFrom(fStrimSource, countRemaining, &countCopied, nullptr);
			if (countCopied == 0)
				break;
			fBuffer.SetPosition(priorPosition);
			}
		else
			{
			localDest += countRead;
			countRemaining -= countRead;
			}
		}
	if (oCount)
		*oCount = localDest - oDest;
	}

void ZStrimR_Rewind::Rewind()
	{ fBuffer.SetPosition(0); }

uint64 ZStrimR_Rewind::GetMark()
	{ return fBuffer.GetPosition(); }

void ZStrimR_Rewind::SetMark(uint64 iMark)
	{
	ZAssertStop(kDebug_StrimR_Rewind, iMark <= fBuffer.GetSize());
	fBuffer.SetPosition(iMark);
	}

} // namespace ZooLib
