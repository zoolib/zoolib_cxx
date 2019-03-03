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

#include "zoolib/Coerce_Any.h"

#include "zoolib/Util_string.h"

using std::string;

// =================================================================================================
#pragma mark - Coercion

namespace ZooLib {

ZQ<bool> sQCoerceBool(const Any& iAny)
	{
	if (const bool* pBool = iAny.PGet<bool>())
		return *pBool;

	if (ZQ<__int64> qq = sQCoerceInt(iAny))
		return *qq;

	if (ZQ<double> qq = sQCoerceRat(iAny))
		return *qq;

	if (const string* pString = iAny.PGet<string>())
		{
		if (pString->empty())
			return false;

		if (ZQ<double> qq = Util_string::sQDouble(*pString))
			return *qq;

		if (ZQ<__int64> qq = Util_string::sQInt64(*pString))
			return *qq;

		if (Util_string::sEquali(*pString, "t") || Util_string::sEquali(*pString, "true"))
			return true;

		if (Util_string::sEquali(*pString, "f") || Util_string::sEquali(*pString, "false"))
			return false;
		}
	return null;
	}

bool sQCoerceBool(const Any& iAny, bool& oVal)
	{
	if (ZQ<bool> qq = sQCoerceBool(iAny))
		{
		oVal = *qq;
		return true;
		}
	return false;
	}

bool sDCoerceBool(bool iDefault, const Any& iAny)
	{
	if (ZQ<bool> qq = sQCoerceBool(iAny))
		return *qq;
	return iDefault;
	}

bool sCoerceBool(const Any& iAny)
	{
	if (ZQ<bool> qq = sQCoerceBool(iAny))
		return *qq;
	return false;
	}

ZQ<__int64> sQCoerceInt(const Any& iAny)
	{
	if (false)
		{}
	else if (const char* pp = iAny.PGet<char>())
		return *pp;
	else if (const signed char* pp = iAny.PGet<signed char>())
		return *pp;
	else if (const unsigned char* pp = iAny.PGet<unsigned char>())
		return *pp;
	else if (const wchar_t* pp = iAny.PGet<wchar_t>())
		return *pp;
	else if (const short* pp = iAny.PGet<short>())
		return *pp;
	else if (const unsigned short* pp = iAny.PGet<unsigned short>())
		return *pp;
	else if (const int* pp = iAny.PGet<int>())
		return *pp;
	else if (const unsigned int* pp = iAny.PGet<unsigned int>())
		return *pp;
	else if (const long* pp = iAny.PGet<long>())
		return *pp;
	else if (const unsigned long* pp = iAny.PGet<unsigned long>())
		return *pp;
	else if (const __int64* pp = iAny.PGet<__int64>())
		return *pp;
	else if (const __uint64* pp = iAny.PGet<__uint64>())
		return *pp;

	return null;
	}

bool sQCoerceInt(const Any& iAny, __int64& oVal)
	{
	if (ZQ<__int64> qq = sQCoerceInt(iAny))
		{
		oVal = *qq;
		return true;
		}
	return false;
	}

__int64 sDCoerceInt(__int64 iDefault, const Any& iAny)
	{
	if (ZQ<__int64> qq = sQCoerceInt(iAny))
		return *qq;
	return iDefault;
	}

__int64 sCoerceInt(const Any& iAny)
	{
	if (ZQ<__int64> qq = sQCoerceInt(iAny))
		return *qq;
	return 0;
	}

ZQ<double> sQCoerceRat(const Any& iAny)
	{
	if (false)
		{}
	else if (const float* pp = iAny.PGet<float>())
		return *pp;
	else if (const double* pp = iAny.PGet<double>())
		return *pp;
	else if (const long double* pp = iAny.PGet<long double>())
		return *pp;

	return null;
	}

bool sQCoerceRat(const Any& iAny, double& oVal)
	{
	if (ZQ<double> qq = sQCoerceRat(iAny))
		{
		oVal = *qq;
		return true;
		}
	return false;
	}

double sDCoerceRat(double iDefault, const Any& iAny)
	{
	if (ZQ<double> qq = sQCoerceRat(iAny))
		return *qq;
	return iDefault;
	}

double sCoerceRat(const Any& iAny)
	{
	if (ZQ<double> qq = sQCoerceRat(iAny))
		return *qq;
	return 0.0;
	}

ZQ<__int64> sQCoerceNumberAsInt(const Any& iAny)
	{
	if (ZQ<__int64> theQ = sQCoerceInt(iAny))
		return *theQ;
	if (ZQ<double> theQ = sQCoerceRat(iAny))
		return *theQ;
	if (const bool* pBool = iAny.PGet<bool>())
		return *pBool;
	return null;
	}

ZQ<double> sQCoerceNumberAsRat(const Any& iAny)
	{
	if (ZQ<__int64> theQ = sQCoerceInt(iAny))
		return *theQ;
	if (ZQ<double> theQ = sQCoerceRat(iAny))
		return *theQ;
	if (const bool* pBool = iAny.PGet<bool>())
		return *pBool;
	return null;
	}

} // namespace ZooLib
