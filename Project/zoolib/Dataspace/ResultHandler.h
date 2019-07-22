// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Dataspace_ResultHandler_h__
#define __ZooLib_Dataspace_ResultHandler_h__ 1
#include "zconfig.h"

#include "zoolib/Dataspace/RelsWatcher.h"

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
#pragma mark - ResultHandler

class ResultHandler : public RelsWatcher::Callable_Changed
	{
public:
	typedef Callable<void(const ZP<Counted>& iRegistration, const ZP<Result>& iResult)>
		Callable_t;

	ResultHandler(const ZP<Callable_t>& iCallable);

	virtual bool QCall(
		const ZP<Counted>& iRegistration,
		int64 iChangeCount,
		const ZP<Result>& iResult,
		const ZP<ResultDeltas>& iResultDeltas);

	const ZP<Callable_t> fCallable;
	ZP<Result> fResult;
	};

} // namespace Dataspace
} // namespace ZooLib

#endif // __ZooLib_Dataspace_ResultHandler_h__
