// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Dataspace_RelsWatcher_h__
#define __ZooLib_Dataspace_RelsWatcher_h__ 1
#include "zconfig.h"

#include "zoolib/StdInt.h"

#include "zoolib/Dataspace/Types.h"

// =================================================================================================
#pragma mark -

namespace ZooLib {
namespace Dataspace {
namespace RelsWatcher {

typedef Callable<void(
		const ZP<Counted>& iRegistration,
		int64 iChangeCount,
		const ZP<Result>& iResult,
		const ZP<ResultDeltas>& iResultDeltas)>
	Callable_Changed;

typedef Callable<ZP<Counted>(
		const ZP<Callable_Changed>& iCallable_Changed,
		const ZP<Expr_Rel>& iRel)>
	Callable_Register;

} // namespace RelsWatcher
} // namespace Dataspace
} // namespace ZooLib

#endif // __ZooLib_Dataspace_RelsWatcher_h__
