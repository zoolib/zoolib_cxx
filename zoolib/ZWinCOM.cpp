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

NAMESPACE_ZOOLIB_BEGIN

namespace ZWinCOM {

// =================================================================================================
#pragma mark -
#pragma mark * Variant

void Variant::swap(Variant& iOther)
	{
	std::swap(static_cast<VARIANT&>(*this), static_cast<VARIANT&>(iOther));
	}

Variant::Variant()
	{
	::VariantInit(this);
	}

Variant::Variant(const VARIANT& iOther)
	{
	::VariantInit(this);
	::VariantCopy(this, const_cast<VARIANT*>(&iOther));
	}

Variant::~Variant()
	{
	::VariantClear(this);
	}

Variant& Variant::operator=(const VARIANT& iOther)
	{
	Variant(iOther).swap(*this);
	return *this;
	}

Variant::Variant(bool iParam)
	{
	::VariantInit(this);
	vt = VT_BOOL;
	boolVal = iParam ? VARIANT_TRUE : VARIANT_FALSE;
	}

Variant::Variant(int8 iParam)
	{
	::VariantInit(this);
	vt = VT_I1;
	cVal = iParam;
	}

Variant::Variant(uint8 iParam)
	{
	::VariantInit(this);
	vt = VT_UI1;
	bVal = iParam;
	}

Variant::Variant(int16 iParam)
	{
	::VariantInit(this);
	vt = VT_I2;
	iVal = iParam;
	}

Variant::Variant(uint16 iParam)
	{
	::VariantInit(this);
	vt = VT_UI2;
	uiVal = iParam;
	}

Variant::Variant(int32 iParam)
	{
	::VariantInit(this);
	vt = VT_I4;
	lVal = iParam;
	}

Variant::Variant(uint32 iParam)
	{
	::VariantInit(this);
	vt = VT_UI4;
	ulVal = iParam;
	}

Variant::Variant(int64 iParam)
	{
	::VariantInit(this);
	vt = VT_I8;
	llVal = iParam;
	}

Variant::Variant(uint64 iParam)
	{
	::VariantInit(this);
	vt = VT_UI8;
	ullVal = iParam;
	}

Variant::Variant(float iParam)
	{
	::VariantInit(this);
	vt = VT_R4;
	fltVal = iParam;
	}

Variant::Variant(double iParam)
	{
	::VariantInit(this);
	vt = VT_R8;
	dblVal = iParam;
	}

Variant::Variant(const std::string& iParam)
	{
	::VariantInit(this);
	vt = VT_BSTR;
	const string16 asUTF16 = ZUnicode::sAsUTF16(iParam);
	bstrVal = ::SysAllocStringLen(asUTF16.c_str(), asUTF16.length());
	}

Variant::Variant(const string16& iParam)
	{
	::VariantInit(this);
	vt = VT_BSTR;
	bstrVal = ::SysAllocStringLen(iParam.c_str(), iParam.length());
	}

Variant::Variant(ZRef<IUnknown> iParam)
	{
	::VariantInit(this);
	vt = VT_UNKNOWN;
	sRefCopy(&punkVal, iParam.Get());
	}

Variant::Variant(ZRef<IDispatch> iParam)
	{
	::VariantInit(this);
	vt = VT_DISPATCH;
	sRefCopy(&pdispVal, iParam.Get());
	}

bool Variant::GetBool() const
	{ return this->DGetBool(false); }

bool Variant::GetBool(bool& oValue) const
	{
	if (vt != VT_BOOL)
		return false;
	oValue = boolVal;
	return true;
	}

bool Variant::DGetBool(bool iDefault) const
	{
	if (vt != VT_BOOL)
		return iDefault;
	return boolVal;
	}

int8 Variant::GetInt8() const
	{ return this->DGetInt8(0); }

bool Variant::GetInt8(int8& oValue) const
	{
	if (vt != VT_I1)
		return false;
	oValue = cVal;
	return true;
	}

int8 Variant::DGetInt8(int8 iDefault) const
	{
	if (vt != VT_I1)
		return iDefault;
	return cVal;
	}

uint8 Variant::GetUInt8() const
	{ return this->DGetUInt8(0); }

bool Variant::GetUInt8(uint8& oValue) const
	{
	if (vt != VT_UI1)
		return false;
	oValue = bVal;
	return true;
	}

uint8 Variant::DGetUInt8(uint8 iDefault) const
	{
	if (vt != VT_UI1)
		return iDefault;
	return bVal;
	}

int16 Variant::GetInt16() const
	{ return this->DGetInt16(0); }

bool Variant::GetInt16(int16& oValue) const
	{
	if (vt != VT_I2)
		return false;
	oValue = iVal;
	return true;
	}

int16 Variant::DGetInt16(int16 iDefault) const
	{
	if (vt != VT_I2)
		return iDefault;
	return iVal;
	}

uint16 Variant::GetUInt16() const
	{ return this->DGetUInt16(0); }

bool Variant::GetUInt16(uint16& oValue) const
	{
	if (vt != VT_UI2)
		return false;
	oValue = uiVal;
	return true;
	}

uint16 Variant::DGetUInt16(uint16 iDefault) const
	{
	if (vt != VT_UI2)
		return iDefault;
	return iVal;
	}

int32 Variant::GetInt32() const
	{ return this->DGetInt32(0); }

bool Variant::GetInt32(int32& oValue) const
	{
	if (vt != VT_I4)
		return false;
	oValue = lVal;
	return true;
	}

int32 Variant::DGetInt32(int32 iDefault) const
	{
	if (vt != VT_I4)
		return iDefault;
	return lVal;
	}

uint32 Variant::GetUInt32() const
	{ return this->DGetUInt32(0); }

bool Variant::GetUInt32(uint32& oValue) const
	{
	if (vt != VT_UI4)
		return false;
	oValue = ulVal;
	return true;
	}

uint32 Variant::DGetUInt32(uint32 iDefault) const
	{
	if (vt != VT_UI4)
		return iDefault;
	return ulVal;
	}

int64 Variant::GetInt64() const
	{ return this->DGetInt64(0); }

bool Variant::GetInt64(int64& oValue) const
	{
	if (vt != VT_I8)
		return false;
	oValue = llVal;
	return true;
	}

int64 Variant::DGetInt64(int64 iDefault) const
	{
	if (vt != VT_I8)
		return iDefault;
	return llVal;
	}

uint64 Variant::GetUInt64() const
	{ return this->DGetUInt64(0); }

bool Variant::GetUInt64(uint64& oValue) const
	{
	if (vt != VT_UI8)
		return false;
	oValue = ullVal;
	return true;
	}

uint64 Variant::DGetUInt64(uint64 iDefault) const
	{
	if (vt != VT_UI8)
		return iDefault;
	return ullVal;
	}

float Variant::GetFloat() const
	{ return this->DGetFloat(0); }
	
bool Variant::GetFloat(float& oValue) const
	{
	if (vt != VT_R4)
		return false;
	oValue = fltVal;
	return true;
	}

float Variant::DGetFloat(float iDefault) const
	{
	if (vt != VT_R4)
		return iDefault;
	return fltVal;
	}

double Variant::GetDouble() const
	{ return this->DGetDouble(0); }
	
bool Variant::GetDouble(double& oValue) const
	{
	if (vt != VT_R8)
		return false;
	oValue = dblVal;
	return true;
	}

double Variant::DGetDouble(double iDefault) const
	{
	if (vt != VT_R8)
		return iDefault;
	return dblVal;
	}

string16 Variant::GetString16() const
	{ return this->DGetString16(string16()); }

bool Variant::GetString16(string16& oValue) const
	{
	if (vt != VT_BSTR)
		return false;
	oValue = string16();
	ZUnimplemented();
	return true;
	}

string16 Variant::DGetString16(const string16& iDefault) const
	{
	if (vt != VT_BSTR)
		return iDefault;
	ZUnimplemented();
	return string16();
	}

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
