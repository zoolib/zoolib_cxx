// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Connection_h__
#define __ZooLib_Connection_h__ 1
#include "zconfig.h"

#include "zoolib/Channer_Bin.h"
#include "zoolib/Factory.h"

namespace ZooLib {

typedef ChannerRWCon<byte> ChannerRWCon_Bin;
typedef Factory<ZP<ChannerRWCon_Bin>> Factory_ChannerRWCon_Bin;

typedef ChannerRW<byte> ChannerRW_Bin;
typedef Factory<ZP<ChannerRW_Bin>> Factory_ChannerRW_Bin;

} // namespace ZooLib

#endif // __ZooLib_Connection_h__
