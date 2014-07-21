/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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

#ifndef __ZooLib_ValueOnce_h__
#define __ZooLib_ValueOnce_h__ 1
#include "zconfig.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ValueOnce

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
// MARK: - FundamentalValueOnce

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
// MARK: -

typedef FundamentalValueOnce<bool,true,false> TrueOnce;

typedef FundamentalValueOnce<bool,false,true> FalseOnce;

} // namespace ZooLib

#endif // __ZooLib_ValueOnce_h__
