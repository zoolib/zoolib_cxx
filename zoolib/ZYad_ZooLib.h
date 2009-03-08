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
#pragma mark * ZYadPrimR_TValue

class ZYadPrimR_TValue
:	public virtual ZYadR_TValue,
	public virtual ZYadPrimR
	{
public:
	ZYadPrimR_TValue(ZType iType, const ZStreamR& iStreamR);
	ZYadPrimR_TValue(const ZTValue& iTV);
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

// From ZYadR via ZYadListRPos
	virtual bool HasChild();
	virtual ZRef<ZYadR> NextChild();

// From ZYadR, disambiguating between ZYadR_TValue and ZYadListRPos
	virtual bool IsSimple(const ZYadOptions& iOptions);

// From ZYadListR via ZYadListRPos
	virtual size_t GetPosition();

// From ZYadListRPos
	virtual size_t GetSize();
	virtual void SetPosition(size_t iPosition);

private:
	const std::vector<ZTValue>& fVector;
	std::vector<ZTValue>::const_iterator fIter;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListMapRPos_Tuple

class ZYadListMapRPos_Tuple
:	public ZYadR_TValue,
	public ZYadListMapRPos
	{
public:
	ZYadListMapRPos_Tuple(const ZTuple& iTuple);

// From ZYadR via ZYadListMapRPos
	virtual bool HasChild();
	virtual ZRef<ZYadR> NextChild();

// From ZYadR, disambiguating between ZYadR_TValue and ZYadListMapRPos
	virtual bool IsSimple(const ZYadOptions& iOptions);

// From ZYadListR via ZYadListMapRPos
	virtual size_t GetPosition();

// From ZYadListRPos via ZYadListMapRPos
	virtual size_t GetSize();
	virtual void SetPosition(size_t iPosition);

// From ZYadMapR via ZYadListMapRPos
	virtual std::string Name();

// From ZYadMapRPos via ZYadListMapRPos
	virtual void SetPosition(const std::string& iName);

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
