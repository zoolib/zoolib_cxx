// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Dataspace_Melange_h__
#define __ZooLib_Dataspace_Melange_h__ 1
#include "zconfig.h"

#include "zoolib/Factory.h"
#include "zoolib/Multi.h"
#include "zoolib/Starter.h"

#include "zoolib/Dataspace/RelsWatcher.h"
#include "zoolib/Dataspace/Daton.h"

// =================================================================================================
#pragma mark -

namespace ZooLib {
namespace Dataspace {

using RelsWatcher::Callable_Register;

typedef Multi3<
	ZP<Callable_Register>,
	ZP<Callable_DatonUpdate>,
	ZP<Starter>
	> Melange_t;

typedef Factory<Melange_t> Factory_Melange;

} // namespace Dataspace
} // namespace ZooLib

#endif // __ZooLib_Dataspace_Melange_h__
