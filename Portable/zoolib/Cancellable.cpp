// Copyright (c) 2015 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Cancellable.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Cancellable_Callable

class Cancellable_Callable
:	public Cancellable
	{
public:
	Cancellable_Callable(const ZP<Callable_void>& iCallable)
	:	fCallable(iCallable)
		{}

	virtual void Cancel()
		{ sCall(fCallable); }

	const ZP<Callable_void> fCallable;
	};

// =================================================================================================
#pragma mark - sCancellable

ZP<Cancellable> sCancellable(const ZP<Callable_void>& iCallable)
	{
	if (iCallable)
		return new Cancellable_Callable(iCallable);
	return null;
	}

} // namespace ZooLib
