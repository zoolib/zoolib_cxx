/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#ifndef __ZUtil_STL_set__
#define __ZUtil_STL_set__
#include "zconfig.h"

#include "zoolib/ZCompat_algorithm.h"

#include <set>

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_STL_set

namespace ZUtil_STL_set {

template <class T>
void sOr(const std::set<T>& iLHS, const std::set<T>& iRHS, std::set<T>& oResult)
	{
	set_union(iLHS.begin(), iLHS.end(),
		iRHS.begin(), iRHS.end(),
		inserter(oResult, oResult.end()));
	}

template <class T>
void sAnd(const std::set<T>& iLHS, const std::set<T>& iRHS, std::set<T>& oResult)
	{
	set_intersection(iLHS.begin(), iLHS.end(),
		iRHS.begin(), iRHS.end(),
		inserter(oResult, oResult.end()));
	}

template <class T>
void sMinus(const std::set<T>& iLHS, const std::set<T>& iRHS, std::set<T>& oResult)
	{
	set_difference(iLHS.begin(), iLHS.end(),
		iRHS.begin(), iRHS.end(),
		inserter(oResult, oResult.end()));
	}

template <class T>
void sXor(const std::set<T>& iLHS, const std::set<T>& iRHS, std::set<T>& oResult)
	{
	set_symmetric_difference(iLHS.begin(), iLHS.end(),
		iRHS.begin(), iRHS.end(),
		inserter(oResult, oResult.end()));
	}

template <class T>
bool sIncludes(const std::set<T>& iLHS, const std::set<T>& iRHS)
	{
	return includes(iLHS.begin(), iLHS.end(),
		iRHS.begin(), iRHS.end());
	}

} // namespace ZUtil_STL_set

NAMESPACE_ZOOLIB_END

#endif // __ZUtil_STL_set__
