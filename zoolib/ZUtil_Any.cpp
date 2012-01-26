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

#include "zoolib/ZUtil_Any.h"
#include "zoolib/ZUtil_string.h"

using std::string;

// =================================================================================================
// MARK: - Coercion

namespace ZooLib {

ZQ<bool> sQCoerceBool(const ZAny& iAny)
	{
	if (const bool* pBool = iAny.PGet<bool>())
		return *pBool;

	if (ZQ<int64> theQ = sQCoerceInt(iAny))
		return *theQ;

	if (ZQ<double> theQ = sQCoerceRat(iAny))
		return *theQ;

	if (const string* pString = iAny.PGet<string>())
		{
		if (pString->empty())
			return false;

		if (ZQ<double> theQ = ZUtil_string::sQDouble(*pString))
			return *theQ;

		if (ZQ<int64> theQ = ZUtil_string::sQInt64(*pString))
			return *theQ;

		if (ZUtil_string::sEquali(*pString, "t") || ZUtil_string::sEquali(*pString, "true"))
			return true;

		if (ZUtil_string::sEquali(*pString, "f") || ZUtil_string::sEquali(*pString, "false"))
			return false;
		}
	return null;
	}

bool sQCoerceBool(const ZAny& iAny, bool& oVal)
	{
	if (ZQ<bool> theQ = sQCoerceBool(iAny))
		{
		oVal = *theQ;
		return true;
		}
	return false;
	}

bool sDCoerceBool(bool iDefault, const ZAny& iAny)
	{
	if (ZQ<bool> theQ = sQCoerceBool(iAny))
		return *theQ;
	return iDefault;
	}

bool sCoerceBool(const ZAny& iAny)
	{
	if (ZQ<bool> theQ = sQCoerceBool(iAny))
		return *theQ;
	return false;
	}

ZQ<int64> sQCoerceInt(const ZAny& iAny)
	{
	if (false)
		{}
	else if (const char* theP = iAny.PGet<char>())
		return *theP;
	else if (const signed char* theP = iAny.PGet<signed char>())
		return *theP;
	else if (const unsigned char* theP = iAny.PGet<unsigned char>())
		return *theP;
	else if (const wchar_t* theP = iAny.PGet<wchar_t>())
		return *theP;
	else if (const short* theP = iAny.PGet<short>())
		return *theP;
	else if (const unsigned short* theP = iAny.PGet<unsigned short>())
		return *theP;
	else if (const int* theP = iAny.PGet<int>())
		return *theP;
	else if (const unsigned int* theP = iAny.PGet<unsigned int>())
		return *theP;
	else if (const long* theP = iAny.PGet<long>())
		return *theP;
	else if (const unsigned long* theP = iAny.PGet<unsigned long>())
		return *theP;
	else if (const int64* theP = iAny.PGet<int64>())
		return *theP;
	else if (const uint64* theP = iAny.PGet<uint64>())
		return *theP;

	return null;
	}

bool sQCoerceInt(const ZAny& iAny, int64& oVal)
	{
	if (ZQ<int64> theQ = sQCoerceInt(iAny))
		{
		oVal = *theQ;
		return true;
		}
	return false;
	}

int64 sDCoerceInt(int64 iDefault, const ZAny& iAny)
	{
	if (ZQ<int64> theQ = sQCoerceInt(iAny))
		return *theQ;
	return iDefault;
	}

int64 sCoerceInt(const ZAny& iAny)
	{
	if (ZQ<int64> theQ = sQCoerceInt(iAny))
		return *theQ;
	return 0;
	}

ZQ<double> sQCoerceRat(const ZAny& iAny)
	{
	if (false)
		{}
	else if (const float* theP = iAny.PGet<float>())
		return *theP;
	else if (const double* theP = iAny.PGet<double>())
		return *theP;

	return null;
	}

bool sQCoerceRat(const ZAny& iAny, double& oVal)
	{
	if (ZQ<double> theQ = sQCoerceRat(iAny))
		{
		oVal = *theQ;
		return true;
		}
	return false;
	}

double sDCoerceRat(double iDefault, const ZAny& iAny)
	{
	if (ZQ<double> theQ = sQCoerceRat(iAny))
		return *theQ;
	return iDefault;
	}

double sCoerceRat(const ZAny& iAny)
	{
	if (ZQ<double> theQ = sQCoerceRat(iAny))
		return *theQ;
	return 0.0;
	}

} // namespace ZooLib
