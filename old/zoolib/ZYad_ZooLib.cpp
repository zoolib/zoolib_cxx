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

#include "zoolib/ZYad_Std.h"
#include "zoolib/ZYad_ZooLib.h"

namespace ZooLib {

using std::pair;
using std::string;
using std::vector;

// =================================================================================================
// MARK: - Helper functions

static bool spIsSimpleString(const ZYadOptions& iOptions, const string& iString)
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

static bool spIsSimple(const ZYadOptions& iOptions, const ZVal_ZooLib& iVal)
	{
	switch (iVal.TypeOf())
		{
		case eZType_Raw:
			{
			return iVal.GetRaw().GetSize() <= iOptions.fRawChunkSize;
			}
		case eZType_Vector:
			{
			const ZSeq_ZooLib& theSeq = iVal.GetSeq();
			if (!theSeq.Count())
				return true;

			if (theSeq.Count() == 1)
				return spIsSimple(iOptions, theSeq.Get(0));

			return false;
			}
		case eZType_Tuple:
			{
			const ZMap_ZooLib& theMap = iVal.GetMap();
			if (theMap.Empty())
				return true;

			if (theMap.Count() == 1)
				return spIsSimple(iOptions, *theMap.PGet(theMap.Begin()));

			return false;
			}
		case eZType_String:
			{
			return spIsSimpleString(iOptions, iVal.GetString());
			}
		default:
			{
			return true;
			}
		}
	}

// =================================================================================================
// MARK: - ZYadAtomR_ZooLib

ZYadAtomR_ZooLib::ZYadAtomR_ZooLib()
	{}

ZYadAtomR_ZooLib::ZYadAtomR_ZooLib(const ZVal_ZooLib& iVal)
:	ZYadR_ZooLib(iVal)
	{}

ZYadAtomR_ZooLib::ZYadAtomR_ZooLib(ZType iType, const ZStreamR& iStreamR)
:	ZYadR_ZooLib(ZVal_ZooLib(iType, iStreamR))
	{}

bool ZYadAtomR_ZooLib::IsSimple(const ZYadOptions& iOptions)
	{ return spIsSimple(iOptions, fVal); }

ZAny ZYadAtomR_ZooLib::AsAny()
	{ return fVal.AsAny(); }

// =================================================================================================
// MARK: - ZYadSeqRPos_ZooLib

ZYadSeqRPos_ZooLib::ZYadSeqRPos_ZooLib(const ZSeq_ZooLib& iSeq)
:	ZYadR_ZooLib(iSeq)
,	YadSeqBase_t(iSeq)
	{}

ZYadSeqRPos_ZooLib::ZYadSeqRPos_ZooLib(const ZSeq_ZooLib& iSeq, uint64 iPosition)
:	ZYadR_ZooLib(iSeq)
,	YadSeqBase_t(iSeq, iPosition)
	{}

bool ZYadSeqRPos_ZooLib::IsSimple(const ZYadOptions& iOptions)
	{ return spIsSimple(iOptions, fVal); }

// =================================================================================================
// MARK: - ZYadMapRPos_ZooLib

ZYadMapRPos_ZooLib::ZYadMapRPos_ZooLib(const ZMap_ZooLib& iMap)
:	ZYadR_ZooLib(iMap)
,	YadMapBase_t(iMap)
	{}

ZYadMapRPos_ZooLib::ZYadMapRPos_ZooLib(const ZMap_ZooLib& iMap, const Index_t& iIndex)
:	ZYadR_ZooLib(iMap)
,	YadMapBase_t(iMap, iIndex)
	{}

bool ZYadMapRPos_ZooLib::IsSimple(const ZYadOptions& iOptions)
	{ return spIsSimple(iOptions, fVal); }

// =================================================================================================
// MARK: - sYadR

ZRef<ZYadR> sYadR(const ZVal_ZooLib& iVal)
	{
	switch (iVal.TypeOf())
		{
		case eZType_Vector: return sYadR(iVal.GetSeq());
		case eZType_Tuple: return sYadR(iVal.GetMap());
		case eZType_Raw: return new ZYadStreamRPos_ZooLib(iVal.GetData());
		case eZType_String: return sYadR(iVal.GetString());
		}

	return new ZYadAtomR_ZooLib(iVal);
	}

ZRef<ZYadSeqRPos> sYadR(const ZSeq_ZooLib& iSeq)
	{ return new ZYadSeqRPos_ZooLib(iSeq); }

ZRef<ZYadMapRPos> sYadR(const ZMap_ZooLib& iMap)
	{ return new ZYadMapRPos_ZooLib(iMap); }

// =================================================================================================
// MARK: - sFromYadR

namespace { // anonymous

class Visitor_GetVal : public ZVisitor_Yad
	{
public:
	Visitor_GetVal(const ZVal_ZooLib& iDefault);

// From ZVisitor_Yad
	virtual void Visit_YadAtomR(const ZRef<ZYadAtomR>& iYadAtomR);
	virtual void Visit_YadStreamR(const ZRef<ZYadStreamR>& iYadStreamR);
	virtual void Visit_YadStrimR(const ZRef<ZYadStrimR>& iYadStrimR);
	virtual void Visit_YadSeqR(const ZRef<ZYadSeqR>& iYadSeqR);
	virtual void Visit_YadMapR(const ZRef<ZYadMapR>& iYadMapR);

// Our protocol
	ZVal_ZooLib GetVal(const ZRef<ZYadR>& iYadR);

private:
	ZVal_ZooLib fDefault;
	ZVal_ZooLib fOutput;
	};

} // anonymous namespace

Visitor_GetVal::Visitor_GetVal(const ZVal_ZooLib& iDefault)
:	fDefault(iDefault)
	{}

void Visitor_GetVal::Visit_YadAtomR(const ZRef<ZYadAtomR>& iYadAtomR)
	{ fOutput = ZVal_ZooLib::sDFromAny(fDefault, iYadAtomR->AsAny()); }

void Visitor_GetVal::Visit_YadStreamR(const ZRef<ZYadStreamR>& iYadStreamR)
	{ fOutput = sReadAll_T<ZData_ZooLib>(iYadStreamR->GetStreamR()); }

void Visitor_GetVal::Visit_YadStrimR(const ZRef<ZYadStrimR>& iYadStrimR)
	{ fOutput = iYadStrimR->GetStrimR().ReadAll8(); }

void Visitor_GetVal::Visit_YadSeqR(const ZRef<ZYadSeqR>& iYadSeqR)
	{
	ZSeq_ZooLib theSeq;

	while (ZRef<ZYadR> theChild = iYadSeqR->ReadInc())
		theSeq.Append(this->GetVal(theChild));

	fOutput = theSeq;
	}

void Visitor_GetVal::Visit_YadMapR(const ZRef<ZYadMapR>& iYadMapR)
	{
	ZMap_ZooLib theMap;

	string theName;
	while (ZRef<ZYadR> theChild = iYadMapR->ReadInc(theName))
		theMap.Set(theName, this->GetVal(theChild));

	fOutput = theMap;
	}

ZVal_ZooLib Visitor_GetVal::GetVal(const ZRef<ZYadR>& iYadR)
	{
	ZVal_ZooLib result;
	if (iYadR)
		{
		iYadR->Accept(*this);
		std::swap(result, fOutput);
		}
	return result;
	}

ZVal_ZooLib sFromYadR(const ZVal_ZooLib& iDefault, ZRef<ZYadR> iYadR)
	{
	if (ZRef<ZYadR_ZooLib> theYadR = iYadR.DynamicCast<ZYadR_ZooLib>())
		return theYadR->GetVal();

	return Visitor_GetVal(iDefault).GetVal(iYadR);
	}

} // namespace ZooLib
