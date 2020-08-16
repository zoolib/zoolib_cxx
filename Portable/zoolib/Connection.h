// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Connection_h__
#define __ZooLib_Connection_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"
#include "zoolib/Channer_Bin.h"

namespace ZooLib {

// ---

typedef ChannerConnection<byte> ChannerConnection_Bin;

typedef Callable<ZP<ChannerConnection_Bin>()> Factory_ChannerConnection_Bin;

// Workaround for now.
typedef ChannerConnection_Bin ChannerRWClose_Bin;
typedef Factory_ChannerConnection_Bin Factory_ChannerRWClose_Bin;

// ---

typedef ChannerRW<byte> ChannerRW_Bin;

typedef Callable<ZP<ChannerRW_Bin>()> Factory_ChannerRW_Bin;

} // namespace ZooLib

#endif // __ZooLib_Connection_h__
