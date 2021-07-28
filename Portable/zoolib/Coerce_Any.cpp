// Copyright (c) 2009 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Coerce_Any.h"

#include "zoolib/Util_string.h"

using std::string;

// =================================================================================================
#pragma mark - Coercion

namespace ZooLib {

ZQ<bool> sQCoerceBool(const AnyBase& iAny)
	{
	if (const bool* pBool = iAny.PGet<bool>())
		return *pBool;

	if (ZQ<int64> qq = sQCoerceInt(iAny))
		return *qq;

	if (ZQ<double> qq = sQCoerceRat(iAny))
		return *qq;

	if (const string* pString = iAny.PGet<string>())
		{
		if (pString->empty())
			return false;

		if (ZQ<double> qq = Util_string::sQDouble(*pString))
			return *qq;

		if (ZQ<int64> qq = Util_string::sQInt64(*pString))
			return *qq;

		if (Util_string::sEquali(*pString, "t") || Util_string::sEquali(*pString, "true"))
			return true;

		if (Util_string::sEquali(*pString, "f") || Util_string::sEquali(*pString, "false"))
			return false;
		}
	return null;
	}

bool sQCoerceBool(const AnyBase& iAny, bool& oVal)
	{
	if (ZQ<bool> qq = sQCoerceBool(iAny))
		{
		oVal = *qq;
		return true;
		}
	return false;
	}

bool sCoerceBool(const AnyBase& iAny)
	{
	if (ZQ<bool> qq = sQCoerceBool(iAny))
		return *qq;
	return false;
	}

ZQ<int64> sQCoerceInt(const AnyBase& iAny)
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
	else if (const long long* pp = iAny.PGet<long long>())
		return *pp;
	else if (const unsigned long long* pp = iAny.PGet<unsigned long long>())
		return *pp;

	return null;
	}

bool sQCoerceInt(const AnyBase& iAny, int64& oVal)
	{
	if (ZQ<int64> qq = sQCoerceInt(iAny))
		{
		oVal = *qq;
		return true;
		}
	return false;
	}

int64 sCoerceInt(const AnyBase& iAny)
	{
	if (ZQ<int64> qq = sQCoerceInt(iAny))
		return *qq;
	return 0;
	}

ZQ<double> sQCoerceRat(const AnyBase& iAny)
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

bool sQCoerceRat(const AnyBase& iAny, double& oVal)
	{
	if (ZQ<double> qq = sQCoerceRat(iAny))
		{
		oVal = *qq;
		return true;
		}
	return false;
	}

double sCoerceRat(const AnyBase& iAny)
	{
	if (ZQ<double> qq = sQCoerceRat(iAny))
		return *qq;
	return 0.0;
	}

ZQ<int64> sQCoerceNumberAsInt(const AnyBase& iAny)
	{
	if (ZQ<int64> theQ = sQCoerceInt(iAny))
		return *theQ;
	if (ZQ<double> theQ = sQCoerceRat(iAny))
		return *theQ;
	if (const bool* pBool = iAny.PGet<bool>())
		return *pBool;
	return null;
	}

ZQ<double> sQCoerceNumberAsRat(const AnyBase& iAny)
	{
	if (ZQ<int64> theQ = sQCoerceInt(iAny))
		return *theQ;
	if (ZQ<double> theQ = sQCoerceRat(iAny))
		return *theQ;
	if (const bool* pBool = iAny.PGet<bool>())
		return *pBool;
	return null;
	}

} // namespace ZooLib
