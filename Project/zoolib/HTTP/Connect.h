// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_HTTP_Connect_h__
#define __ZooLib_HTTP_Connect_h__ 1
#include "zconfig.h"

#include "zoolib/Connection.h"

namespace ZooLib {
namespace HTTP {

// =================================================================================================
#pragma mark - HTTP

typedef Callable<ZP<ChannerRWClose_Bin>(const std::string& iHost, uint16 iPort, bool iUseSSL)>
	Callable_Connect;

ZP<ChannerRWClose_Bin> sConnect(const std::string& iHost, uint16 iPort, bool iUseSSL);

} // namespace HTTP
} // namespace ZooLib

#endif // __ZooLib_HTTP_Connect_h__
