/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#ifndef __ZYad_Val_T__
#define __ZYad_Val_T__ 1
#include "zconfig.h"

#include "zoolib/ZYad.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZYadR_Val_T

template <class Val_t>
class ZYadR_Val_T : public virtual ZYadR
	{
public:
	typedef ZYadR_Val_T YadBase_t;

	ZYadR_Val_T()
		{}

	template <class P0>
	ZYadR_Val_T(const P0& iP0)
	:	fVal(iP0)
		{}

	template <class P0, class P1>
	ZYadR_Val_T(const P0& iP0, const P1& iP1)
	:	fVal(iP0, iP1)
		{}

// Our protocol
	Val_t GetVal()
		{ return fVal; }

protected:
	const Val_t fVal;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListRPos_Val_T

template <class Self_t, class ValList_t>
class ZYadListRPos_Val_T
:	public ZYadListRPos
	{
public:
	typedef ZYadListRPos_Val_T YadListBase_t;

	ZYadListRPos_Val_T(const ValList_t& iList)
	:	fList(iList)
	,	fPosition(0)
		{}

	ZYadListRPos_Val_T(const ValList_t& iList, uint64 iPosition)
	:	fList(iList)
	,	fPosition(iPosition)
		{}

// From ZYadListR via ZYadListRPos
	virtual ZRef<ZYadR> ReadInc()
		{
		if (fPosition < fList.Count())
			return sMakeYadR(fList.Get(fPosition++));
		return ZRef<ZYadR>();
		}

// From ZYadListRPos
	virtual ZRef<ZYadListRPos> Clone()
		{ return new Self_t(fList, fPosition); }

	virtual uint64 GetPosition()
		{ return fPosition; }

	virtual void SetPosition(uint64 iPosition)
		{ fPosition = iPosition; }

	virtual uint64 GetSize()
		{ return fList.Count(); }

protected:
	const ValList_t fList;
	uint64 fPosition;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapRPos_Val_T

template <class Self_t, class ValMap_t, class Index_P = typename ValMap_t::Index_t>
class ZYadMapRPos_Val_T
:	public ZYadMapRPos
	{
public:
	typedef ZYadMapRPos_Val_T YadMapBase_t;
	typedef Index_P Index_t;

	ZYadMapRPos_Val_T(const ValMap_t& iMap)
	:	fMap(iMap)
		{}

	ZYadMapRPos_Val_T(const ValMap_t& iMap, const Index_t& iIndex)
	:	fMap(iMap)
	,	fIndex(iIndex)
		{}

// From ZYadMapR
	// Redeclared as pure virtual for emphasis. Concrete subclass
	// has to provide this, because different ValMaps handle iteration differently.
	virtual ZRef<ZYadR> ReadInc(std::string& oName) = 0;

// From ZYadMapRPos
	virtual ZRef<ZYadMapRPos> Clone()
		{ return new Self_t(fMap, fIndex); }

	virtual void SetPosition(const std::string& iName) = 0;

protected:
	const ValMap_t fMap;
	Index_t fIndex;
	};

NAMESPACE_ZOOLIB_END

#endif // __ZYad_Val_T__
