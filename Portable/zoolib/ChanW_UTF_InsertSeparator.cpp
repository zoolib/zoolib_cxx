// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ChanW_UTF_InsertSeparator.h"

#include "zoolib/Compat_algorithm.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanW_UTF_InsertSeparator

ChanW_UTF_InsertSeparator::ChanW_UTF_InsertSeparator(
	size_t iSpacing, const string8& iSeparator, const ChanW_UTF& iChanW)
:	fChanW(iChanW)
,	fCount(0)
	{
	if (iSpacing)
		fSpacings[iSpacing] = iSeparator;
	}

ChanW_UTF_InsertSeparator::ChanW_UTF_InsertSeparator(
	const Spacings& iSpacings, const ChanW_UTF& iChanW)
:	fChanW(iChanW)
,	fSpacings(iSpacings)
,	fCount(0)
	{}

size_t ChanW_UTF_InsertSeparator::Write(const UTF32* iSource, size_t iCountCU)
	{
	const UTF32* localSource = iSource;
	size_t countRemaining = iCountCU;
	while (countRemaining)
		{
		size_t countToWrite = countRemaining;
		
		if (not fSpacings.empty())
			{
			// Find the largest spacing that divides fCount exactly (if any).
			if (fCount)
				{
				// This is not the very first write.
				for (Spacings::const_reverse_iterator
					riter = fSpacings.rbegin(), end = fSpacings.rend();
					riter != end; ++riter)
					{
					if (riter->first)
						{
						if (0 == (fCount % riter->first))
							{
							sEWrite(fChanW, riter->second);
							break;
							}
						}
					}
				}
			
			const uint64 newEnd = fCount + countToWrite;

			for (Spacings::const_iterator iter = fSpacings.begin(), end = fSpacings.end();
				iter != end; ++iter)
				{
				if (iter->first)
					{
					const size_t tickSize = iter->first;
					const uint64 nextTick = (fCount / tickSize) * tickSize + tickSize;
					if (nextTick <= newEnd)
						countToWrite = std::min(countToWrite, size_t(nextTick - fCount));
					}
				}
			}

		const size_t countWritten = sWrite(fChanW, localSource, countToWrite);
		if (countWritten == 0)
			break;
		
		countRemaining -= countWritten;
		localSource += countWritten;
		fCount += countWritten;
		}

	return localSource - iSource;
	}

} // namespace ZooLib
