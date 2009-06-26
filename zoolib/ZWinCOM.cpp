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
using std::string;

NAMESPACE_ZOOLIB_BEGIN

namespace ZWinCOM {

// =================================================================================================
#pragma mark -
#pragma mark * Variant

Variant::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(VT_NULL != vt); }

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

Variant::Variant(const std::string& iVal)
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

Variant::Variant(ZRef<IUnknown> iVal)
	{
	::VariantInit(this);
	vt = VT_UNKNOWN;
	sRefCopy(&punkVal, iVal.Get());
	}

Variant::Variant(ZRef<IDispatch> iVal)
	{
	::VariantInit(this);
	vt = VT_DISPATCH;
	sRefCopy(&pdispVal, iVal.Get());
	}

VARIANT* Variant::ParamO()
	{
	::VariantClear(this);
	return this;
	}

template <>
bool Variant::QGet_T<int8>(int8& oVal) const
	{
	if (VT_I1 == vt)
		{
		oVal = cVal;
		return true;
		}
	return false;
	}

template <>
bool Variant::QGet_T<uint8>(uint8& oVal) const
	{
	if (VT_UI1 == vt)
		{
		oVal = bVal;
		return true;
		}
	return false;
	}

template <>
bool Variant::QGet_T<int16>(int16& oVal) const
	{
	if (VT_I2 == vt)
		{
		oVal = iVal;
		return true;
		}
	return false;
	}

template <>
bool Variant::QGet_T<uint16>(uint16& oVal) const
	{
	if (VT_UI2 == vt)
		{
		oVal = uiVal;
		return true;
		}
	return false;
	}

template <>
bool Variant::QGet_T<int32>(int32& oVal) const
	{
	if (VT_I4 == vt)
		{
		oVal = lVal;
		return true;
		}
	return false;
	}

template <>
bool Variant::QGet_T<uint32>(uint32& oVal) const
	{
	if (VT_UI4 == vt)
		{
		oVal = ulVal;
		return true;
		}
	return false;
	}

template <>
bool Variant::QGet_T<int64>(int64& oVal) const
	{
	if (VT_I8 == vt)
		{
		oVal = llVal;
		return true;
		}
	return false;
	}

template <>
bool Variant::QGet_T<uint64>(uint64& oVal) const
	{
	if (VT_UI8 == vt)
		{
		oVal = ullVal;
		return true;
		}
	return false;
	}

template <>
bool Variant::QGet_T<bool>(bool& oVal) const
	{
	if (VT_BOOL == vt)
		{
		oVal = boolVal;
		return true;
		}
	return false;
	}

template <>
bool Variant::QGet_T<float>(float& oVal) const
	{
	if (VT_R4 == vt)
		{
		oVal = fltVal;
		return true;
		}
	return false;
	}

template <>
bool Variant::QGet_T<double>(double& oVal) const
	{
	if (VT_R8 == vt)
		{
		oVal = dblVal;
		return true;
		}
	return false;
	}

template <>
bool Variant::QGet_T<string>(string& oVal) const
	{
	if (VT_BSTR == vt)
		{
		oVal = ZUnicode::sAsUTF8(bstrVal, ::SysStringLen(bstrVal));
		return true;
		}
	return false;
	}

template <>
bool Variant::QGet_T<string16>(string16& oVal) const
	{
	if (VT_BSTR == vt)
		{
		oVal = string16(bstrVal, ::SysStringLen(bstrVal));
		return true;
		}
	return false;
	}

template <>
bool Variant::QGet_T<ZRef<IUnknown> >(ZRef<IUnknown>& oVal) const
	{
	if (VT_UNKNOWN == vt)
		{
		oVal = punkVal;
		return true;
		}
	return false;
	}

template <>
bool Variant::QGet_T<ZRef<IDispatch> >(ZRef<IDispatch>& oVal) const
	{
	if (VT_DISPATCH == vt)
		{
		oVal = pdispVal;
		return true;
		}
	return false;
	}

template <>
void Variant::Set_T<int8>(const int8& iVal)
	{
	::VariantClear(this);
	vt = VT_I1;
	cVal = iVal;
	}

template <>
void Variant::Set_T<uint8>(const uint8& iVal)
	{
	::VariantClear(this);
	vt = VT_UI1;
	bVal = iVal;
	}

template <>
void Variant::Set_T<int16>(const int16& iVal)
	{
	::VariantClear(this);
	vt = VT_I2;
	this->iVal = iVal;
	}

template <>
void Variant::Set_T<uint16>(const uint16& iVal)
	{
	::VariantClear(this);
	vt = VT_UI2;
	uiVal = iVal;
	}

template <>
void Variant::Set_T<int32>(const int32& iVal)
	{
	::VariantClear(this);
	vt = VT_I4;
	lVal = iVal;
	}

template <>
void Variant::Set_T<uint32>(const uint32& iVal)
	{
	::VariantClear(this);
	vt = VT_UI4;
	ulVal = iVal;
	}

template <>
void Variant::Set_T<int64>(const int64& iVal)
	{
	::VariantClear(this);
	vt = VT_I8;
	llVal = iVal;
	}

template <>
void Variant::Set_T<uint64>(const uint64& iVal)
	{
	::VariantClear(this);
	vt = VT_UI8;
	ullVal = iVal;
	}

template <>
void Variant::Set_T<bool>(const bool& iVal)
	{
	::VariantClear(this);
	vt = VT_BOOL;
	boolVal = iVal ? VARIANT_TRUE : VARIANT_FALSE;
	}

template <>
void Variant::Set_T<float>(const float& iVal)
	{
	::VariantClear(this);
	vt = VT_R4;
	fltVal = iVal;
	}

template <>
void Variant::Set_T<double>(const double& iVal)
	{
	::VariantClear(this);
	vt = VT_R8;
	dblVal = iVal;
	}

template <>
void Variant::Set_T<string>(const string& iVal)
	{
	::VariantClear(this);
	vt = VT_BSTR;
	const string16 asUTF16 = ZUnicode::sAsUTF16(iVal);
	bstrVal = ::SysAllocStringLen(asUTF16.c_str(), asUTF16.length());
	}

template <>
void Variant::Set_T<string16>(const string16& iVal)
	{
	::VariantClear(this);
	vt = VT_BSTR;
	bstrVal = ::SysAllocStringLen(iVal.c_str(), iVal.length());
	}

template <>
void Variant::Set_T<ZRef<IUnknown> >(const ZRef<IUnknown>& iVal)
	{
	::VariantClear(this);
	vt = VT_UNKNOWN;
	sRefCopy(&punkVal, iVal.Get());
	}

template <>
void Variant::Set_T<ZRef<IDispatch> >(const ZRef<IDispatch>& iVal)
	{
	::VariantClear(this);
	vt = VT_DISPATCH;
	sRefCopy(&pdispVal, iVal.Get());
	}

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

String::operator string16() const
	{
	if (fBSTR)
		return string16(fBSTR, ::SysStringLen(fBSTR));
	return string16();
	}

void String::Clear()
	{
	if (fBSTR)
		::SysFreeString(fBSTR);
	fBSTR = nullptr;
	}

BSTR& String::GetPtrRef()
	{ return fBSTR; }

const BSTR& String::GetPtrRef() const
	{ return fBSTR; }

BSTR* sCOMPtr(String& iStr)
	{
	iStr.Clear();
	return &iStr.GetPtrRef();
	}

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
	{ return l.GetPtrRef() == r; };

bool operator==(const string16& l, const String& r)
	{ return l == r.GetPtrRef(); };

} // namespace ZWinCOM

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(Win)
