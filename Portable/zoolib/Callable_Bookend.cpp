// Copyright (c) 2021 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Callable_Bookend.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -

class Callable_BookendMaker_Nested
:	public Callable_BookendMaker
	{
public:
	Callable_BookendMaker_Nested(
		ZP<Callable_BookendMaker> iFirst,
		ZP<Callable_BookendMaker> iNext)
	:	fFirst(iFirst)
	,	fNext(iNext)
		{}

	ZP<Callable_Void> QCall(const ZP<Callable_Void>& iCallable) override
		{
		ZP<Callable_Void> result = iCallable;
		if (result)
			result = sCall(fFirst, result);
		if (result)
			result = sCall(fNext, result);
		return result;
		}

	const ZP<Callable_BookendMaker> fFirst;
	const ZP<Callable_BookendMaker> fNext;
	};

ZP<Callable_BookendMaker> sBookendMaker_Nested(
	ZP<Callable_BookendMaker> iFirst,
	ZP<Callable_BookendMaker> iNext)
	{
	if (iFirst)
		{
		if (iNext)
			return new Callable_BookendMaker_Nested(iFirst, iNext);
		return iFirst;
		}
	return iNext;
	}

} // namespace ZooLib
