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
#include "zoolib/ZStream_Data_T.h"

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
#pragma mark * ZYadStreamRPos_Val_T

template <class Data_t>
class ZYadStreamRPos_Val_T
:	public ZYadStreamR,
	public ZStreamerRPos_T<ZStreamRPos_Data_T<Data_t> >
	{
public:
	ZYadStreamRPos_Val_T(const Data_t& iData)
	:	ZStreamerRPos_T<ZStreamRPos_Data_T<Data_t> >(iData)
		{}

// From ZYadR
	bool IsSimple(const ZYadOptions& iOptions)
		{ return this->GetStreamRPos().GetSize() <= iOptions.fRawChunkSize; }
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListRPos_Val_T

template <class List_t>
class ZYadListRPos_Val_T
:	public ZYadListRPos
	{
public:
	ZYadListRPos_Val_T(const List_t& iList)
	:	fList(iList)
	,	fPosition(0)
		{}

	ZYadListRPos_Val_T(const List_t& iList, uint64 iPosition)
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
		{ return new ZYadListRPos_Val_T(fList, fPosition); }

	virtual uint64 GetPosition()
		{ return fPosition; }

	virtual void SetPosition(uint64 iPosition)
		{ fPosition = iPosition; }

	virtual uint64 GetSize()
		{ return fList.Count(); }

protected:
	const List_t fList;
	uint64 fPosition;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListRPos_Val_Self_T

// Urgh, quite an ugly name.

template <class Self_t, class List_t>
class ZYadListRPos_Val_Self_T
:	public ZYadListRPos
	{
public:
	typedef ZYadListRPos_Val_Self_T YadListBase_t;

protected:
	ZYadListRPos_Val_Self_T(const List_t& iList)
	:	fList(iList)
	,	fPosition(0)
		{}

	ZYadListRPos_Val_Self_T(const List_t& iList, uint64 iPosition)
	:	fList(iList)
	,	fPosition(iPosition)
		{}

public:
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
	const List_t fList;
	uint64 fPosition;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapRPos_Val_T

template <class Map_t, class Index_P = typename Map_t::Index_t>
class ZYadMapRPos_Val_T
:	public ZYadMapRPos
	{
public:
	typedef Index_P Index_t;

	ZYadMapRPos_Val_T(const Map_t& iMap)
	:	fMap(iMap)
	,	fIndex(fMap.Begin())
		{}

	ZYadMapRPos_Val_T(const Map_t& iMap, const Index_t& iIndex)
	:	fMap(iMap)
	,	fIndex(fMap.IndexOf(iMap, iIndex))
		{}

// From ZYadMapR
	ZRef<ZYadR> ReadInc(std::string& oName)
		{
		if (fIndex != fMap.End())
			{
			oName = fMap.NameOf(fIndex);
			return sMakeYadR(fMap.Get(fIndex++));
			}
		return ZRef<ZYadR>();
		}

// From ZYadMapRPos
	void SetPosition(const std::string& iName)
		{ fIndex = fMap.IndexOf(iName); }

	virtual ZRef<ZYadMapRPos> Clone()
		{ return new ZYadMapRPos_Val_T(fMap, fIndex); }

protected:
	const Map_t fMap;
	Index_t fIndex;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapRPos_Val_Self_T

template <class Self_t, class Map_t, class Index_P = typename Map_t::Index_t>
class ZYadMapRPos_Val_Self_T
:	public ZYadMapRPos
	{
public:
	typedef Index_P Index_t;

protected:
	typedef ZYadMapRPos_Val_Self_T YadMapBase_t;
	ZYadMapRPos_Val_Self_T(const Map_t& iMap)
	:	fMap(iMap)
	,	fIndex(fMap.Begin())
		{}

	ZYadMapRPos_Val_Self_T(const Map_t& iMap, const Index_t& iIndex)
	:	fMap(iMap)
	,	fIndex(iIndex)
		{}

public:
// From ZYadMapR
	ZRef<ZYadR> ReadInc(std::string& oName)
		{
		if (fIndex != fMap.End())
			{
			oName = fMap.NameOf(fIndex);
			return sMakeYadR(fMap.Get(fIndex++));
			}
		return ZRef<ZYadR>();
		}

// From ZYadMapRPos
	void SetPosition(const std::string& iName)
		{ fIndex = fMap.IndexOf(iName); }

	virtual ZRef<ZYadMapRPos> Clone()
		{ return new Self_t(fMap, fIndex); }

protected:
	const Map_t fMap;
	Index_t fIndex;
	};

NAMESPACE_ZOOLIB_END

#endif // __ZYad_Val_T__
