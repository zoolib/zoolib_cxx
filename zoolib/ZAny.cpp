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

// =================================================================================================
#pragma mark -
#pragma mark * ZAny, copied/reworked from boost::any

#if ! ZCONFIG_SPI_Enabled(boost)

NAMESPACE_ZOOLIB_BEGIN

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

NAMESPACE_ZOOLIB_END

#endif // ! ZCONFIG_SPI_Enabled(boost)

// =================================================================================================
#pragma mark -
#pragma mark * ZAny

NAMESPACE_ZOOLIB_BEGIN

ZAny::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(this->type() != typeid(void)); }

void ZAny::Clear()
	{
	ZAny temp;
	this->swap(temp);
	}

NAMESPACE_ZOOLIB_END

// =================================================================================================
#pragma mark -
#pragma mark * Coercion

NAMESPACE_ZOOLIB_BEGIN

bool sQCoerceInt(const ZAny& iAny, int64& oVal)
	{
	if (false)
		{}
	else if (const char* theVal = ZAnyCast<char>(&iAny))
		oVal = *theVal;
	else if (const signed char* theVal = ZAnyCast<signed char>(&iAny))
		oVal = *theVal;
	else if (const unsigned char* theVal = ZAnyCast<unsigned char>(&iAny))
		oVal = *theVal;
	else if (const short* theVal = ZAnyCast<short>(&iAny))
		oVal = *theVal;
	else if (const unsigned short* theVal = ZAnyCast<unsigned short>(&iAny))
		oVal = *theVal;
	else if (const int* theVal = ZAnyCast<int>(&iAny))
		oVal = *theVal;
	else if (const unsigned int* theVal = ZAnyCast<unsigned int>(&iAny))
		oVal = *theVal;
	else if (const long* theVal = ZAnyCast<long>(&iAny))
		oVal = *theVal;
	else if (const unsigned long* theVal = ZAnyCast<unsigned long>(&iAny))
		oVal = *theVal;
	else if (const int64* theVal = ZAnyCast<int64>(&iAny))
		oVal = *theVal;
	else if (const uint64* theVal = ZAnyCast<uint64>(&iAny))
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
	else if (const float* theVal = ZAnyCast<float>(&iAny))
		oVal = *theVal;
	else if (const double* theVal = ZAnyCast<double>(&iAny))
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
	return 0;
	}

NAMESPACE_ZOOLIB_END
