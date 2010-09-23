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

#include "zoolib/ZAny.h"
#include "zoolib/ZString.h"

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * ZAnyBase, copied/reworked from boost::any

namespace ZooLib {

ZAnyBase::ZAnyBase()
:	content(0)
	{}

ZAnyBase::ZAnyBase(const ZAnyBase& other)
:	content(other.content ? other.content->clone() : 0)
	{}

ZAnyBase::~ZAnyBase()
	{ delete content; }

ZAnyBase& ZAnyBase::operator=(ZAnyBase rhs)
	{
	rhs.swap(*this);
	return *this;
	}

ZAnyBase& ZAnyBase::swap(ZAnyBase& rhs)
	{
	std::swap(content, rhs.content);
	return *this;
	}

bool ZAnyBase::empty() const
	{ return !content; }

const std::type_info& ZAnyBase::type() const
	{ return content ? content->type() : typeid(void); }

const void* ZAnyBase::voidstar() const
	{ return content ? content->voidstar() : nullptr; }

void* ZAnyBase::voidstar()
	{ return content ? content->voidstar() : nullptr; }

} // namespace ZooLib

// =================================================================================================
#pragma mark -
#pragma mark * ZAny

namespace ZooLib {

ZAny::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(this->type() != typeid(void)); }

void ZAny::Clear()
	{
	ZAny temp;
	this->swap(temp);
	}

} // namespace ZooLib

// =================================================================================================
#pragma mark -
#pragma mark * Coercion

namespace ZooLib {

ZQ<bool> sQCoerceBool(const ZAny& iAny)
	{
	if (const bool* pBool = iAny.PGet_T<bool>())
		return *pBool;

	if (ZQ<int64> qInt64 = sQCoerceInt(iAny))
		return (0 != qInt64.Get());

	if (ZQ<double> qDouble = sQCoerceReal(iAny))
		return (0.0 != qDouble.Get());

	if (const string* pString = iAny.PGet_T<string>())
		{
		if (pString->empty())
			return false;

		if (ZQ<double> qDouble = ZString::sQDouble(*pString))
			return (0.0 != qDouble.Get());

		if (ZQ<int64> qInt64 = ZString::sQInt64(*pString))
			return (0 != qInt64.Get());

		if (ZString::sEquali(*pString, "t") || ZString::sEquali(*pString, "true"))
			return true;

		if (ZString::sEquali(*pString, "f") || ZString::sEquali(*pString, "false"))
			return false;
		}
	return null;
	}

bool sQCoerceBool(const ZAny& iAny, bool& oVal)
	{
	if (ZQ<bool> qBool = sQCoerceBool(iAny))
		{
		oVal = qBool.Get();
		return true;
		}
	return false;
	}

bool sDCoerceBool(bool iDefault, const ZAny& iAny)
	{
	bool result;
	if (sQCoerceBool(iAny, result))
		return result;
	return iDefault;
	}

bool sCoerceBool(const ZAny& iAny)
	{
	bool result;
	if (sQCoerceBool(iAny, result))
		return result;
	return false;
	}

ZQ<int64> sQCoerceInt(const ZAny& iAny)
	{
	if (false)
		{}
	else if (const char* theVal = iAny.PGet_T<char>())
		return *theVal;
	else if (const signed char* theVal = iAny.PGet_T<signed char>())
		return *theVal;
	else if (const unsigned char* theVal = iAny.PGet_T<unsigned char>())
		return *theVal;
	else if (const wchar_t* theVal = iAny.PGet_T<wchar_t>())
		return *theVal;
	else if (const short* theVal = iAny.PGet_T<short>())
		return *theVal;
	else if (const unsigned short* theVal = iAny.PGet_T<unsigned short>())
		return *theVal;
	else if (const int* theVal = iAny.PGet_T<int>())
		return *theVal;
	else if (const unsigned int* theVal = iAny.PGet_T<unsigned int>())
		return *theVal;
	else if (const long* theVal = iAny.PGet_T<long>())
		return *theVal;
	else if (const unsigned long* theVal = iAny.PGet_T<unsigned long>())
		return *theVal;
	else if (const int64* theVal = iAny.PGet_T<int64>())
		return *theVal;
	else if (const uint64* theVal = iAny.PGet_T<uint64>())
		return *theVal;

	return null;
	}

bool sQCoerceInt(const ZAny& iAny, int64& oVal)
	{
	if (ZQ<int64> qInt64 = sQCoerceInt(iAny))
		{
		oVal = qInt64.Get();
		return true;
		}
	return false;
	}

int64 sDCoerceInt(int64 iDefault, const ZAny& iAny)
	{
	if (ZQ<int64> qInt = sQCoerceInt(iAny))
		return qInt.Get();
	return iDefault;
	}

int64 sCoerceInt(const ZAny& iAny)
	{ return sDCoerceInt(0, iAny); }

ZQ<double> sQCoerceReal(const ZAny& iAny)
	{
	if (false)
		{}
	else if (const float* theVal = iAny.PGet_T<float>())
		return *theVal;
	else if (const double* theVal = iAny.PGet_T<double>())
		return *theVal;

	return null;
	}

bool sQCoerceReal(const ZAny& iAny, double& oVal)
	{
	if (ZQ<double> qDouble = sQCoerceReal(iAny))
		{
		oVal = qDouble.Get();
		return true;
		}
	return false;
	}

double sDCoerceReal(double iDefault, const ZAny& iAny)
	{
	if (ZQ<double> qDouble = sQCoerceReal(iAny))
		return qDouble.Get();
	return iDefault;
	}

double sCoerceReal(const ZAny& iAny)
	{ return sDCoerceReal(0.0, iAny); }

} // namespace ZooLib
