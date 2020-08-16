// Copyright (c) 2008 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_PairwiseCombiner_T_h__
#define __ZooLib_PairwiseCombiner_T_h__ 1
#include "zconfig.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - PairwiseCombiner_T

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
class PairwiseCombiner_T
	{
public:
	PairwiseCombiner_T()
	:	fCount(0)
		{}

	PairwiseCombiner_T(const T& iT)
	:	fStack(1, iT)
	,	fCount(1)
		{}

	void Include(const T& iT)
		{
		fStack.push_back(iT);

		unsigned changedBits = (fCount + 1) ^ fCount;
		++fCount;
		while ((changedBits = (changedBits >> 1)))
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
	unsigned fCount;
	};

// =================================================================================================

} // namespace ZooLib

#endif // __ZooLib_PairwiseCombiner_T_h__
