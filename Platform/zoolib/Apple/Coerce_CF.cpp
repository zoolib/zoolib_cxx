/* -------------------------------------------------------------------------------------------------
Copyright (c) 2018 Andrew Green and Mark/Space, Inc.
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

#include "zoolib/Apple/Coerce_CF.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/Util_string.h"

#include "zoolib/Apple/Util_CF.h"

#include ZMACINCLUDE2(CoreFoundation,CFNumber.h)
#include ZMACINCLUDE2(CoreFoundation,CFString.h)

// =================================================================================================
#pragma mark - Coercion

namespace ZooLib {

using namespace Util_CF;

ZQ<bool> sQCoerceBool(CFTypeRef iCF)
	{
	if (not iCF)
		return null;

	if (CFBooleanRef theBooleanRef = sStaticCastIf<CFBooleanRef>(::CFBooleanGetTypeID(), iCF))
		return CFBooleanGetValue(theBooleanRef);

	if (ZQ<__int64> qq = sQCoerceInt(iCF))
		return *qq;

	if (ZQ<double> qq = sQCoerceRat(iCF))
		return *qq;

	if (CFStringRef theStringRef = sStaticCastIf<CFStringRef>(::CFStringGetTypeID(), iCF))
		{
		const string8 asString = Util_CF::sAsUTF8(theStringRef);

		if (asString.empty())
			return false;

		if (ZQ<double> qq = Util_string::sQDouble(asString))
			return *qq;

		if (ZQ<__int64> qq = Util_string::sQInt64(asString))
			return *qq;

		if (Util_string::sEquali(asString, "t") || Util_string::sEquali(asString, "true"))
			return true;

		if (Util_string::sEquali(asString, "f") || Util_string::sEquali(asString, "false"))
			return false;
		}
	return null;
	}

bool sQCoerceBool(CFTypeRef iCF, bool& oVal)
	{
	if (ZQ<bool> qq = sQCoerceBool(iCF))
		{
		oVal = *qq;
		return true;
		}
	return false;
	}

bool sDCoerceBool(bool iDefault, CFTypeRef iCF)
	{
	if (ZQ<bool> qq = sQCoerceBool(iCF))
		return *qq;
	return iDefault;
	}

bool sCoerceBool(CFTypeRef iCF)
	{
	if (ZQ<bool> qq = sQCoerceBool(iCF))
		return *qq;
	return false;
	}

bool sCoerceBool(const ZRef<CFTypeRef>& iCF)
	{ return sCoerceBool(iCF.Get()); }

ZQ<__int64> sQCoerceInt(CFTypeRef iCF)
	{
	if (iCF && ::CFGetTypeID(iCF) == ::CFNumberGetTypeID())
		{
		__int64 result;
		::CFNumberGetValue((CFNumberRef)iCF, kCFNumberSInt64Type, &result);
		return result;
		}
	return null;
	}

bool sQCoerceInt(CFTypeRef iCF, __int64& oVal)
	{
	if (ZQ<__int64> qq = sQCoerceInt(iCF))
		{
		oVal = *qq;
		return true;
		}
	return false;
	}

__int64 sDCoerceInt(__int64 iDefault, CFTypeRef iCF)
	{
	if (ZQ<__int64> qq = sQCoerceInt(iCF))
		return *qq;
	return iDefault;
	}

__int64 sCoerceInt(CFTypeRef iCF)
	{
	if (ZQ<__int64> qq = sQCoerceInt(iCF))
		return *qq;
	return 0;
	}

__int64 sCoerceInt(const ZRef<CFTypeRef>& iCF)
	{ return sCoerceInt(iCF.Get()); }

ZQ<double> sQCoerceRat(CFTypeRef iCF)
	{
	if (CFNumberRef theNumberRef = sStaticCastIf<CFNumberRef>(::CFNumberGetTypeID(), iCF))
		{
		double result;
		::CFNumberGetValue(theNumberRef, kCFNumberFloat64Type, &result);
		return result;
		}
	return null;
	}

bool sQCoerceRat(CFTypeRef iCF, double& oVal)
	{
	if (ZQ<double> qq = sQCoerceRat(iCF))
		{
		oVal = *qq;
		return true;
		}
	return false;
	}

double sDCoerceRat(double iDefault, CFTypeRef iCF)
	{
	if (ZQ<double> qq = sQCoerceRat(iCF))
		return *qq;
	return iDefault;
	}

double sCoerceRat(CFTypeRef iCF)
	{
	if (ZQ<double> qq = sQCoerceRat(iCF))
		return *qq;
	return 0.0;
	}

double sCoerceRat(const ZRef<CFTypeRef>& iCF)
	{ return sCoerceRat(iCF.Get()); }

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CFType)
