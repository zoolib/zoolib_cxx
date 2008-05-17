/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZHandle__
#define __ZHandle__ 1
#include "zconfig.h"
#include "ZCONFIG_SPI.h"

#include "ZCompat_NonCopyable.h"
#include "ZRefCount.h"
#include "ZStreamer.h"

#include <cstddef> // For size_t

#if ZCONFIG_SPI_Enabled(Carbon)
#	include ZMACINCLUDE(CarbonCore,MacMemory.h)
#endif

// =================================================================================================
#pragma mark -
#pragma mark * ZHandle

class ZHandle
	{
public:
	class Rep;

	class Ref;
	friend class Ref;

	class ConstRef;
	friend class ConstRef;

	ZHandle();
	explicit ZHandle(size_t iInitialSize);
	ZHandle(const void* iSourceData, size_t iSize);
	ZHandle(const ZHandle& iOther);
	~ZHandle();

	ZHandle& operator=(const ZHandle& iOther);

	size_t GetSize() const;
	void SetSize(size_t iSize);

	void CopyFrom(const void* sSource, size_t sSize, size_t iOffset);
	void CopyFrom(const void* sSource, size_t iSize)
		{ this->CopyFrom(sSource, iSize, 0); }

	void CopyTo(void* iDest, size_t iSize, size_t iOffset) const;
	void CopyTo(void* iDest, size_t iSize) const
		{ this->CopyTo(iDest, iSize, 0); }

	void Touch();

	#if ZCONFIG_SPI_Enabled(Carbon)
		Handle GetMacHandle();
	#endif

private:
	ZRef<Rep> fRep;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZHandle::Rep

class ZHandle::Rep : public ZRefCounted
	{
public:
	Rep();
	Rep(size_t iSize);
	~Rep();

private:
	ZThreadSafe_t fLockCount;

	#if ZCONFIG_SPI_Enabled(Carbon)
		Handle fMacHandle;
	#else
		char* fData;
		size_t fSize;
	#endif

	friend class ZHandle;
	friend class ZHandle::Ref;
	friend class ZHandle::ConstRef;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZHandle::Ref

class ZHandle::Ref : ZooLib::NonCopyable
	{
public:
	explicit Ref(ZHandle& iHandle);
	~Ref();
	void* GetData();
	size_t GetSize();

private:
	ZHandle& fHandle;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZHandle::ConstRef

class ZHandle::ConstRef : ZooLib::NonCopyable
	{
public:
	explicit ConstRef(const ZHandle& iHandle);
	~ConstRef();
	const void* GetData();
	size_t GetSize();

private:
	const ZHandle& fHandle;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRWPos_Handle

class ZStreamRWPos_Handle : public ZStreamRWPos
	{
public:
	ZStreamRWPos_Handle(ZHandle& iHandle, size_t iGrowIncrement);
	ZStreamRWPos_Handle(ZHandle& iHandle);
	~ZStreamRWPos_Handle();

// From ZStreamR via ZStreamRWPos
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);
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

private:
	ZHandle& fHandle;
	size_t fGrowIncrement;
	uint64 fPosition;
	size_t fSizeLogical;
	};

#endif // __ZHandle__
