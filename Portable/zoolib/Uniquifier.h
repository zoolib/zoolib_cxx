// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Uniquifier_h__
#define __ZooLib_Uniquifier_h__ 1
#include "zconfig.h"

#include <set>

namespace ZooLib {

// =================================================================================================
#pragma mark - Uniquifier

template <class Type_p, class Compare_p = typename std::set<Type_p>::key_compare>
class Uniquifier
	{
public:
	const Type_p& Get(const Type_p& iValue)
		{
		typename std::set<Type_p, Compare_p>::iterator iter = fSet.lower_bound(iValue);
		if (iter != fSet.end()
			&& not fSet.key_comp()(*iter, iValue)
			&& not fSet.key_comp()(iValue, *iter))
			{
			return *iter;
			}
		return *fSet.insert(iter, iValue);
		}

private:
	std::set<Type_p, Compare_p> fSet;
	};

} // namespace ZooLib

#endif // __ZooLib_Uniquifier_h__
