/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#ifndef __ZStream_Indirect_h__
#define __ZStream_Indirect_h__ 1
#include "zconfig.h"

#include "zoolib/ZStream.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ZStream_Indirect

template <class Stream_p>
class ZStream_Indirect
	{
public:
	ZStream_Indirect(const Stream_p* iStream)
	:	fStream(sNonConst(iStream))
		{}

	const Stream_p* Get()
		{ return fStream; }

	void Set(const Stream_p* iStream)
		{ fStream = sNonConst(iStream); }

	const Stream_p* GetSet(const Stream_p* iStream)
		{ return sGetSet(fStream, sNonConst(iStream)); }

	bool CAS(const Stream_p* iPrior, const Stream_p* iNew)
		{
		if (fStream == iPrior)
			{
			fStream = sNonConst(iNew);
			return true;
			}
		return false;
		}

protected:
	Stream_p* fStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark ZStreamR_Indirect

class ZStreamR_Indirect
:	public ZStreamR
,	public ZStream_Indirect<ZStreamR>
	{
public:
	ZStreamR_Indirect(const ZStreamR* iStream);
	~ZStreamR_Indirect();

// From ZStreamR
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual bool Imp_WaitReadable(double iTimeout);
	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped);

	virtual void Imp_CopyToDispatch(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_CopyTo(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);
	};

// =================================================================================================
#pragma mark -
#pragma mark ZStreamRCon_Indirect

class ZStreamRCon_Indirect
:	public ZStreamRCon
,	public ZStream_Indirect<ZStreamRCon>
	{
public:
	ZStreamRCon_Indirect(const ZStreamRCon* iStream);
	~ZStreamRCon_Indirect();

// From ZStreamR via ZStreamRCon
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
	virtual bool Imp_WaitReadable(double iTimeout);
	virtual size_t Imp_CountReadable();
	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped);

// From ZStreamRCon
	virtual bool Imp_ReceiveDisconnect(double iTimeout);
	virtual void Imp_Abort();
	};

// =================================================================================================
#pragma mark -
#pragma mark ZStreamU_Indirect

class ZStreamU_Indirect
:	public ZStreamU
,	public ZStream_Indirect<ZStreamU>
	{
public:
	ZStreamU_Indirect(const ZStreamU* iStream);
	~ZStreamU_Indirect();

// From ZStreamR via ZStreamU
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual bool Imp_WaitReadable(double iTimeout);
	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped);

// from ZStreamU
	virtual void Imp_Unread();
	};

// =================================================================================================
#pragma mark -
#pragma mark ZStreamRPos_Indirect

class ZStreamRPos_Indirect
:	public ZStreamRPos
,	public ZStream_Indirect<ZStreamRPos>
	{
public:
	ZStreamRPos_Indirect(const ZStreamRPos* iStream);
	~ZStreamRPos_Indirect();

// From ZStreamR via ZStreamRPos
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped);

// From ZStreamRPos
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 iPosition);

	virtual uint64 Imp_GetSize();
	};

// =================================================================================================
#pragma mark -
#pragma mark ZStreamW_Indirect

class ZStreamW_Indirect
:	public ZStreamW
,	public ZStream_Indirect<ZStreamW>
	{
public:
	ZStreamW_Indirect(const ZStreamW* iStream);
	~ZStreamW_Indirect();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();
	};

// =================================================================================================
#pragma mark -
#pragma mark ZStreamWCon_Indirect

class ZStreamWCon_Indirect
:	public ZStreamWCon
,	public ZStream_Indirect<ZStreamWCon>
	{
public:
	ZStreamWCon_Indirect(const ZStreamWCon* iStream);
	~ZStreamWCon_Indirect();

// From ZStreamW via ZStreamRWCon
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

// From ZStreamWCon
	virtual void Imp_SendDisconnect();
	virtual void Imp_Abort();
	};

// =================================================================================================
#pragma mark -
#pragma mark ZStreamWPos_Indirect

class ZStreamWPos_Indirect
:	public ZStreamWPos
,	public ZStream_Indirect<ZStreamWPos>
	{
public:
	ZStreamWPos_Indirect(const ZStreamWPos* iStream);
	~ZStreamWPos_Indirect();

// From ZStreamW via ZStreamRWPos
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

// From ZStreamWPos
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 iPosition);

	virtual uint64 Imp_GetSize();
	virtual void Imp_SetSize(uint64 iSize);
	};

// =================================================================================================
#pragma mark -
#pragma mark ZStreamRWPos_Indirect

class ZStreamRWPos_Indirect
:	public ZStreamRWPos
,	public ZStream_Indirect<ZStreamRWPos>
	{
public:
	ZStreamRWPos_Indirect(const ZStreamRWPos* iStream);
	~ZStreamRWPos_Indirect();

// From ZStreamR via ZStreamRWPos
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped);

// From ZStreamW via ZStreamRWPos
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

// From ZStreamRPos/ZStreamWPos via ZStreamRWPos
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 iPosition);

	virtual uint64 Imp_GetSize();

// From ZStreamWPos via ZStreamRWPos
	virtual void Imp_SetSize(uint64 iSize);
	};

} // namespace ZooLib

#endif // __ZStream_Indirect_h__
