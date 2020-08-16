// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ValueOnce_h__
#define __ZooLib_ValueOnce_h__ 1
#include "zconfig.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ValueOnce

template <class T>
class ValueOnce
	{
public:
	ValueOnce(const T& iFirst, const T& iSubsequent)
	:	fFirst(iFirst)
	,	fSubsequent(iSubsequent)
	,	fIsFirst(true)
		{}

	const T& operator()()
		{
		if (fIsFirst)
			{
			fIsFirst = false;
			return fFirst;
			}
		return fSubsequent;
		}

	void Reset()
		{ fIsFirst = true; }

private:
	const T fFirst;
	const T fSubsequent;
	bool fIsFirst;
	};

// =================================================================================================
#pragma mark - FundamentalValueOnce

// Type_p is not fundamental in the precise sense used by the standard, but it's close.
// <http://en.cppreference.com/w/cpp/language/template_parameters#Non-type_template_parameters>
// <https://stackoverflow.com/questions/5687540/non-type-template-parameters>

template <class Type_p, Type_p first_p, Type_p subsequent_p>
class FundamentalValueOnce
	{
public:
	FundamentalValueOnce()
	:	fIsFirst(true)
		{}

	const Type_p operator()()
		{
		if (fIsFirst)
			{
			fIsFirst = false;
			return first_p;
			}
		return subsequent_p;
		}

	void Reset()
		{ fIsFirst = true; }

private:
	bool fIsFirst;
	};

// =================================================================================================
#pragma mark -

typedef FundamentalValueOnce<bool,true,false> TrueOnce;

typedef FundamentalValueOnce<bool,false,true> FalseOnce;

} // namespace ZooLib

#endif // __ZooLib_ValueOnce_h__
