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

#include "zoolib/ZFunctionChain.h"
#include "zoolib/ZYad_JSONNormalize.h"
#include "zoolib/ZYad_StdMore.h"

NAMESPACE_ZOOLIB_BEGIN

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListR_JSONNormalize declaration

class ZYadListR_JSONNormalize : public ZYadListR_Std
	{
public:
	ZYadListR_JSONNormalize(ZRef<ZYadListR> iYadListR, bool iPreserveLists, bool iPreserveMaps);

// From ZYadListR_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZRef<ZYadR>& oYadR);

private:
	ZRef<ZYadListR> fYadListR;
	bool fPreserveLists;
	bool fPreserveMaps;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_JSONNormalize declaration

class ZYadMapR_JSONNormalize : public ZYadMapR_Std
	{
public:
	ZYadMapR_JSONNormalize(ZRef<ZYadMapR> iYadMapR, bool iPreserveLists, bool iPreserveMaps);

// From ZYadMapR_Std
	virtual void Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR>& oYadR);

private:
	ZRef<ZYadMapR> fYadMapR;
	bool fPreserveLists;
	bool fPreserveMaps;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

static bool spNormalizeSimpleValue(const ZAny& iVal, ZAny& oVal)
	{
	if (false)
		{}
	else if (iVal.type() == typeid(void)
		|| ZAnyCast<int64>(&iVal)
		|| ZAnyCast<double>(&iVal)
		|| ZAnyCast<bool>(&iVal)
		|| ZAnyCast<string>(&iVal))
		{
		oVal = iVal;
		}
	else if (const int8* theValue = ZAnyCast<int8>(&iVal))
		{
		oVal = int64(*theValue);
		}
	else if (const int16* theValue = ZAnyCast<int16>(&iVal))
		{
		oVal = int64(*theValue);
		}
	else if (const int32* theValue = ZAnyCast<int32>(&iVal))
		{
		oVal = int64(*theValue);
		}
	else if (const float* theValue = ZAnyCast<float>(&iVal))
		{
		oVal = double(*theValue);
		}
	else if (const uint64* theValue = ZAnyCast<uint64>(&iVal))
		{
		oVal = int64(*theValue);
		}
	else if (const ZTime* theValue = ZAnyCast<ZTime>(&iVal))
		{
		oVal = double(theValue->fVal);
		}
	else
		{
		return false;
		}
	return true;
	}

static ZRef<ZYadR> spMakeYadR_JSONNormalize(
	ZRef<ZYadR> iYadR, bool iPreserve, bool iPreserveLists, bool iPreserveMaps)
	{
	if (ZRef<ZYadListR> theYadListR = ZRefDynamicCast<ZYadListR>(iYadR))
		{
		return new ZYadListR_JSONNormalize(theYadListR, iPreserveLists, iPreserveMaps);
		}
	else if (ZRef<ZYadMapR> theYadMapR = ZRefDynamicCast<ZYadMapR>(iYadR))
		{
		return new ZYadMapR_JSONNormalize(theYadMapR, iPreserveLists, iPreserveMaps);
		}
	else if (ZRef<ZYadStrimR> theYadStrimR = ZRefDynamicCast<ZYadStrimR>(iYadR))
		{
		return theYadStrimR;
		}
	else if (ZRef<ZYadPrimR> theYadPrimR = ZRefDynamicCast<ZYadPrimR>(iYadR))
		{
		ZAny normalized;
		if (spNormalizeSimpleValue(theYadPrimR->AsAny(), normalized))
			return new ZYadPrimR_Std(normalized);
		}

	if (iPreserve)
		{
		// We weren't able to normalize, but we've been
		// asked to preserve values, so return a null.
		return new ZYadPrimR_Std(ZAny());
		}

	return ZRef<ZYadPrimR_Std>();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListR_JSONNormalize

ZYadListR_JSONNormalize::ZYadListR_JSONNormalize(
	ZRef<ZYadListR> iYadListR, bool iPreserveLists, bool iPreserveMaps)
:	fYadListR(iYadListR),
	fPreserveLists(iPreserveLists),
	fPreserveMaps(iPreserveMaps)
	{}

void ZYadListR_JSONNormalize::Imp_ReadInc(bool iIsFirst, ZRef<ZYadR>& oYadR)
	{
	for (;;)
		{
		ZRef<ZYadR> next = fYadListR->ReadInc();
		if (!next)
			break;
		oYadR = spMakeYadR_JSONNormalize(
			next, fPreserveLists, fPreserveLists, fPreserveMaps);
		if (oYadR)
			break;
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_JSONNormalize

ZYadMapR_JSONNormalize::ZYadMapR_JSONNormalize(
	ZRef<ZYadMapR> iYadMapR, bool iPreserveLists, bool iPreserveMaps)
:	fYadMapR(iYadMapR),
	fPreserveLists(iPreserveLists),
	fPreserveMaps(iPreserveMaps)
	{}

void ZYadMapR_JSONNormalize::Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR>& oYadR)
	{
	for (;;)
		{
		ZRef<ZYadR> next = fYadMapR->ReadInc(oName);
		if (!next)
			break;
		oYadR = spMakeYadR_JSONNormalize(
			next, fPreserveMaps, fPreserveLists, fPreserveMaps);
		if (oYadR)
			break;
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYad_JSONNormalize

ZRef<ZYadR> ZYad_JSONNormalize::sMakeYadR(
	ZRef<ZYadR> iYadR, bool iPreserveLists, bool iPreserveMaps)
	{ return spMakeYadR_JSONNormalize(iYadR, true, iPreserveLists, iPreserveMaps); }

NAMESPACE_ZOOLIB_END
