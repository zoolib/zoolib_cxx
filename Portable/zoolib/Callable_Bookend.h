// Copyright (c) 2021 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Callable_Bookend_h__
#define __ZooLib_Callable_Bookend_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -

typedef Callable<void(const ZP<Callable_void>&)> Callable_Bookend;

ZP<Callable_Bookend> sNest(ZP<Callable_Bookend> iBookend0, ZP<Callable_Bookend> iBookend1);

} // namespace ZooLib

#endif // __ZooLib_Callable_Bool_h__
