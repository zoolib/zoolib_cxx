/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#ifndef __ZAccumulator_T_h__
#define __ZAccumulator_T_h__ 1
#include "zconfig.h"
#include "zoolib/ZStdInt.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZAccumulator_T

// AG 2000-02-17. If we assume that unioning a rectangle into a region requires a linear search
// through the target region, then a region built out of n rectangles will take (n^2)/2 operations.
// Unioning two regions together also requires a linear search, but not for each rectangle in the
// source region -- it's basically a merge sort. So we union rects 0 & 1 together, then 2 & 3, and
// then merge the results. Then we merge 4 & 5, then 6 & 7 and merge those two results, and merge
// that result with the earlier one, and so forth. fStack holds regions from fully merged branches
// of the tree, the number of bit flips in the fCount indicates how much of that stack should be
// merged at each step. It's quite hard to explain in prose, ask me if you're curious. The upshot
// is that we go from O(n^2) operation to O(n log2 n). Which is very noticeable when working with
// hundreds of rectangles.

// T is the type on which we're operating.
// C is a combiner functor
// S is a stack-like container of T (vector, list or deque are all fine).

template <typename T, typename C, typename S>
class ZAccumulator_T
	{
	static inline uint32 sHammingWeight(uint32 n)
		{
		// See <https://stackoverflow.com/revisions/9830282/2>
		// for the nicest explanation I've seen of this.
		n = (n & 0x55555555) + ((n & 0xAAAAAAAA) >> 1);
		n = (n & 0x33333333) + ((n & 0xCCCCCCCC) >> 2);
		n = (n & 0x0F0F0F0F) + ((n & 0xF0F0F0F0) >> 4);
		n = (n & 0x00FF00FF) + ((n & 0xFF00FF00) >> 8);
		n = (n & 0x0000FFFF) + ((n & 0xFFFF0000) >> 16);
		return n;
		}

public:
	ZAccumulator_T()
	:	fCount(0)
		{}

	ZAccumulator_T(const T& iT)
	:	fStack(1, iT)
	,	fCount(1)
		{}

	void Include(const T& iT)
		{
		fStack.push_back(iT);

		uint32 changedBitsCount = sHammingWeight((fCount + 1) ^ fCount);
		++fCount;
		while (--changedBitsCount)
			{
			T tail = fStack.back();
			fStack.pop_back();
			fCombiner(fStack.back(), tail);
			}
		}

	const T Get() const
		{
		T result;
		for (typename S::const_iterator ii = fStack.begin(); ii != fStack.end(); ++ii)
			fCombiner(result, *ii);
		return result;
		}

private:
	C fCombiner;
	S fStack;
	uint32 fCount;
	};

// =================================================================================================

} // namespace ZooLib

#endif // __ZAccumulator_T_h__
