/* ------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------ */

#ifndef __ZMemoryBlock__
#define __ZMemoryBlock__ 1
#include "zconfig.h"

#include "ZCompare.h"
#include "ZRefCount.h"
#include "ZStream.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZMemoryBlock

class ZMemoryBlock
	{
    ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZMemoryBlock,
    	operator_bool_generator_type, operator_bool_type);
public:
	class Rep;

	ZMemoryBlock();
	ZMemoryBlock(size_t iSize);
	ZMemoryBlock(const void* iSourceData, size_t iSize);
	ZMemoryBlock(const ZMemoryBlock& iOther);
	~ZMemoryBlock();

	ZMemoryBlock& operator=(const ZMemoryBlock& iOther);

	operator operator_bool_type() const;

	int Compare(const ZMemoryBlock& iOther) const;
	bool operator<(const ZMemoryBlock& iOther) const;
	bool operator==(const ZMemoryBlock& iOther) const;

	void Touch();

	size_t GetSize() const;
	void SetSize(size_t iSize);

	const void* GetData() const;
	void* GetData();

	void CopyFrom(size_t iOffset, const void* iSource, size_t iCount);
	void CopyFrom(const void* iSource, size_t iCount)
		{ this->CopyFrom(0, iSource, iCount); }
	void CopyTo(size_t iOffset, void* iDest, size_t iCount) const;
	void CopyTo(void* iDest, size_t iCount) const
		{ this->CopyTo(0, iDest, iCount); }

private:
	ZRef<Rep> fRep;
	};

namespace ZooLib {
template <> inline int sCompare_T(const ZMemoryBlock& iL, const ZMemoryBlock& iR)
	{ return iL.Compare(iR); }
} // namespace ZooLib

// =================================================================================================
#pragma mark -
#pragma mark * ZMemoryBlock::Rep

class ZMemoryBlock::Rep : public ZRefCounted
	{
public:
	static bool sCheckAccessEnabled() { return false; }

	void* operator new(size_t iObjectSize, size_t iDataSize);
	void operator delete(void* iPtr, size_t iObjectSize);

	Rep();
	Rep(size_t inSize);

private:
	size_t fSize;
	char fData[1];	// MDC was fData[0], but zero-length arrays are a GCC extension, incompatible with CW

	friend class ZMemoryBlock;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZMemoryBlock::Rep inlines

inline void* ZMemoryBlock::Rep::operator new(size_t iObjectSize, size_t iDataSize)
	{ return new char[iObjectSize + iDataSize]; }

inline void ZMemoryBlock::Rep::operator delete(void* iPtr, size_t iDataSize)
	{ delete[] static_cast<char*>(iPtr); }

inline ZMemoryBlock::Rep::Rep()
:	fSize(0)
	{}

inline ZMemoryBlock::Rep::Rep(size_t iSize)
:	fSize(iSize)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZMemoryBlock inlines

inline ZMemoryBlock::ZMemoryBlock()
:	fRep(new (0) ZMemoryBlock::Rep)
	{}

inline ZMemoryBlock::ZMemoryBlock(size_t iSize)
:	fRep(new (iSize) ZMemoryBlock::Rep(iSize))
	{}

inline ZMemoryBlock::ZMemoryBlock(const ZMemoryBlock& iOther)
:	fRep(iOther.fRep)
	{}

inline ZMemoryBlock::~ZMemoryBlock()
	{}

inline ZMemoryBlock& ZMemoryBlock::operator=(const ZMemoryBlock& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

inline ZMemoryBlock::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fRep->fSize); }

inline int ZMemoryBlock::Compare(const ZMemoryBlock& iOther) const
	{
	Rep* myRep = fRep.GetObject();
	Rep* otherRep = iOther.fRep.GetObject();

	if (myRep == otherRep)
		return 0;

	if (int result = memcmp(myRep->fData, otherRep->fData, min(myRep->fSize, otherRep->fSize)))
		return result;

	return int(myRep->fSize) - int(otherRep->fSize);
	}

inline bool ZMemoryBlock::operator<(const ZMemoryBlock& iOther) const
	{
	Rep* myRep = fRep.GetObject();
	Rep* otherRep = iOther.fRep.GetObject();

	if (myRep == otherRep)
		return false;

	if (int result = memcmp(myRep->fData, otherRep->fData, min(myRep->fSize, otherRep->fSize)))
		return result < 0;

	return fRep->fSize < otherRep->fSize;
	}

inline bool ZMemoryBlock::operator==(const ZMemoryBlock& iOther) const
	{
	Rep* myRep = fRep.GetObject();
	Rep* otherRep = iOther.fRep.GetObject();

	if (myRep == otherRep)
		return true;

	if (myRep->fSize != otherRep->fSize)
		return false;

	return 0 == memcmp(myRep->fData, otherRep->fData, myRep->fSize);
	}

inline size_t ZMemoryBlock::GetSize() const
	{ return fRep->fSize; }

inline const void* ZMemoryBlock::GetData() const
	{ return fRep->fData; }

inline void* ZMemoryBlock::GetData()
	{ return fRep->fData; }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRPos_MemoryBlock

class ZStreamRPos_MemoryBlock : public ZStreamRPos
	{
public:
	ZStreamRPos_MemoryBlock(const ZMemoryBlock& iMemoryBlock);
	~ZStreamRPos_MemoryBlock();

// From ZStreamR via ZStreamRPos
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);
	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped);

// From ZStreamRPos
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 iPosition);

	virtual uint64 Imp_GetSize();

private:
	ZMemoryBlock fMemoryBlock;
	uint64 fPosition;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRWPos_MemoryBlock

class ZStreamRWPos_MemoryBlock : public ZStreamRWPos
	{
public:
	ZStreamRWPos_MemoryBlock(ZMemoryBlock& iMemoryBlock, size_t iGrowIncrement);
	ZStreamRWPos_MemoryBlock(ZMemoryBlock& iMemoryBlock);
	~ZStreamRWPos_MemoryBlock();

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
	ZMemoryBlock& fMemoryBlock;
	size_t fGrowIncrement;
	uint64 fPosition;
	size_t fSizeLogical;
	};

// =================================================================================================

#endif // __ZMemoryBlock__
