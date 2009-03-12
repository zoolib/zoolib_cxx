/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#ifndef __ZYad_ZooLib__
#define __ZYad_ZooLib__ 1
#include "zconfig.h"

#include "zoolib/ZYad.h"
#include "zoolib/ZMemoryBlock.h"
#include "zoolib/ZTuple.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZYadR_TValue

class ZYadR_TValue : public virtual ZYadR
	{
public:
	ZYadR_TValue(ZType iType, const ZStreamR& iStreamR);
	ZYadR_TValue(const ZTValue& iTV);

// From ZYadR
	virtual bool IsSimple(const ZYadOptions& iOptions);

// Our protocol
	const ZTValue& GetTValue();

private:
	const ZTValue fTValue;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadRawRPos_MemoryBlock

typedef ZStreamerRPos_T<ZStreamRPos_MemoryBlock> ZStreamerRPos_MemoryBlock;

class ZYadRawRPos_MemoryBlock
:	public ZYadR_TValue,
	public ZYadRawR,
	public ZStreamerRPos_MemoryBlock
	{
public:
	ZYadRawRPos_MemoryBlock(const ZMemoryBlock& iMB);

// From ZYadR, disambiguating between ZYadR_TValue and ZYadRawR
	virtual bool IsSimple(const ZYadOptions& iOptions);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListRPos_Vector

class ZYadListRPos_Vector
:	public ZYadR_TValue,
	public ZYadListRPos
	{
public:
	ZYadListRPos_Vector(const ZTValue& iTV);
	ZYadListRPos_Vector(const std::vector<ZTValue>& iVector);
	ZYadListRPos_Vector(const std::vector<ZTValue>& iVector, uint64 iPosition);

// From ZYadR via ZYadListRPos
	virtual ZRef<ZYadR> ReadInc();

// From ZYadR, disambiguating between ZYadR_TValue and ZYadListRPos
	virtual bool IsSimple(const ZYadOptions& iOptions);

// From ZYadListR via ZYadListRPos
	virtual uint64 GetPosition();

// From ZYadListRPos
	virtual uint64 GetSize();
	virtual void SetPosition(uint64 iPosition);
	virtual ZRef<ZYadListRPos> Clone();

private:
	const std::vector<ZTValue>& fVector;
	uint64 fPosition;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListMapRPos_Tuple

class ZYadMapRPos_Tuple
:	public ZYadR_TValue,
	public ZYadMapRPos
	{
public:
	ZYadMapRPos_Tuple(const ZTuple& iTuple);
	ZYadMapRPos_Tuple(const ZTuple& iTuple, const ZTuple::const_iterator& iIter);

// From ZYadR via ZYadMapRPos
	virtual ZRef<ZYadR> ReadInc(std::string& oName);

// From ZYadR, disambiguating between ZYadR_TValue and ZYadMapRPos
	virtual bool IsSimple(const ZYadOptions& iOptions);

// From ZYadMapR via ZYadMapRPos
	virtual void SetPosition(const std::string& iName);
	virtual ZRef<ZYadMapRPos> Clone();

private:
	const ZTuple fTuple;
	ZTuple::const_iterator fIter;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYad_ZooLib

namespace ZYad_ZooLib {

ZRef<ZYadR> sMakeYadR(const ZTValue& iTV);

ZTValue sFromYadR(ZRef<ZYadR> iYadR);

} // namespace ZYad_ZooLib

NAMESPACE_ZOOLIB_END

#endif // __ZYad_ZooLib__
