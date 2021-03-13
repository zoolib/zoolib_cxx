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
	typedef std::set<Type_p, Compare_p> Set_t;

	const Type_p& Get(const Type_p& iValue) { return *fSet.insert(iValue).first; }
	Set_t& GetStorage() { return fSet; }

private:
	Set_t fSet;
	};

} // namespace ZooLib

#endif // __ZooLib_Uniquifier_h__
