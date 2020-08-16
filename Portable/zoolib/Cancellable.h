// Copyright (c) 2015 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Cancellable_h__
#define __ZooLib_Cancellable_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Cancellable

class Cancellable
:	public Counted
	{
public:
	virtual void Cancel() = 0;
	};

// =================================================================================================
#pragma mark - sCancellable

ZP<Cancellable> sCancellable(const ZP<Callable_Void>& iCallable);

} // namespace ZooLib

#endif // __ZooLib_Cancellable_h__
