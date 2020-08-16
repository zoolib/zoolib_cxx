// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Factory_Fallback_h__
#define __ZooLib_Factory_Fallback_h__ 1
#include "zconfig.h"

#include "zoolib/Factory.h"
#include "zoolib/ZMACRO_foreach.h"

#include <vector>

namespace ZooLib {

// =================================================================================================
#pragma mark - Factory_Fallback

template <class T>
class Factory_Fallback
:	public Factory<T>
	{
public:
	typedef T Result_t;

	Factory_Fallback()
		{}

	Factory_Fallback(const ZP<Factory_Fallback<T>>* iFactories, size_t iCount)
	:	fFactories(iFactories, iFactories + iCount)
		{}

	virtual ~Factory_Fallback()
		{}

// From Callable
	virtual ZQ<T> QCall()
		{
		foreachi (ii, fFactories)
			{
			if (ZQ<T> theQ = sQCall(*ii))
				return theQ;
			}
		return null;
		}

// Our protocol
	void Add(ZP<Factory_Fallback<T>> iFactory)
		{ fFactories.push_back(iFactory); }

	void Add(const ZP<Factory_Fallback<T>>* iFactories, size_t iCount)
		{
		if (iFactories && iCount)
			fFactories.insert(fFactories.end(), iFactories, iFactories + iCount);
		}

protected:
	std::vector<ZP<Factory_Fallback<T>>> fFactories;
	};

} // namespace ZooLib

#endif // __ZooLib_Factory_Fallback_h__
