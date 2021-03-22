// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Dataspace/ResultHandler.h"

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
#pragma mark - ResultHandler

ResultHandler::ResultHandler(const ZP<Callable_t>& iCallable)
:	fCallable(iCallable)
	{}

bool ResultHandler::QCall(
	const ZP<Counted>& iRegistration,
	int64 iChangeCount,
	const ZP<Result>& iResult,
	const ZP<ResultDeltas>& iResultDeltas)
	{
	if (iResult)
		{
		fResult = iResult;
		}
	else
		{
		ZAssert(iResultDeltas && fResult);
		fResult = sApplyDeltas(fResult, iResultDeltas);
		}
	sCall(fCallable, iRegistration, fResult);

	return true;
	}

} // namespace Dataspace
} // namespace ZooLib
