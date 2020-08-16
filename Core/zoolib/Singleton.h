// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Singleton_h__
#define __ZooLib_Singleton_h__ 1
#include "zconfig.h"

#include "zoolib/Atomic.h"
#include "zoolib/Deleter.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - sSingleton

template <class Type_p, class Tag_p>
Type_p& sSingleton()
	{
	static std::atomic<Type_p*> spType_p;
	if (not spType_p)
		{
		Type_p* newValue = new Type_p();
		if (not sAtomic_CAS<Type_p*>(&spType_p, nullptr, newValue))
			{
			delete newValue;
			}
		else
			{
			static Deleter<Type_p> deleter(*(Type_p**)&spType_p);
			}
		}
	return *spType_p;
	}

template <class Type_p>
Type_p& sSingleton()
	{ return sSingleton<Type_p,Type_p>(); }

} // namespace ZooLib

#endif // __ZooLib_Singleton_h___
