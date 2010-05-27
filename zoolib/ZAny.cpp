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

bool sQCoerceBool(const ZAny& iAny, bool& oVal)
	{
	int64 asInt64;
	if (sQCoerceInt(iAny, asInt64))
		{
		oVal = (0 != asInt64);
		return true;
		}

	double asDouble;
	if (sQCoerceReal(iAny, asDouble))
		{
		oVal =(0.0 != asDouble);
		return true;
		}

	if (const string* asString = iAny.PGet_T<string>())
		{
		if (asString->empty())
			{
			oVal = false;
			return true;
			}

		if (ZString::sQDouble(*asString, asDouble))
			{
			oVal = (0.0 != asDouble);
			return true;
			}

		if (ZString::sQInt64(*asString, asInt64))
			{
			oVal = (0 != asInt64);
			return true;
			}

		if (ZString::sEquali(*asString, "t") || ZString::sEquali(*asString, "true"))
			{
			oVal = true;
			return true;
			}

		if (ZString::sEquali(*asString, "f") || ZString::sEquali(*asString, "false"))
			{
			oVal = false;
			return true;
			}
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

bool sQCoerceInt(const ZAny& iAny, int64& oVal)
	{
	if (false)
		{}
	else if (const char* theVal = iAny.PGet_T<char>())
		oVal = *theVal;
	else if (const signed char* theVal = iAny.PGet_T<signed char>())
		oVal = *theVal;
	else if (const unsigned char* theVal = iAny.PGet_T<unsigned char>())
		oVal = *theVal;
	else if (const wchar_t* theVal = iAny.PGet_T<wchar_t>())
		oVal = *theVal;
	else if (const short* theVal = iAny.PGet_T<short>())
		oVal = *theVal;
	else if (const unsigned short* theVal = iAny.PGet_T<unsigned short>())
		oVal = *theVal;
	else if (const int* theVal = iAny.PGet_T<int>())
		oVal = *theVal;
	else if (const unsigned int* theVal = iAny.PGet_T<unsigned int>())
		oVal = *theVal;
	else if (const long* theVal = iAny.PGet_T<long>())
		oVal = *theVal;
	else if (const unsigned long* theVal = iAny.PGet_T<unsigned long>())
		oVal = *theVal;
	else if (const int64* theVal = iAny.PGet_T<int64>())
		oVal = *theVal;
	else if (const uint64* theVal = iAny.PGet_T<uint64>())
		oVal = *theVal;
	else
		return false;
	return true;
	}

int64 sDCoerceInt(int64 iDefault, const ZAny& iAny)
	{
	int64 result;
	if (sQCoerceInt(iAny, result))
		return result;
	return iDefault;
	}

int64 sCoerceInt(const ZAny& iAny)
	{
	int64 result;
	if (sQCoerceInt(iAny, result))
		return result;
	return 0;
	}

bool sQCoerceReal(const ZAny& iAny, double& oVal)
	{
	if (false)
		{}
	else if (const float* theVal = iAny.PGet_T<float>())
		oVal = *theVal;
	else if (const double* theVal = iAny.PGet_T<double>())
		oVal = *theVal;
	else
		return false;
	return true;
	}

double sDCoerceReal(double iDefault, const ZAny& iAny)
	{
	int64 result;
	if (sQCoerceInt(iAny, result))
		return result;
	return iDefault;
	}

double sCoerceReal(const ZAny& iAny)
	{
	double result;
	if (sQCoerceReal(iAny, result))
		return result;
	return 0.0;
	}

} // namespace ZooLib
