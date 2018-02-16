/* -------------------------------------------------------------------------------------------------
Copyright (c) 2017 Andrew Green
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

#include "zoolib/Chan_XX_Count.h"
#include "zoolib/Chan_XX_Limited.h"
#include "zoolib/ChanR_Bin_More.h"
#include "zoolib/ChanW_Bin_More.h"
#include "zoolib/Log.h"
#include "zoolib/NameUniquifier.h" // For sName
#include "zoolib/Stringf.h"
#include "zoolib/Util_Any.h"
#include "zoolib/Yad_Any.h"
#include "zoolib/Yad_ASN1.h"
#include "zoolib/Yad_Std.h"

#include <map>
#include <set>
#include <vector>

#include "zoolib/ZMACRO_foreach.h"

namespace ZooLib {
namespace Yad_ASN1 {

using std::min;
using std::set;
using std::string;
using std::vector;

namespace { // anonymous

// =================================================================================================
#pragma mark -
#pragma mark spComplain

void spComplain(bool iCanRecover, std::string iString, va_list iArgs)
	{
	const std::string result = sStringv(iString.c_str(), iArgs);
	if (ZLOGF(w, eInfo))
		w << result;
	if (not iCanRecover)
		throw std::runtime_error(result);
	}

void spComplain(bool iCanRecover, std::string iString, ...)
	{
	va_list args;
	va_start(args, iString);
	spComplain(iCanRecover, iString, args);
	va_end(args);
	}

void spFail(std::string iString, ...)
	{
	va_list args;
	va_start(args, iString);
	spComplain(false, iString, args);
	va_end(args);
	}

// =================================================================================================
#pragma mark -
#pragma mark *

enum ETagClass
	{
	eTagClass_Universal = 0,
	eTagClass_Application = 1,
	eTagClass_ContextSpecific = 2,
	eTagClass_Private = 3,
	};

string spAsString(ETagClass iTagClass)
	{
	switch(iTagClass)
		{
		case eTagClass_Universal: return "Universal";
		case eTagClass_Application: return "Application";
		case eTagClass_ContextSpecific: return "Context";
		case eTagClass_Private: return "Private";
		}
	return "Invalid TagClass";
	}

enum ETagNumber_Universal
	{
	eTagNumber_Universal_EOC = 0x0,
	eTagNumber_Universal_BOOLEAN = 0x1,
	eTagNumber_Universal_INTEGER = 0x2,
	eTagNumber_Universal_BIT_STRING = 0x3,
	eTagNumber_Universal_OCTET_STRING = 0x4,
	eTagNumber_Universal_NULL = 0x5,
	eTagNumber_Universal_OBJECT_IDENTIFIER = 0x6,
	eTagNumber_Universal_ObjectDescriptor = 0x7,
	eTagNumber_Universal_EXTERNAL = 0x8,
	eTagNumber_Universal_REAL = 0x9,
	eTagNumber_Universal_ENUMERATED = 0xA,
	eTagNumber_Universal_EMBEDDED_PDV = 0xB,
	eTagNumber_Universal_UTF8String = 0xC,
	eTagNumber_Universal_RELATIVE_OID = 0xD,

	eTagNumber_Universal_SEQUENCE = 0x10,
	eTagNumber_Universal_SET = 0x11,
	eTagNumber_Universal_NumericString = 0x12,
	eTagNumber_Universal_PrintableString = 0x13,
	eTagNumber_Universal_T61String = 0x14,
	eTagNumber_Universal_VideotexString = 0x15,
	eTagNumber_Universal_IA5String = 0x16,
	eTagNumber_Universal_UTCTime = 0x17,
	eTagNumber_Universal_GeneralizedTime = 0x18,
	eTagNumber_Universal_GraphicString = 0x19,
	eTagNumber_Universal_VisibleString = 0x1A,
	eTagNumber_Universal_GeneralString = 0x1B,
	eTagNumber_Universal_UniversalString = 0x1C,
	eTagNumber_Universal_CHARACTER_STRING = 0x1D,
	eTagNumber_Universal_BMPString = 0x1E
	};

string spAsString(ETagNumber_Universal iTagNumber)
	{
	switch(iTagNumber)
		{
		case eTagNumber_Universal_EOC: return "End-of-contents octets";
		case eTagNumber_Universal_BOOLEAN: return "BOOLEAN";
		case eTagNumber_Universal_INTEGER: return "INTEGER";
		case eTagNumber_Universal_BIT_STRING: return "BIT STRING";
		case eTagNumber_Universal_OCTET_STRING: return "OCTET STRING";
		case eTagNumber_Universal_NULL: return "NULL";
		case eTagNumber_Universal_OBJECT_IDENTIFIER: return "OBJECT IDENTIFIER";
		case eTagNumber_Universal_ObjectDescriptor: return "ObjectDescriptor";
		case eTagNumber_Universal_EXTERNAL: return "EXTERNAL";
		case eTagNumber_Universal_REAL: return "REAL";
		case eTagNumber_Universal_ENUMERATED: return "ENUMERATED";
		case eTagNumber_Universal_EMBEDDED_PDV: return "EMBEDDED PDV";
		case eTagNumber_Universal_UTF8String: return "UTF8String";
		case eTagNumber_Universal_RELATIVE_OID: return "RELATIVE_OID";

		case eTagNumber_Universal_SEQUENCE: return "SEQUENCE";
		case eTagNumber_Universal_SET: return "SET";
		case eTagNumber_Universal_NumericString: return "NumericString";
		case eTagNumber_Universal_PrintableString: return "PrintableString";
		case eTagNumber_Universal_T61String: return "TeletexString";
		case eTagNumber_Universal_VideotexString: return "VideotexString";
		case eTagNumber_Universal_IA5String: return "IA5String";
		case eTagNumber_Universal_UTCTime: return "UTCTime";
		case eTagNumber_Universal_GeneralizedTime: return "GeneralizedTime";
		case eTagNumber_Universal_GraphicString: return "GraphicString";
		case eTagNumber_Universal_VisibleString: return "VisibleString";
		case eTagNumber_Universal_GeneralString: return "GeneralString";
		case eTagNumber_Universal_UniversalString: return "UniversalString";
		case eTagNumber_Universal_CHARACTER_STRING: return "CHARACTER_STRING";
		case eTagNumber_Universal_BMPString: return "BMPString";
		default: return sStringf("Unknown universal tag number (%d)", iTagNumber);
		}
	}

// =================================================================================================
#pragma mark -

ZRef<YadR> spMakeYadR(const ZRef<ChannerR_Bin>& iChannerR_Bin);

ZRef<YadR> spMakeYadR(const ZRef<ChannerR_Bin>& iChannerR_Bin,
	bool iConstructed, ETagClass iTagClass, uint64 uTagNumber, ZQ<uint64> iLengthQ);

void spReadHeader(const ChanR_Bin& r,
	bool& oConstructed, ETagClass& oTagClass, uint64& oTagNumber, ZQ<uint64>& oLengthQ)
	{
	const uint8 octet0 = sEReadBE<uint8>(r);

	oTagClass = ETagClass(octet0 >> 6);
	oTagNumber = octet0 & 0x1F;

	if (oTagNumber == 0x1F)
		{
		// It's an extended tag number.
		oTagNumber = 0;
		for (;;)
			{
			const uint8 nextOctet = sEReadBE<uint8>(r);
			oTagNumber = oTagNumber << 7;
			oTagNumber = oTagNumber | (nextOctet & 0x7F);
			// We've read at least one subsequent chunk of bits, and the value should be at least
			// 31, otherwise we'd have been able to fit it in octet0.
			if (not nextOctet & 0x80)
				break;
			}
		if (oTagNumber < 31);
			spComplain(true, "oTagNumber: %llu. Should be >= 31", oTagNumber);
		}

	const uint8 nextOctet = sEReadBE<uint8>(r);
	if (false)
		{}
	else if (nextOctet < 128)
		{
		oLengthQ = nextOctet;
		}
	else if (nextOctet == 128)
		{
		// Indefinite, do nothing.
		}
	else if (nextOctet == 255)
		{
		// Invalid, throw?
		spFail("Invalid length octet (255)");
		}
	else
		{
		const int count = nextOctet & 0x7F;
		if (count > 8)
			spFail("count: %d, should be <= 8", count);
		uint64 realLength = 0;
		for (int xx = 0; xx < count; ++xx)
			{
			realLength = realLength << 8;
			realLength = realLength | sEReadBE<uint8>(r);
			}
		if (realLength < 126)
			spComplain(true, "Length: %d, should be 126 or more", count);
		oLengthQ = realLength;
		}

	oConstructed = octet0 & 0x20;
	}

// =================================================================================================
#pragma mark -
#pragma mark ChanR_RefYad_Definite

// We return this for an definite universal SEQUENCE or SET, and as the value for a definite
// non-universal constructed.

class ChanR_RefYad_Definite
:	public ChanR_RefYad_Std
	{
public:
	ChanR_RefYad_Definite(const ZRef<Channer<ChanR_Bin>>& iChannerR_Bin , uint64 iLength)
	:	fChannerR_Count(sChanner_Channer_T<ChanR_XX_Count<ChanR_Bin>>(iChannerR_Bin))
	,	fLength(iLength)
		{}

// From YadSeqR_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZRef<YadR>& oYadR)
		{
		if (fChannerR_Count->GetCount() < fLength)
			oYadR = spMakeYadR(fChannerR_Count);
		}

private:
	const ZRef<Channer_T<ChanR_XX_Count<ChanR_Bin>>> fChannerR_Count;
	const uint64 fLength;
	};

// =================================================================================================
#pragma mark -
#pragma mark ChanR_RefYad_Indefinite

// We return this for an indefinite universal SEQUENCE or SET, and as the value for an indefinite
// non-universal constructed.

class ChanR_RefYad_Indefinite
:	public ChanR_RefYad_Std
	{
public:
	ChanR_RefYad_Indefinite(const ZRef<ChannerR_Bin>& iChannerR_Bin)
	:	fChannerR_Bin(iChannerR_Bin)
		{}

// From YadSeqR_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZRef<YadR>& oYadR)
		{
		bool constructed;
		ETagClass tagClass;
		uint64 tagNumber;
		ZQ<uint64> lengthQ;
		spReadHeader(*fChannerR_Bin, constructed, tagClass, tagNumber, lengthQ);

		if (tagClass == eTagClass_Universal && tagNumber == eTagNumber_Universal_EOC)
			return;

		oYadR = spMakeYadR(fChannerR_Bin, constructed, tagClass, tagNumber, lengthQ);
		}

private:
	const ZRef<ChannerR_Bin> fChannerR_Bin;
	};

// =================================================================================================
#pragma mark -
#pragma mark ChanR_NameRefYad_SingleChild

// We return this as the value of a non-universal constructed.
// It'll be a map with a single named element being a sequence.

class ChanR_NameRefYad_SingleChild
:	public ChanR_NameRefYad_Std
	{
public:
	ChanR_NameRefYad_SingleChild(string iName, ZRef<YadR> iChild)
	:	fName(iName)
	,	fChild(iChild)
		{}

// From YadMapR_Std
	virtual void Imp_ReadInc(bool iIsFirst, Name& oName, ZRef<YadR>& oYadR)
		{
		if (not iIsFirst)
			return;
		oName = fName;
		oYadR = fChild;
		}

private:
	const string fName;
	const ZRef<YadR> fChild;
	};

// =================================================================================================
#pragma mark -

ZRef<YadR> spMakeYadR_Constructed(const ZRef<ChannerR_Bin>& iChannerR_Bin,
	ETagClass iTagClass, uint64 iTagNumber, ZQ<uint64> iLengthQ)
	{
	if (iTagClass != eTagClass_Universal
		|| iTagNumber == eTagNumber_Universal_EXTERNAL
		|| iTagNumber == eTagNumber_Universal_EMBEDDED_PDV)
		{
		string theName = spAsString(iTagClass) + sStringf(" %llu", iTagNumber);
		ZRef<YadR> theChild;
		if (iLengthQ)
			theChild = sChanner_T<ChanR_RefYad_Definite>(iChannerR_Bin, *iLengthQ);
		else
			theChild = sChanner_T<ChanR_RefYad_Indefinite>(iChannerR_Bin);

		return sChanner_T<ChanR_NameRefYad_SingleChild>(theName, theChild);
		}

	// It's a constructed universal.
	switch (iTagNumber)
		{
		case eTagNumber_Universal_SEQUENCE:
		case eTagNumber_Universal_SET:
			{
			if (iLengthQ)
				return sChanner_T<ChanR_RefYad_Definite>(iChannerR_Bin, *iLengthQ);
			else
				return sChanner_T<ChanR_RefYad_Indefinite>(iChannerR_Bin);
			}
		}

	switch (iTagNumber)
		{
		case eTagNumber_Universal_BIT_STRING:
		case eTagNumber_Universal_OCTET_STRING:
		case eTagNumber_Universal_ObjectDescriptor:
		case eTagNumber_Universal_UTF8String:
		case eTagNumber_Universal_NumericString:
		case eTagNumber_Universal_PrintableString:
		case eTagNumber_Universal_T61String:
		case eTagNumber_Universal_VideotexString:
		case eTagNumber_Universal_IA5String:
		case eTagNumber_Universal_UTCTime:
		case eTagNumber_Universal_GeneralizedTime:
		case eTagNumber_Universal_GraphicString:
		case eTagNumber_Universal_VisibleString:
		case eTagNumber_Universal_GeneralString:
		case eTagNumber_Universal_UniversalString:
		case eTagNumber_Universal_CHARACTER_STRING:
		case eTagNumber_Universal_BMPString:
			ZLOGTRACE(eDebug);
			break;
//			return a stream that concatenates the component entities till we hit primitives, and errors
//			when we encounter something that's not an iTagNumber.
		}

	// Otherwise it is somethat that *must* be primitive, so we fail?

	return null;
	}

ZRef<YadR> spMakeYadR_Primitive(const ZRef<ChannerR_Bin>& iChannerR_Bin,
	ETagClass iTagClass, uint64 iTagNumber, uint64 iLength)
	{
	if (iTagClass != eTagClass_Universal)
		{
		string theName = spAsString(iTagClass) + sStringf(" %llu", iTagNumber);
		ZRef<YadR> theChild = sChanner_Channer_T<ChanR_XX_Limited<byte>>(iLength, iChannerR_Bin);
		return sChanner_T<ChanR_NameRefYad_SingleChild>(theName, theChild);
		}

	switch (iTagNumber)
		{
		case eTagNumber_Universal_EXTERNAL:
		case eTagNumber_Universal_EMBEDDED_PDV:
		case eTagNumber_Universal_SEQUENCE:
		case eTagNumber_Universal_SET:
			{
			const string theName = "Universal "
				+ spAsString(ETagNumber_Universal(iTagNumber))
				+ sStringf(" (%llu))", iTagNumber)
				+ "should not be primitive";
			ZRef<YadR> theChild = sChanner_Channer_T<ChanR_XX_Limited<byte>>(iLength, iChannerR_Bin);
			return sChanner_T<ChanR_NameRefYad_SingleChild>(theName, theChild);
			}
		}

	switch (iTagNumber)
		{
		case eTagNumber_Universal_BIT_STRING:
		case eTagNumber_Universal_OCTET_STRING:
		case eTagNumber_Universal_ObjectDescriptor:
		case eTagNumber_Universal_UTF8String:
		case eTagNumber_Universal_NumericString:
		case eTagNumber_Universal_PrintableString:
		case eTagNumber_Universal_T61String:
		case eTagNumber_Universal_VideotexString:
		case eTagNumber_Universal_IA5String:
		case eTagNumber_Universal_UTCTime:
		case eTagNumber_Universal_GeneralizedTime:
		case eTagNumber_Universal_GraphicString:
		case eTagNumber_Universal_VisibleString:
		case eTagNumber_Universal_GeneralString:
		case eTagNumber_Universal_UniversalString:
		case eTagNumber_Universal_CHARACTER_STRING:
		case eTagNumber_Universal_BMPString:
			{
			// For now return as raw bytes. Later we'll stick appropriate interpretation on them
			return sChanner_Channer_T<ChanR_XX_Limited<byte>>(iLength, iChannerR_Bin);
			}
		}

	switch (iTagNumber)
		{
		case eTagNumber_Universal_EOC:
			{
			// Should probably have been handled by an enclosing constructed element
			break;
			}
		case eTagNumber_Universal_NULL:
			{
			return ZooLib::sYadR(Any());
			}
		case eTagNumber_Universal_BOOLEAN:
			{
			if (iLength != 1)
				spComplain(true, "BOOLEAN should be exactly one byte");
			const uint8 octet = sEReadBE<uint8>(*iChannerR_Bin);
			sSkip(*iChannerR_Bin, iLength - 1);
			if (octet == 0)
				return ZooLib::sYadR(Any(false));

			if (octet != 0xFF)
				spComplain(true, "BOOLEAN value should be 0 or 0xFF, got %X", octet);
			return ZooLib::sYadR(Any(true));
			}
		case eTagNumber_Universal_INTEGER:
		case eTagNumber_Universal_OBJECT_IDENTIFIER:
		case eTagNumber_Universal_REAL:
		case eTagNumber_Universal_ENUMERATED:
		case eTagNumber_Universal_RELATIVE_OID:
			{
			string theName = spAsString(ETagNumber_Universal(iTagNumber));
			ZRef<YadR> theChild = sChanner_Channer_T<ChanR_XX_Limited<byte>>(iLength, iChannerR_Bin);
			return sChanner_T<ChanR_NameRefYad_SingleChild>(theName, theChild);
			}
		}

	ZAssert(false);

	return null;
	}

// =================================================================================================
#pragma mark -

ZRef<YadR> spMakeYadR(const ZRef<ChannerR_Bin>& iChannerR_Bin,
	bool iConstructed, ETagClass iTagClass, uint64 uTagNumber, ZQ<uint64> iLengthQ)
	{
	if (iConstructed)
		return spMakeYadR_Constructed(iChannerR_Bin, iTagClass, uTagNumber, iLengthQ);

	if (not iLengthQ)
		spFail("Element is primitive, but length is indefinite.");

	return spMakeYadR_Primitive(iChannerR_Bin, iTagClass, uTagNumber, *iLengthQ);
	}

ZRef<YadR> spMakeYadR(const ZRef<ChannerR_Bin>& iChannerR_Bin)
	{
	bool constructed;
	ETagClass tagClass;
	uint64 tagNumber;
	ZQ<uint64> lengthQ;
	spReadHeader(*iChannerR_Bin, constructed, tagClass, tagNumber, lengthQ);
	return spMakeYadR(iChannerR_Bin, constructed, tagClass, tagNumber, lengthQ);
	}

// =================================================================================================
#pragma mark -
#pragma mark Visitor_Writer
} // anonymous namespace

ZRef<YadR> sYadR(ZRef<ChannerR_Bin> iChannerR_Bin)
	{ return spMakeYadR(iChannerR_Bin); }

} // namespace Yad_ASN1
} // namespace ZooLib
