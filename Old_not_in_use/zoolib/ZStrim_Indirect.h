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

#ifndef __ZStrim_Indirect_h__
#define __ZStrim_Indirect_h__ 1
#include "zconfig.h"

#include "zoolib/ZStrim.h"
#include "zoolib/ZTypes.h" // For sNonConst

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ZStrim_Indirect

template <class Strim_p>
class ZStrim_Indirect
	{
public:
	ZStrim_Indirect(const Strim_p* iStrim)
	:	fStrim(sNonConst(iStrim))
		{}

	const Strim_p* Get()
		{ return fStrim; }

	void Set(const Strim_p* iStrim)
		{ fStrim = sNonConst(iStrim); }

	const Strim_p* GetSet(const Strim_p* iStrim)
		{ return GetSet(fStrim, sNonConst(iStrim)); }

	bool CAS(const Strim_p* iPrior, const Strim_p* iNew)
		{
		if (fStrim == iPrior)
			{
			fStrim = sNonConst(iNew);
			return true;
			}
		return false;
		}

protected:
	Strim_p* fStrim;
	};

// =================================================================================================
#pragma mark -
#pragma mark ZStrimR_Indirect

class ZStrimR_Indirect
:	public ZStrimR
,	public ZStrim_Indirect<ZStrimR>
	{
public:
	ZStrimR_Indirect(const ZStrimR* iStrim);
	~ZStrimR_Indirect();

// From ZStrimR
	virtual void Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount);

	virtual void Imp_ReadUTF16(UTF16* oDest,
		size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP);

	virtual void Imp_ReadUTF8(UTF8* oDest,
		size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP);

	virtual void Imp_CopyToDispatch(const ZStrimW& iStrimW,
		uint64 iCountCP, uint64* oCountCPRead, uint64* oCountCPWritten);

	virtual void Imp_CopyTo(const ZStrimW& iStrimW,
		uint64 iCountCP, uint64* oCountCPRead, uint64* oCountCPWritten);

	virtual bool Imp_ReadCP(UTF32& oCP);

	virtual void Imp_Skip(uint64 iCountCP, uint64* oCountCPSkipped);
	};

// =================================================================================================
#pragma mark -
#pragma mark ZStrimU_Indirect

class ZStrimU_Indirect
:	public ZStrimU
,	public ZStrim_Indirect<ZStrimU>
	{
public:
	ZStrimU_Indirect(const ZStrimU* iStrim);
	~ZStrimU_Indirect();

// From ZStrimR via ZStrimU
	virtual void Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount);

	virtual void Imp_ReadUTF16(UTF16* oDest,
		size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP);

	virtual void Imp_ReadUTF8(UTF8* oDest,
		size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP);

	virtual void Imp_CopyToDispatch(const ZStrimW& iStrimW,
		uint64 iCountCP, uint64* oCountCPRead, uint64* oCountCPWritten);

	virtual void Imp_CopyTo(const ZStrimW& iStrimW,
		uint64 iCountCP, uint64* oCountCPRead, uint64* oCountCPWritten);

	virtual bool Imp_ReadCP(UTF32& oCP);

	virtual void Imp_Skip(uint64 iCountCP, uint64* oCountCPSkipped);

// from ZStrimU
	virtual void Imp_Unread(UTF32 iCP);
	virtual size_t Imp_UnreadableLimit();
	};

// =================================================================================================
#pragma mark -
#pragma mark ZStrimW_Indirect

class ZStrimW_Indirect
:	public ZStrimW
,	public ZStrim_Indirect<ZStrimW>
	{
public:
	ZStrimW_Indirect(const ZStrimW* iStrim);
	~ZStrimW_Indirect();

// From ZStrimW
	virtual void Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU);
	virtual void Imp_WriteUTF16(const UTF16* iSource, size_t iCountCU, size_t* oCountCU);
	virtual void Imp_WriteUTF8(const UTF8* iSource, size_t iCountCU, size_t* oCountCU);

	virtual void Imp_CopyFromDispatch(const ZStrimR& iStrimR,
		uint64 iCountCP, uint64* oCountCPRead, uint64* oCountCPWritten);

	virtual void Imp_CopyFrom(const ZStrimR& iStrimR,
		uint64 iCountCP, uint64* oCountCPRead, uint64* oCountCPWritten);

	virtual void Imp_Flush();
	};

} // namespace ZooLib

#endif // __ZStrim_Indirect_h__
