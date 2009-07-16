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

#include "zoolib/ZFunctionChain.h"
#include "zoolib/ZYad_ZooLib.h"

NAMESPACE_ZOOLIB_BEGIN

using std::pair;
using std::string;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Factory

namespace ZANONYMOUS {

class Maker0
:	public ZFunctionChain_T<ZVal_ZooLib, ZRef<ZYadR> >
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
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

ZYadR_ZooLib::ZYadR_ZooLib(const ZVal_ZooLib& iVal)
:	YadBase_t(iVal)
	{}

ZYadR_ZooLib::ZYadR_ZooLib(ZType iType, const ZStreamR& iStreamR)
:	YadBase_t(iType, iStreamR)
	{}

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

ZYadStrimU_String::ZYadStrimU_String(const string& iString)
:	ZYadR_ZooLib(iString),
	ZStrimmerU_String(this->GetVal().GetString())
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListRPos_ZooLib

ZYadListRPos_ZooLib::ZYadListRPos_ZooLib(const ZValList_ZooLib& iList)
:	ZYadR_ZooLib(iList)
,	YadListBase_t(iList)
	{}

ZYadListRPos_ZooLib::ZYadListRPos_ZooLib(const ZValList_ZooLib& iList, uint64 iPosition)
:	ZYadR_ZooLib(iList)
,	YadListBase_t(iList, iPosition)
	{}

bool ZYadListRPos_ZooLib::IsSimple(const ZYadOptions& iOptions)
	{ return ZYadR_ZooLib::IsSimple(iOptions); }

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapRPos_ZooLib

ZYadMapRPos_ZooLib::ZYadMapRPos_ZooLib(const ZValMap_ZooLib& iMap)
:	ZYadR_ZooLib(iMap)
,	YadMapBase_t(iMap, iMap.begin())
	{}

ZYadMapRPos_ZooLib::ZYadMapRPos_ZooLib(const ZValMap_ZooLib& iMap, const Index_t& iIndex)
:	ZYadR_ZooLib(iMap)
,	YadMapBase_t(iMap, iIndex)
	{}

bool ZYadMapRPos_ZooLib::IsSimple(const ZYadOptions& iOptions)
	{ return ZYadR_ZooLib::IsSimple(iOptions); }

ZRef<ZYadR> ZYadMapRPos_ZooLib::ReadInc(string& oName)
	{
	if (fIndex != fMap.end())
		{
		oName = fMap.NameOf(fIndex).AsString();
		return sMakeYadR(fMap.Get(fIndex++));
		}
	return ZRef<ZYadR>();
	}

void ZYadMapRPos_ZooLib::SetPosition(const std::string& iName)
	{ fIndex = fMap.IndexOf(iName); }

// =================================================================================================
#pragma mark -
#pragma mark * sMakeYadR

ZRef<ZYadR> sMakeYadR(const ZVal_ZooLib& iVal)
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

// =================================================================================================
#pragma mark -
#pragma mark * sFromYadR_T

namespace ZANONYMOUS {

class YadVisitor_GetValZooLib : public ZYadVisitor
	{
public:
// From ZYadVisitor
	virtual bool Visit_YadR(ZRef<ZYadR> iYadR);
	virtual bool Visit_YadStreamR(ZRef<ZYadStreamR> iYadStreamR);
	virtual bool Visit_YadStrimR(ZRef<ZYadStrimR> iYadStrimR);
	virtual bool Visit_YadListR(ZRef<ZYadListR> iYadListR);
	virtual bool Visit_YadMapR(ZRef<ZYadMapR> iYadMapR);

	ZVal_ZooLib fOutput;
	};

} // anonymous namespace

template <>
ZVal_ZooLib sFromYadR_T<ZVal_ZooLib>(ZRef<ZYadR> iYadR)
	{
	YadVisitor_GetValZooLib theVisitor;
	iYadR->Accept(theVisitor);
	return theVisitor.fOutput;
	}

bool YadVisitor_GetValZooLib::Visit_YadR(ZRef<ZYadR> iYadR)
	{
	fOutput = ZFunctionChain_T<ZVal_ZooLib, ZRef<ZYadR> >::sInvoke(iYadR);
	return true;
	}

bool YadVisitor_GetValZooLib::Visit_YadStreamR(ZRef<ZYadStreamR> iYadStreamR)
	{
	fOutput = sReadAll_T<ZValData_ZooLib>(iYadStreamR->GetStreamR());
	return true;
	}

bool YadVisitor_GetValZooLib::Visit_YadStrimR(ZRef<ZYadStrimR> iYadStrimR)
	{
	string theString;
	ZStrimW_String(theString).CopyAllFrom(iYadStrimR->GetStrimR());
	fOutput = theString;
	return true;
	}

bool YadVisitor_GetValZooLib::Visit_YadListR(ZRef<ZYadListR> iYadListR)
	{
	ZValList_ZooLib theList;

	while (ZRef<ZYadR> theChild = iYadListR->ReadInc())
		theList.Append(sFromYadR_T<ZVal_ZooLib>(theChild));

	fOutput = theList;
	return true;
	}

bool YadVisitor_GetValZooLib::Visit_YadMapR(ZRef<ZYadMapR> iYadMapR)
	{
	ZValMap_ZooLib theMap;

	string theName;
	while (ZRef<ZYadR> theChild = iYadMapR->ReadInc(theName))
		theMap.Set(theName, sFromYadR_T<ZVal_ZooLib>(theChild));

	fOutput = theMap;
	return true;
	}

NAMESPACE_ZOOLIB_END
