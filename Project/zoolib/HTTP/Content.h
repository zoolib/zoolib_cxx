// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_HTTP_Content_h__
#define __ZooLib_HTTP_Content_h__ 1
#include "zconfig.h"

#include "zoolib/Channer_Bin.h"

#include "zoolib/HTTP/HTTP.h"

namespace ZooLib {
namespace HTTP {

// =================================================================================================
#pragma mark - ChanR_Bin_Chunked

class ChanR_Bin_Chunked
:	public ChanR_Bin
	{
public:
	ChanR_Bin_Chunked(const ChanR_Bin& iChanR);
	virtual ~ChanR_Bin_Chunked();

// From ChanR_Bin
	virtual size_t Read(byte* oDest, size_t iCount);
	virtual size_t Readable();

private:
	const ChanR_Bin& fChanR;
	uint64 fChunkSize;
	bool fHitEnd;
	};

// =================================================================================================
#pragma mark - ChanW_Bin_Chunked

class ChanW_Bin_Chunked
:	public ChanW_Bin
	{
public:
	ChanW_Bin_Chunked(size_t iBufferSize, const ChanW_Bin& iChanW);
	ChanW_Bin_Chunked(const ChanW_Bin& iChanW);
	virtual ~ChanW_Bin_Chunked();

// From ChanW_Bin
	virtual size_t Write(const byte* iSource, size_t iCount);
	virtual void Flush();

private:
	void pFlush();

	const ChanW_Bin& fChanW;
	std::vector<byte> fBuffer;
	size_t fBufferUsed;
	};

// =================================================================================================
#pragma mark - sMakeContentChanner

ZP<ChannerR_Bin> sMakeContentChanner(const Map& iHeader, ZP<ChannerR_Bin> iChannerR);

ZP<ChannerR_Bin> sMakeContentChanner(
	const std::string& iMethod, int responseCode,
	const Map& iHeader, const ZP<ChannerR_Bin>& iChannerR);

} // namespace HTTP
} // namespace ZooLib

#endif // __ZooLib_HTTP_Requests_h__
