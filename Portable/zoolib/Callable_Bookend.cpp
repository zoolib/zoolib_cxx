// Copyright (c) 2021 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Callable_Bookend.h"
#include "zoolib/Callable_Bind.h"
#include "zoolib/Callable_Function.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -

static void spNester(const ZP<Callable_Bookend>& iBookend0, const ZP<Callable_Bookend>& iBookend1,
	const ZP<Callable_Void>& iCallable_Void)
	{ sCall(iBookend0, sBindR(iBookend1, iCallable_Void)); }

ZP<Callable_Bookend> sNest(ZP<Callable_Bookend> iBookend0, ZP<Callable_Bookend> iBookend1)
	{
	if (not iBookend0)
		return iBookend1;

	if (not iBookend1)
		return iBookend0;

	return sBindL(iBookend0, iBookend1, sCallable(spNester));
	}

} // namespace ZooLib
