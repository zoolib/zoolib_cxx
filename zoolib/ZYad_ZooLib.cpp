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

#include "zoolib/ZFactoryChain.h"
#include "zoolib/ZYad_ZooLib.h"

NAMESPACE_ZOOLIB_BEGIN

using std::string;
using std::vector;

ZOOLIB_FACTORYCHAIN_HEAD(ZVal_ZooLib, ZRef<ZYadR>);

// =================================================================================================
#pragma mark -
#pragma mark * Factory

namespace ZANONYMOUS {

class Maker0
:	public ZFactoryChain_T<ZVal_ZooLib, ZRef<ZYadR> >
	{
public:
	Maker0()
	:	ZFactoryChain_T<Result_t, Param_t>(true)
		{}

	virtual bool Make(Result_t& oResult, Param_t iParam)
		{
		if (ZRef<ZYadR_ZooLib> theYadR = ZRefDynamicCast<ZYadR_ZooLib>(iParam))
			{
			oResult = theYadR->GetVal();
			return true;
			}
		return false;
		}	
	} sMaker0;

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Helper functions

static bool sIsSimpleString(const ZYadOptions& iOptions, const string& iString)
	{
	if (iOptions.fStringLineLength && iString.size() > iOptions.fStringLineLength)
		{
		// We have a non-zero desired line length, and the string contains
		// more characters than that. This doesn't account for increases in width
		// due to escaping etc.
		return false;
		}

	if (iOptions.fBreakStrings)
		{
		// We've been asked to break strings at line ending characters,
		// which means (here and in ZStrimW_Escaped) LF and CR. Strictly
		// speaking we should use ZUnicode::sIsEOL().
		if (string::npos != iString.find_first_of("\n\r"))
			return false;
		}

	return true;
	}

static bool sIsSimple(const ZYadOptions& iOptions, const ZVal_ZooLib& iVal)
	{
	switch (iVal.TypeOf())
		{
		case eZType_Raw:
			{
			return iVal.GetRaw().GetSize() <= iOptions.fRawChunkSize;
			}
		case eZType_Vector:
			{
			const ZValList_ZooLib& theList = iVal.GetList();
			if (!theList.Count())
				return true;

			if (theList.Count() == 1)
				return sIsSimple(iOptions, theList.Get(0));

			return false;
			}
		case eZType_Tuple:
			{
			const ZValMap_ZooLib& theMap = iVal.GetMap();
			if (theMap.Empty())
				return true;

			if (theMap.Count() == 1)
				return sIsSimple(iOptions, theMap.RGet(theMap.begin()));

			return false;
			}
		case eZType_String:
			{
			return sIsSimpleString(iOptions, iVal.GetString());
			}
		default:
			{
			return true;
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadR_ZooLib

ZYadR_ZooLib::ZYadR_ZooLib()
	{}

ZYadR_ZooLib::ZYadR_ZooLib(ZType iType, const ZStreamR& iStreamR)
:	fVal(iType, iStreamR)
	{}

ZYadR_ZooLib::ZYadR_ZooLib(const ZVal_ZooLib& iVal)
:	fVal(iVal)
	{}

const ZVal_ZooLib& ZYadR_ZooLib::GetVal()
	{ return fVal; }

bool ZYadR_ZooLib::IsSimple(const ZYadOptions& iOptions)
	{ return sIsSimple(iOptions, fVal); }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStreamRPos_ZooLib

ZYadStreamRPos_ZooLib::ZYadStreamRPos_ZooLib(const ZValData_ZooLib& iValData)
:	ZYadR_ZooLib(iValData),
	ZStreamerRPos_ValData_ZooLib(iValData)
	{}

bool ZYadStreamRPos_ZooLib::IsSimple(const ZYadOptions& iOptions)
	{ return this->GetStreamRPos().GetSize() <= iOptions.fRawChunkSize; }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadStrimU_String

ZYadStrimU_String::ZYadStrimU_String(const std::string& iString)
:	ZYadR_ZooLib(iString),
	ZStrimmerU_String(this->GetVal().GetString())
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListRPos_ZooLib

ZYadListRPos_ZooLib::ZYadListRPos_ZooLib(const ZValList_ZooLib& iList)
:	ZYadR_ZooLib(iList),
	fList(this->GetVal().GetList()),
	fPosition(0)
	{}

ZYadListRPos_ZooLib::ZYadListRPos_ZooLib(const ZValList_ZooLib& iList, uint64 iPosition)
:	ZYadR_ZooLib(iList),
	fList(this->GetVal().GetList()),
	fPosition(iPosition)
	{}

ZRef<ZYadR> ZYadListRPos_ZooLib::ReadInc()
	{
	if (fPosition < fList.Count())
		return ZYad_ZooLib::sMakeYadR(fList.Get(fPosition++));
	return ZRef<ZYadR>();
	}

bool ZYadListRPos_ZooLib::IsSimple(const ZYadOptions& iOptions)
	{ return ZYadR_ZooLib::IsSimple(iOptions); }

uint64 ZYadListRPos_ZooLib::GetPosition()
	{ return fPosition; }

uint64 ZYadListRPos_ZooLib::GetSize()
	{ return fList.Count(); }

void ZYadListRPos_ZooLib::SetPosition(uint64 iPosition)
	{ fPosition = iPosition; }

ZRef<ZYadListRPos> ZYadListRPos_ZooLib::Clone()
	{ return new ZYadListRPos_ZooLib(fList, fPosition); }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapRPos_ZooLib

ZYadMapRPos_ZooLib::ZYadMapRPos_ZooLib(const ZValMap_ZooLib& iMap)
:	ZYadR_ZooLib(iMap),
	fMap(this->GetVal().GetMap()),
	fIter(fMap.begin())
	{}

ZYadMapRPos_ZooLib::ZYadMapRPos_ZooLib(const ZValMap_ZooLib& iMap, const ZValMap_ZooLib::const_iterator& iIter)
:	ZYadR_ZooLib(iMap),
	fMap(this->GetVal().GetMap()),
	fIter(iIter)
	{}

ZRef<ZYadR> ZYadMapRPos_ZooLib::ReadInc(std::string& oName)
	{
	if (fIter < fMap.end())
		{
		oName = fMap.NameOf(fIter).AsString();
		return ZYad_ZooLib::sMakeYadR(fMap.Get(fIter++));
		}
	return ZRef<ZYadR>();
	}

bool ZYadMapRPos_ZooLib::IsSimple(const ZYadOptions& iOptions)
	{ return ZYadR_ZooLib::IsSimple(iOptions); }

void ZYadMapRPos_ZooLib::SetPosition(const std::string& iName)
	{ fIter = fMap.IteratorOf(iName); }

ZRef<ZYadMapRPos> ZYadMapRPos_ZooLib::Clone()
	{ return new ZYadMapRPos_ZooLib(fMap, fIter); }

// =================================================================================================
#pragma mark -
#pragma mark * ZYad_ZooLib

ZRef<ZYadR> ZYad_ZooLib::sMakeYadR(const ZVal_ZooLib& iVal)
	{
	switch (iVal.TypeOf())
		{
		case eZType_Vector: return new ZYadListRPos_ZooLib(iVal.GetList());
		case eZType_Tuple: return new ZYadMapRPos_ZooLib(iVal.GetMap());
		case eZType_Raw: return new ZYadStreamRPos_ZooLib(iVal.GetData());
		case eZType_String: return new ZYadStrimU_String(iVal.GetString());
		}

	return new ZYadR_ZooLib(iVal);
	}

ZVal_ZooLib ZYad_ZooLib::sFromYadR(ZRef<ZYadR> iYadR)
	{
	if (!iYadR)
		{
		return ZVal_ZooLib();
		}
	else if (ZRef<ZYadR_ZooLib> theYadR = ZRefDynamicCast<ZYadR_ZooLib>(iYadR))
		{
		return theYadR->GetVal();
		}
	else if (ZRef<ZYadMapR> theYadMapR = ZRefDynamicCast<ZYadMapR>(iYadR))
		{
		ZValMap_ZooLib theMap;
		string theName;
		while (ZRef<ZYadR> theChild = theYadMapR->ReadInc(theName))
			theMap.Set(theName, sFromYadR(theChild));

		return theMap;
		}
	else if (ZRef<ZYadListR> theYadListR = ZRefDynamicCast<ZYadListR>(iYadR))
		{
		ZValList_ZooLib theList;
		while (ZRef<ZYadR> theChild = theYadListR->ReadInc())
			theList.Append(sFromYadR(theChild));

		return theList;	
		}
	else if (ZRef<ZYadStreamR> theYadStreamR = ZRefDynamicCast<ZYadStreamR>(iYadR))
		{
		return sReadAll_T<ZValData_ZooLib>(theYadStreamR->GetStreamR());
		}
	else if (ZRef<ZYadStrimR> theYadStrimR = ZRefDynamicCast<ZYadStrimR>(iYadR))
		{
		string theString;
		ZStrimW_String(theString).CopyAllFrom(theYadStrimR->GetStrimR());
		return theString;
		}
	else
		{
		return ZFactoryChain_T<ZVal_ZooLib, ZRef<ZYadR> >::sMake(iYadR);
		}
	}

NAMESPACE_ZOOLIB_END
