/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#include "zoolib/ChanW_UTF_InsertSeparator.h"
#include "zoolib/Compat_algorithm.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ChanW_UTF_InsertSeparator

ChanW_UTF_InsertSeparator::ChanW_UTF_InsertSeparator(
	size_t iSpacing, const string8& iSeparator, const ChanW_UTF& iStrimSink)
:	fStrimSink(iStrimSink)
,	fCount(0)
	{
	if (iSpacing)
		fSpacings[iSpacing] = iSeparator;
	}

ChanW_UTF_InsertSeparator::ChanW_UTF_InsertSeparator(
	const Spacings& iSpacings, const ChanW_UTF& iStrimSink)
:	fStrimSink(iStrimSink)
,	fSpacings(iSpacings)
,	fCount(0)
	{}

size_t ChanW_UTF_InsertSeparator::QWrite(const UTF32* iSource, size_t iCountCU)
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
							sEWrite(fStrimSink, riter->second);
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

		const size_t countWritten = sQWrite(fStrimSink, localSource, countToWrite);
		if (countWritten == 0)
			break;
		
		countRemaining -= countWritten;
		localSource += countWritten;
		fCount += countWritten;
		}

	return localSource - iSource;
	}

} // namespace ZooLib
