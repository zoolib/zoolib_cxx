// Copyright (c) 2014-2021 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Starter_ThreadLoop_h__
#define __ZooLib_Starter_ThreadLoop_h__ 1
#include "zconfig.h"

#include <string>

#include "zoolib/Starter.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - sStarter_ThreadLoop

ZP<Starter> sStarter_ThreadLoop(
	const ZP<Callable_Bookend>& iBookend_Loop,
	const ZP<Callable_Bookend>& iBookend_Startable);

ZP<Starter> sStarter_ThreadLoop(const std::string& iName);

ZP<Starter> sStarter_ThreadLoop();

} // namespace ZooLib

#endif // __ZooLib_Starter_ThreadLoop_h__
