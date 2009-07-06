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

#include "zoolib/ZVal_ZooLib.h"
#include "zoolib/ZYad.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZYadR_ZooLib

class ZYadR_ZooLib : public virtual ZYadR
	{
public:
	ZYadR_ZooLib();
	ZYadR_ZooLib(ZType iType, const ZStreamR& iStreamR);
	ZYadR_ZooLib(const ZVal_ZooLib& iTV);

// From ZYadR
	virtual bool IsSimple(const ZYadOptions& iOptions);

// Our protocol
	const ZVal_ZooLib& GetVal();

private:
	const ZVal_ZooLib fVal;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStreamRPos_MemoryBlock

typedef ZStreamerRPos_T<ZStreamRPos_MemoryBlock> ZStreamerRPos_MemoryBlock;

class ZYadStreamRPos_MemoryBlock
:	public ZYadR_ZooLib,
	public ZYadStreamR,
	public ZStreamerRPos_MemoryBlock
	{
public:
	ZYadStreamRPos_MemoryBlock(const ZMemoryBlock& iMB);

// From ZYadR, disambiguating between ZYadR_ZooLib and ZYadStreamR
	virtual bool IsSimple(const ZYadOptions& iOptions);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStrimU_String

typedef ZStrimmerU_T<ZStrimU_String> ZStrimmerU_String;

class ZYadStrimU_String
:	public ZYadR_ZooLib,
	public ZYadStrimR,
	public ZStrimmerU_String
	{
public:
	ZYadStrimU_String(const std::string& iString);

// From ZYadR, disambiguating between ZYadR_ZooLib and ZYadStreamR
//	virtual bool IsSimple(const ZYadOptions& iOptions);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListRPos_ZooLib

class ZYadListRPos_ZooLib
:	public ZYadR_ZooLib,
	public ZYadListRPos
	{
public:
	ZYadListRPos_ZooLib(const ZValList_ZooLib& iList);
	ZYadListRPos_ZooLib(const ZValList_ZooLib& iList, uint64 iPosition);

// From ZYadR via ZYadListRPos
	virtual ZRef<ZYadR> ReadInc();

// From ZYadR, disambiguating between ZYadR_ZooLib and ZYadListRPos
	virtual bool IsSimple(const ZYadOptions& iOptions);

// From ZYadListR via ZYadListRPos
	virtual uint64 GetPosition();

// From ZYadListRPos
	virtual uint64 GetSize();
	virtual void SetPosition(uint64 iPosition);
	virtual ZRef<ZYadListRPos> Clone();

private:
	const ZValList_ZooLib fList;
	uint64 fPosition;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListMapRPos_ZooLib

class ZYadMapRPos_ZooLib
:	public ZYadR_ZooLib,
	public ZYadMapRPos
	{
public:
	ZYadMapRPos_ZooLib(const ZValMap_ZooLib& iMap);
	ZYadMapRPos_ZooLib(const ZValMap_ZooLib& iMap, const ZValMap_ZooLib::const_iterator& iIter);

// From ZYadR via ZYadMapRPos
	virtual ZRef<ZYadR> ReadInc(std::string& oName);

// From ZYadR, disambiguating between ZYadR_ZooLib and ZYadMapRPos
	virtual bool IsSimple(const ZYadOptions& iOptions);

// From ZYadMapR via ZYadMapRPos
	virtual void SetPosition(const std::string& iName);
	virtual ZRef<ZYadMapRPos> Clone();

private:
	const ZValMap_ZooLib fMap;
	ZValMap_ZooLib::const_iterator fIter;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYad_ZooLib

namespace ZYad_ZooLib {

ZRef<ZYadR> sMakeYadR(const ZVal_ZooLib& iTV);

ZVal_ZooLib sFromYadR(ZRef<ZYadR> iYadR);

} // namespace ZYad_ZooLib

NAMESPACE_ZOOLIB_END

#endif // __ZYad_ZooLib__
