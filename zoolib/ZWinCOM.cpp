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

#include "zoolib/ZWinCOM.h"

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZCompat_algorithm.h"
#include "zoolib/ZUnicode.h"

using std::min;

namespace ZooLib {
namespace ZWinCOM {

// =================================================================================================
#pragma mark -
#pragma mark * Variant

/**
\class ZWinCOM::Variant
\ingroup ZVal

\brief Compatible with ZVal & VARIANT
*/

Variant::operator operator_bool() const
	{ return operator_bool_gen::translate(VT_NULL != vt); }

void Variant::swap(Variant& iOther)
	{ std::swap(static_cast<Variant&>(*this), static_cast<Variant&>(iOther)); }

Variant::Variant()
	{ ::VariantInit(this); }

Variant::Variant(const Variant& iOther)
	{
	::VariantInit(this);
	::VariantCopy(this, const_cast<Variant*>(&iOther));
	}

Variant::~Variant()
	{ ::VariantClear(this); }

Variant& Variant::operator=(const Variant& iOther)
	{
	Variant(iOther).swap(*this);
	return *this;
	}

Variant::Variant(const VARIANT& iOther)
	{
	::VariantInit(this);
	::VariantCopy(this, const_cast<VARIANT*>(&iOther));
	}

Variant& Variant::operator=(const VARIANT& iOther)
	{
	Variant(iOther).swap(*this);
	return *this;
	}

Variant::Variant(int8 iVal)
	{
	::VariantInit(this);
	vt = VT_I1;
	cVal = iVal;
	}

Variant::Variant(uint8 iVal)
	{
	::VariantInit(this);
	vt = VT_UI1;
	bVal = iVal;
	}

Variant::Variant(int16 iVal)
	{
	::VariantInit(this);
	vt = VT_I2;
	iVal = iVal;
	}

Variant::Variant(uint16 iVal)
	{
	::VariantInit(this);
	vt = VT_UI2;
	uiVal = iVal;
	}

Variant::Variant(int32 iVal)
	{
	::VariantInit(this);
	vt = VT_I4;
	lVal = iVal;
	}

Variant::Variant(uint32 iVal)
	{
	::VariantInit(this);
	vt = VT_UI4;
	ulVal = iVal;
	}

Variant::Variant(int64 iVal)
	{
	::VariantInit(this);
	vt = VT_I8;
	llVal = iVal;
	}

Variant::Variant(uint64 iVal)
	{
	::VariantInit(this);
	vt = VT_UI8;
	ullVal = iVal;
	}

Variant::Variant(bool iVal)
	{
	::VariantInit(this);
	vt = VT_BOOL;
	boolVal = iVal ? VARIANT_TRUE : VARIANT_FALSE;
	}

Variant::Variant(float iVal)
	{
	::VariantInit(this);
	vt = VT_R4;
	fltVal = iVal;
	}

Variant::Variant(double iVal)
	{
	::VariantInit(this);
	vt = VT_R8;
	dblVal = iVal;
	}

Variant::Variant(const string8& iVal)
	{
	::VariantInit(this);
	vt = VT_BSTR;
	const string16 asUTF16 = ZUnicode::sAsUTF16(iVal);
	bstrVal = ::SysAllocStringLen(asUTF16.c_str(), asUTF16.length());
	}

Variant::Variant(const string16& iVal)
	{
	::VariantInit(this);
	vt = VT_BSTR;
	bstrVal = ::SysAllocStringLen(iVal.c_str(), iVal.length());
	}

Variant::Variant(IUnknown* iVal)
	{
	::VariantInit(this);
	vt = VT_UNKNOWN;
	sRefCopy(&punkVal, iVal);
	}

Variant::Variant(IDispatch* iVal)
	{
	::VariantInit(this);
	vt = VT_DISPATCH;
	sRefCopy(&pdispVal, iVal);
	}

template <>
ZQ<int8> Variant::QGet<int8>() const
	{
	if (VT_I1 == vt)
		return int8(cVal);
	return null;
	}

template <>
ZQ<uint8> Variant::QGet<uint8>() const
	{
	if (VT_UI1 == vt)
		return uint8(bVal);
	return null;
	}

template <>
ZQ<int16> Variant::QGet<int16>() const
	{
	if (VT_I2 == vt)
		return int16(iVal);
	return null;
	}

template <>
ZQ<uint16> Variant::QGet<uint16>() const
	{
	if (VT_UI2 == vt)
		return uint16(uiVal);
	return null;
	}

template <>
ZQ<int32> Variant::QGet<int32>() const
	{
	if (VT_I4 == vt)
		return int32(lVal);
	return null;
	}

template <>
ZQ<uint32> Variant::QGet<uint32>() const
	{
	if (VT_UI4 == vt)
		return uint32(ulVal);
	return null;
	}

template <>
ZQ<int64> Variant::QGet<int64>() const
	{
	if (VT_I8 == vt)
		return int64(llVal);
	return null;
	}

template <>
ZQ<uint64> Variant::QGet<uint64>() const
	{
	if (VT_UI8 == vt)
		return uint64(ullVal);
	return null;
	}

template <>
ZQ<bool> Variant::QGet<bool>() const
	{
	if (VT_BOOL == vt)
		return bool(boolVal);
	return null;
	}

template <>
ZQ<float> Variant::QGet<float>() const
	{
	if (VT_R4 == vt)
		return float(fltVal);
	return null;
	}

template <>
ZQ<double> Variant::QGet<double>() const
	{
	if (VT_R8 == vt)
		return double(dblVal);
	return null;
	}

template <>
ZQ<string8> Variant::QGet<string8>() const
	{
	if (VT_BSTR == vt)
		return ZUnicode::sAsUTF8(bstrVal, ::SysStringLen(bstrVal));
	return null;
	}

template <>
ZQ<string16> Variant::QGet<string16>() const
	{
	if (VT_BSTR == vt)
		return string16(bstrVal, ::SysStringLen(bstrVal));
	return null;
	}

template <>
ZQ<ZRef<IUnknown> > Variant::QGet<ZRef<IUnknown> >() const
	{
	if (VT_UNKNOWN == vt)
		return ZRef<IUnknown>(punkVal);
	return null;
	}

template <>
ZQ<ZRef<IDispatch> > Variant::QGet<ZRef<IDispatch> >() const
	{
	if (VT_DISPATCH == vt)
		return ZRef<IDispatch>(pdispVal);
	return null;
	}

template <>
void Variant::Set<int8>(const int8& iVal)
	{
	::VariantClear(this);
	vt = VT_I1;
	cVal = iVal;
	}

template <>
void Variant::Set<uint8>(const uint8& iVal)
	{
	::VariantClear(this);
	vt = VT_UI1;
	bVal = iVal;
	}

template <>
void Variant::Set<int16>(const int16& iVal)
	{
	::VariantClear(this);
	vt = VT_I2;
	this->iVal = iVal;
	}

template <>
void Variant::Set<uint16>(const uint16& iVal)
	{
	::VariantClear(this);
	vt = VT_UI2;
	uiVal = iVal;
	}

template <>
void Variant::Set<int32>(const int32& iVal)
	{
	::VariantClear(this);
	vt = VT_I4;
	lVal = iVal;
	}

template <>
void Variant::Set<uint32>(const uint32& iVal)
	{
	::VariantClear(this);
	vt = VT_UI4;
	ulVal = iVal;
	}

template <>
void Variant::Set<int64>(const int64& iVal)
	{
	::VariantClear(this);
	vt = VT_I8;
	llVal = iVal;
	}

template <>
void Variant::Set<uint64>(const uint64& iVal)
	{
	::VariantClear(this);
	vt = VT_UI8;
	ullVal = iVal;
	}

template <>
void Variant::Set<bool>(const bool& iVal)
	{
	::VariantClear(this);
	vt = VT_BOOL;
	boolVal = iVal ? VARIANT_TRUE : VARIANT_FALSE;
	}

template <>
void Variant::Set<float>(const float& iVal)
	{
	::VariantClear(this);
	vt = VT_R4;
	fltVal = iVal;
	}

template <>
void Variant::Set<double>(const double& iVal)
	{
	::VariantClear(this);
	vt = VT_R8;
	dblVal = iVal;
	}

template <>
void Variant::Set<string8>(const string8& iVal)
	{
	::VariantClear(this);
	vt = VT_BSTR;
	const string16 asUTF16 = ZUnicode::sAsUTF16(iVal);
	bstrVal = ::SysAllocStringLen(asUTF16.c_str(), asUTF16.length());
	}

template <>
void Variant::Set<string16>(const string16& iVal)
	{
	::VariantClear(this);
	vt = VT_BSTR;
	bstrVal = ::SysAllocStringLen(iVal.c_str(), iVal.length());
	}

template <>
void Variant::Set<ZRef<IUnknown> >(const ZRef<IUnknown>& iVal)
	{
	::VariantClear(this);
	vt = VT_UNKNOWN;
	sRefCopy(&punkVal, iVal.Get());
	}

template <>
void Variant::Set<ZRef<IDispatch> >(const ZRef<IDispatch>& iVal)
	{
	::VariantClear(this);
	vt = VT_DISPATCH;
	sRefCopy(&pdispVal, iVal.Get());
	}

VARIANT& Variant::OParam()
	{
	::VariantClear(this);
	return *this;
	}

/// \cond DoxygenIgnore
ZMACRO_ZValAccessors_Def_Entry(Variant, Int8, int8)
ZMACRO_ZValAccessors_Def_Entry(Variant, UInt, uint8)
ZMACRO_ZValAccessors_Def_Entry(Variant, Int16, int16)
ZMACRO_ZValAccessors_Def_Entry(Variant, UInt16, uint16)
ZMACRO_ZValAccessors_Def_Entry(Variant, Int32, int32)
ZMACRO_ZValAccessors_Def_Entry(Variant, UInt32, uint32)
ZMACRO_ZValAccessors_Def_Entry(Variant, Int64, int64)
ZMACRO_ZValAccessors_Def_Entry(Variant, UInt64, uint64)
ZMACRO_ZValAccessors_Def_Entry(Variant, Bool, bool)
ZMACRO_ZValAccessors_Def_Entry(Variant, Float, float)
ZMACRO_ZValAccessors_Def_Entry(Variant, Double, double)
ZMACRO_ZValAccessors_Def_Entry(Variant, String, string8)
ZMACRO_ZValAccessors_Def_Entry(Variant, String8, string8)
ZMACRO_ZValAccessors_Def_Entry(Variant, String16, string16)
ZMACRO_ZValAccessors_Def_Entry(Variant, Unknown, ZRef<IUnknown>)
ZMACRO_ZValAccessors_Def_Entry(Variant, Dispatch, ZRef<IDispatch>)
/// \endcond DoxygenIgnore

// =================================================================================================
#pragma mark -
#pragma mark * String

String::String()
:	fBSTR(nullptr)
	{}

String::String(const String& iOther)
:	fBSTR(nullptr)
	{
	if (iOther.fBSTR)
		{
		if (size_t sourceLength = ::SysStringLen(iOther.fBSTR))
			fBSTR = ::SysAllocStringLen(iOther.fBSTR, sourceLength);
		}
	}

String::~String()
	{
	if (fBSTR)
		::SysFreeString(fBSTR);
	}

String& String::operator=(const String& iOther)
	{
	BSTR newBSTR = nullptr;
	if (iOther.fBSTR)
		{
		if (size_t sourceLength = ::SysStringLen(iOther.fBSTR))
			newBSTR = ::SysAllocStringLen(iOther.fBSTR, sourceLength);
		}

	if (fBSTR)
		::SysFreeString(fBSTR);
	fBSTR = newBSTR;

	return *this;
	}

String::String(const BSTR& iOther)
:	fBSTR(nullptr)
	{
	if (iOther)
		{
		if (size_t sourceLength = ::SysStringLen(iOther))
			fBSTR = ::SysAllocStringLen(iOther, sourceLength);
		}
	}

String& String::operator=(const BSTR& iOther)
	{
	BSTR newBSTR = nullptr;
	if (iOther)
		{
		if (size_t sourceLength = ::SysStringLen(iOther))
			newBSTR = ::SysAllocStringLen(iOther, sourceLength);
		}

	if (fBSTR)
		::SysFreeString(fBSTR);
	fBSTR = newBSTR;

	return *this;
	}

String::String(const string16& iOther)
:	fBSTR(nullptr)
	{
	if (size_t sourceLength = iOther.length())
		fBSTR = ::SysAllocStringLen(iOther.data(), sourceLength);
	}

String& String::operator=(const string16& iOther)
	{
	BSTR newBSTR = nullptr;
	if (size_t sourceLength = iOther.length())
		newBSTR = ::SysAllocStringLen(iOther.data(), sourceLength);

	if (fBSTR)
		::SysFreeString(fBSTR);
	fBSTR = newBSTR;

	return *this;
	}

String::String(const string8& iOther)
:	fBSTR(nullptr)
	{
	if (! iOther.empty())
		{
		const string16 other = ZUnicode::sAsUTF16(iOther);
		fBSTR = ::SysAllocStringLen(other.data(), other.length());
		}
	}

String& String::operator=(const string8& iOther)
	{
	BSTR newBSTR = nullptr;
	if (! iOther.empty())
		{
		const string16 other = ZUnicode::sAsUTF16(iOther);
		newBSTR = ::SysAllocStringLen(other.data(), other.length());
		}

	if (fBSTR)
		::SysFreeString(fBSTR);
	fBSTR = newBSTR;

	return *this;
	}

String::operator string16() const
	{
	if (fBSTR)
		return string16(fBSTR, ::SysStringLen(fBSTR));
	return string16();
	}

String::operator string8() const
	{
	if (fBSTR)
		return ZUnicode::sAsUTF8(fBSTR, ::SysStringLen(fBSTR));
	return string8();
	}

void String::Clear()
	{
	if (fBSTR)
		{
		::SysFreeString(fBSTR);
		fBSTR = nullptr;
		}
	}

String::operator const BSTR&() const
	{ return fBSTR; }

BSTR& String::OParam()
	{
	if (fBSTR)
		{
		::SysFreeString(fBSTR);
		fBSTR = nullptr;
		}
	return fBSTR;
	}

const BSTR& String::IParam() const
	{ return fBSTR; }

BSTR* sCOMPtr(String& iStr)
	{ return &iStr.OParam(); }

bool operator==(const BSTR& iBSTR, const string16& iString)
	{
	const size_t theLength = ::SysStringLen(iBSTR);
	if (theLength == iString.length())
		return 0 == iString.compare(0, theLength, (UTF16*)iBSTR);
	return false;
	}

bool operator==(const string16& iString, const BSTR& iBSTR)
	{
	const size_t theLength = ::SysStringLen(iBSTR);
	if (theLength == iString.length())
		return 0 == iString.compare(0, theLength, (UTF16*)iBSTR);
	return false;
	}

bool operator==(const String& l, const string16& r)
	{ return l.IParam() == r; };

bool operator==(const string16& l, const String& r)
	{ return l == r.IParam(); };

} // namespace ZWinCOM
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)
