/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------------------------- */

#ifndef __ZooLib_HTTP_Content_h__
#define __ZooLib_HTTP_Content_h__ 1
#include "zconfig.h"

#include "zoolib/Channer_Bin.h"
#include "zoolib/HTTP.h"

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

ZRef<ChannerR_Bin> sMakeContentChanner(const Map& iHeader, ZRef<ChannerR_Bin> iChannerR);

ZRef<ChannerR_Bin> sMakeContentChanner(
	const std::string& iMethod, int responseCode,
	const Map& iHeader, const ZRef<ChannerR_Bin>& iChannerR);

} // namespace HTTP
} // namespace ZooLib

#endif // __ZooLib_HTTP_Requests_h__
