/* -------------------------------------------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZTime__
#define __ZTime__ 1
#include "zconfig.h"

#include "ZTypes.h"

#include "ZCompat_operator_bool.h"

class ZMutex;

// =================================================================================================
#pragma mark -
#pragma mark * ZTime

class ZTime
	{
    ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZTime, operator_bool_generator_type, operator_bool_type);
public:
	ZTime();

	ZTime(double iDouble) : fVal(iDouble) {}

	ZTime& operator=(double iDouble) { fVal = iDouble; return *this; }

	operator operator_bool_type() const;

	bool operator<(const ZTime& iOther) const;

	bool operator<=(const ZTime& iOther) const { return !(iOther < *this); }

	bool operator>(const ZTime& iOther) const { return iOther < *this; }

	bool operator>=(const ZTime& iOther) const { return !(*this < iOther); }

	bool operator==(const ZTime& iOther) const;

	bool operator!=(const ZTime& iOther) const { return !(*this == iOther); }

	int Compare(const ZTime& iOther) const;

	double operator-(const ZTime& iOther) const { return fVal - iOther.fVal; }

	ZTime operator-(double iInterval)
		{
		ZTime result = *this;
		result.fVal -= iInterval;
		return result;
		}

	ZTime& operator-=(double iInterval)
		{ fVal -= iInterval; return *this; }

	ZTime operator+(double iInterval) const
		{
		ZTime result = *this;
		result.fVal += iInterval;
		return result;
		}

	ZTime& operator+=(double iInterval)
		{ fVal += iInterval; return *this; }

	static ZTime sNow();
	static ZTime sSystem();
	static ZTime sBoot();
	static ZTime sSinceBoot();

	double fVal;

	static const uint32 kEpochDelta_1900_To_1904
		= 4U * 365 * 24 * 60 * 60; // 126,144,000

	static const uint32 kEpochDelta_1904_To_1970
		= ((66U * 365) + 17) * 24 * 60 * 60; // 2,082,844,800

	static const uint32 kEpochDelta_1900_To_1970
		= ((70U * 365) + 17) * 24 * 60 * 60; // 2,208,988,800

	static const uint64 kEpochDelta_1601_To_1970
		= ((369LLU * 365) + 89) * 24 * 60 * 60; // == 11,644,473,600
	};

namespace ZooLib {
template <class T> int sCompare_T(const T& iL, const T& iR);
template <> inline int sCompare_T(const ZTime& iL, const ZTime& iR)
	{ return iL.Compare(iR); }
}

#endif // __ZTime__
