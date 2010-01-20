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

#include "zoolib/ZTime.h"
#include "zoolib/ZYad_JSONNormalize.h"
#include "zoolib/ZYad_Std.h"

NAMESPACE_ZOOLIB_BEGIN

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * ZYadSeqR_JSONNormalize declaration

class ZYadSeqR_JSONNormalize : public ZYadSeqR_Std
	{
public:
	ZYadSeqR_JSONNormalize(ZRef<ZYadSeqR> iYadSeqR, bool iPreserveSeqs, bool iPreserveMaps);

// From ZYadSeqR_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZRef<ZYadR>& oYadR);

private:
	ZRef<ZYadSeqR> fYadSeqR;
	bool fPreserveSeqs;
	bool fPreserveMaps;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_JSONNormalize declaration

class ZYadMapR_JSONNormalize : public ZYadMapR_Std
	{
public:
	ZYadMapR_JSONNormalize(ZRef<ZYadMapR> iYadMapR, bool iPreserveSeqs, bool iPreserveMaps);

// From ZYadMapR_Std
	virtual void Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR>& oYadR);

private:
	ZRef<ZYadMapR> fYadMapR;
	bool fPreserveSeqs;
	bool fPreserveMaps;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

static bool spNormalizeSimpleValue(const ZAny& iVal, ZAny& oVal)
	{
	int64 asInt64;
	double asDouble;

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
	else if (sQCoerceInt(iVal, asInt64))
		{
		oVal = asInt64;
		}
	else if (sQCoerceReal(iVal, asDouble))
		{
		oVal = asDouble;
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
	ZRef<ZYadR> iYadR, bool iPreserve, bool iPreserveSeqs, bool iPreserveMaps)
	{
	if (ZRef<ZYadSeqR> theYadSeqR = ZRefDynamicCast<ZYadSeqR>(iYadR))
		{
		return new ZYadSeqR_JSONNormalize(theYadSeqR, iPreserveSeqs, iPreserveMaps);
		}
	else if (ZRef<ZYadMapR> theYadMapR = ZRefDynamicCast<ZYadMapR>(iYadR))
		{
		return new ZYadMapR_JSONNormalize(theYadMapR, iPreserveSeqs, iPreserveMaps);
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
#pragma mark * ZYadSeqR_JSONNormalize

ZYadSeqR_JSONNormalize::ZYadSeqR_JSONNormalize(
	ZRef<ZYadSeqR> iYadSeqR, bool iPreserveSeqs, bool iPreserveMaps)
:	fYadSeqR(iYadSeqR),
	fPreserveSeqs(iPreserveSeqs),
	fPreserveMaps(iPreserveMaps)
	{}

void ZYadSeqR_JSONNormalize::Imp_ReadInc(bool iIsFirst, ZRef<ZYadR>& oYadR)
	{
	for (;;)
		{
		ZRef<ZYadR> next = fYadSeqR->ReadInc();
		if (!next)
			break;
		oYadR = spMakeYadR_JSONNormalize(
			next, fPreserveSeqs, fPreserveSeqs, fPreserveMaps);
		if (oYadR)
			break;
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_JSONNormalize

ZYadMapR_JSONNormalize::ZYadMapR_JSONNormalize(
	ZRef<ZYadMapR> iYadMapR, bool iPreserveSeqs, bool iPreserveMaps)
:	fYadMapR(iYadMapR),
	fPreserveSeqs(iPreserveSeqs),
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
			next, fPreserveMaps, fPreserveSeqs, fPreserveMaps);
		if (oYadR)
			break;
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYad_JSONNormalize

ZRef<ZYadR> ZYad_JSONNormalize::sMakeYadR(
	ZRef<ZYadR> iYadR, bool iPreserveSeqs, bool iPreserveMaps)
	{ return spMakeYadR_JSONNormalize(iYadR, true, iPreserveSeqs, iPreserveMaps); }

NAMESPACE_ZOOLIB_END
