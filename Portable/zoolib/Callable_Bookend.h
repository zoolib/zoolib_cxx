// Copyright (c) 2021 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Callable_Bookend_h__
#define __ZooLib_Callable_Bookend_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"

namespace ZooLib {

typedef Callable<void(const ZP<Callable_Void>&)> Callable_Bookend;

// =================================================================================================
#pragma mark -

typedef Callable<ZP<Callable_Void>(const ZP<Callable_Void>&)> Callable_BookendMaker;

ZP<Callable_BookendMaker> sBookendMaker_Nested(
	ZP<Callable_BookendMaker> iFirst,
	ZP<Callable_BookendMaker> iNext);

} // namespace ZooLib

#endif // __ZooLib_Callable_Bool_h__
