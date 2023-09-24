// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_StartOnNewThread_h__
#define __ZooLib_StartOnNewThread_h__ 1
#include "zconfig.h"

#include "zoolib/Startable.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - sStartOnNewThread

void sStartOnNewThread(const ZP<Startable>& iStartable);

void sStartOnNewThread_ProcessIsAboutToExit();

} // namespace ZooLib

#endif // __ZooLib_StartOnNewThread_h__
