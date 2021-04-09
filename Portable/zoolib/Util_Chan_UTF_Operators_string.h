// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_Chan_UTF_Operators_string_h__
#define __ZooLib_Util_Chan_UTF_Operators_string_h__ 1
#include "zconfig.h"

#include "zoolib/Chan_UTF_string.h"
#include "zoolib/ChanR_UTF.h"
#include "zoolib/ChanW_UTF.h"
#include "zoolib/Util_Chan_UTF_Operators.h"

namespace ZooLib {
namespace Util_Chan_UTF_Operators {

// =================================================================================================
#pragma mark - Util_Chan_UTF_Operators

// These are not efficient -- syntactically and performance-wise they're very
// like the standard operator+

template <class Val_p>
string32 operator<<(const string32& iString, const Val_p& iVal)
	{
	string32 result = iString;
	ChanW_UTF_string<UTF32>(&result) << iVal;
	return result;
	}

template <class Val_p>
string16 operator<<(const string16& iString, const Val_p& iVal)
	{
	string16 result = iString;
	ChanW_UTF_string<UTF16>(&result) << iVal;
	return result;
	}

template <class Val_p>
string8 operator<<(const string8& iString, const Val_p& iVal)
	{
	string8 result = iString;
	ChanW_UTF_string<UTF8>(&result) << iVal;
	return result;
	}

// =================================================================================================
#pragma mark - Util_Chan_UTF_Operators

template <class Val_p>
string32& operator<<(string32& ioString, const Val_p& iVal)
	{
	ChanW_UTF_string<UTF32>(&ioString) << iVal;
	return ioString;
	}

template <class Val_p>
string16& operator<<(string16& ioString, const Val_p& iVal)
	{
	ChanW_UTF_string<UTF16>(&ioString) << iVal;
	return ioString;
	}

template <class Val_p>
string8& operator<<(string8& ioString, const Val_p& iVal)
	{
	ChanW_UTF_string<UTF8>(&ioString) << iVal;
	return ioString;
	}

} // namespace Util_Chan_UTF_Operators

#if not defined(ZMACRO_Util_Chan_UTF_Operators_Suppress) || not ZMACRO_Util_Chan_UTF_Operators_Suppress
	using Util_Chan_UTF_Operators::operator<<;
#endif

} // namespace ZooLib

#endif // __ZooLib_Util_Chan_UTF_Operators_string_h__
