// Copyright (c) 2008 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_HTTP_Requests_h__
#define __ZooLib_HTTP_Requests_h__ 1
#include "zconfig.h"

#include "zoolib/HTTP.h"
#include "zoolib/HTTP_Connect.h" // For Callable_Connect

namespace ZooLib {
namespace HTTP {

// =================================================================================================
#pragma mark - HTTP

// -----

bool sQRequest(ZP<ChannerRWClose_Bin>& ioConnection,
	ZP<Callable_Connect> iCallable_Connect,
	const string& iMethod, const string& iURL, const Map* iHeader,
	bool iConnectionClose,
	string* oURL, int32* oResponseCode, Map* oHeader, Data* oRawHeader);

// -----

ZP<ChannerRWClose_Bin> sPOST_Send(ZP<Callable_Connect> iCallable_Connect,
	const string& iMethod,
	const string& iURL, const Map* iHeader, const ChanR_Bin& iBody, ZQ<uint64> iBodyCountQ);

ZP<ChannerRWClose_Bin> sPOST_Receive(ZP<ChannerRWClose_Bin> iConn,
	int32* oResponseCode, Map* oHeader, Data* oRawHeader);

ZP<ChannerRWClose_Bin> sPOST(ZP<Callable_Connect> iCallable_Connect,
	const string& iURL, const Map* iHeader, const ChanR_Bin& iBody, ZQ<uint64> iBodyCountQ,
	int32* oResponseCode, Map* oHeader, Data* oRawHeader);

// -----

bool sQCONNECT(const ChanR_Bin& r, const ChanW_Bin& w,
	const std::string& iAddress, const Map* iHeader,
	int32* oResponseCode, Map* oHeader);

} // namespace HTTP
} // namespace ZooLib

#endif // __ZooLib_HTTP_Requests_h__
