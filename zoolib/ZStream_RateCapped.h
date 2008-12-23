/* -------------------------------------------------------------------------------------------------
Copyright (c) 2006 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZStream_RateCapped__
#define __ZStream_RateCapped__ 1
#include "zconfig.h"

#include "zoolib/ZStream_Filter.h"
#include "zoolib/ZStreamer.h"

#include "zoolib/ZTime.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * RateLimiter

class RateLimiter
	{
public:
	RateLimiter(double iRate, size_t iQuantum);
	
	size_t GetCount(size_t iLastCount, size_t iCount);

private:
	double fRate;
	size_t fQuantum;

	ZTime fLastTime;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_RateCapped

class ZStreamR_RateCapped : public ZStreamR_Filter
	{
public:
	ZStreamR_RateCapped(double iRate, size_t iQuantum, const ZStreamR& iStreamReal);

	~ZStreamR_RateCapped();

// From ZStreamR via ZStreamR_Filter
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);

protected:
	RateLimiter fLimiter;
	size_t fLastCount;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerR_RateCapped

class ZStreamerR_RateCapped : public ZStreamerR
	{
public:
	ZStreamerR_RateCapped(double iRate, size_t iQuantum, ZRef<ZStreamerR> iStreamerReal);

	virtual ~ZStreamerR_RateCapped();

// From ZStreamerR
	virtual const ZStreamR& GetStreamR();

protected:
	ZRef<ZStreamerR> fStreamerReal;
	ZStreamR_RateCapped fStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamU_RateCapped

class ZStreamU_RateCapped : public ZStreamU_Filter
	{
public:
	ZStreamU_RateCapped(double iRate, size_t iQuantum, const ZStreamU& iStreamReal);

	~ZStreamU_RateCapped();

// From ZStreamR via ZStreamU_Filter
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);

protected:
	RateLimiter fLimiter;
	size_t fLastCount;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerU_RateCapped

class ZStreamerU_RateCapped : public ZStreamerU
	{
public:
	ZStreamerU_RateCapped(double iRate, size_t iQuantum, ZRef<ZStreamerU> iStreamerReal);

	virtual ~ZStreamerU_RateCapped();

// From ZStreamerU
	virtual const ZStreamU& GetStreamU();

protected:
	ZRef<ZStreamerU> fStreamerReal;
	ZStreamU_RateCapped fStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRPos_RateCapped

class ZStreamRPos_RateCapped : public ZStreamRPos_Filter
	{
public:
	ZStreamRPos_RateCapped(double iRate, size_t iQuantum, const ZStreamRPos& iStreamReal);

	~ZStreamRPos_RateCapped();

// From ZStreamR via ZStreamRPos_Filter
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);

protected:
	RateLimiter fLimiter;
	size_t fLastCount;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRPos_RateCapped

class ZStreamerRPos_RateCapped : public ZStreamerRPos
	{
public:
	ZStreamerRPos_RateCapped(double iRate, size_t iQuantum, ZRef<ZStreamerRPos> iStreamerReal);

	virtual ~ZStreamerRPos_RateCapped();

// From ZStreamerRPos
	virtual const ZStreamRPos& GetStreamRPos();

protected:
	ZRef<ZStreamerRPos> fStreamerReal;
	ZStreamRPos_RateCapped fStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_RateCapped

class ZStreamW_RateCapped : public ZStreamW_Filter
	{
public:
	ZStreamW_RateCapped(double iRate, size_t iQuantum, const ZStreamW& iStreamReal);

	~ZStreamW_RateCapped();

// From ZStreamW via ZStreamW_Filter
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

protected:
	RateLimiter fLimiter;
	size_t fLastCount;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerW_RateCapped

class ZStreamerW_RateCapped : public ZStreamerW
	{
public:
	ZStreamerW_RateCapped(double iRate, size_t iQuantum, ZRef<ZStreamerW> iStreamerReal);

	virtual ~ZStreamerW_RateCapped();

// From ZStreamerW
	virtual const ZStreamW& GetStreamW();

protected:
	ZRef<ZStreamerW> fStreamerReal;
	ZStreamW_RateCapped fStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamWPos_RateCapped

class ZStreamWPos_RateCapped : public ZStreamWPos_Filter
	{
public:
	ZStreamWPos_RateCapped(double iRate, size_t iQuantum, const ZStreamWPos& iStreamReal);

	~ZStreamWPos_RateCapped();

// From ZStreamW via ZStreamWPos_Filter
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

protected:
	RateLimiter fLimiter;
	size_t fLastCount;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerWPos_RateCapped

class ZStreamerWPos_RateCapped : public ZStreamerWPos
	{
public:
	ZStreamerWPos_RateCapped(double iRate, size_t iQuantum, ZRef<ZStreamerWPos> iStreamerReal);

	virtual ~ZStreamerWPos_RateCapped();

// From ZStreamerWPos
	virtual const ZStreamWPos& GetStreamWPos();

protected:
	ZRef<ZStreamerWPos> fStreamerReal;
	ZStreamWPos_RateCapped fStream;
	};

NAMESPACE_ZOOLIB_END

#endif // __ZStream_RateCapped__
