/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#ifndef __ZQ_T__
#define __ZQ_T__
#include "zconfig.h"

#include "zoolib/ZCompat_operator_bool.h"
#include "zoolib/ZCompat_algorithm.h" // For std::swap
#include "zoolib/ZCtorDtor.h" // For placement ctor/copy/dtor/assign
#include "zoolib/ZDebug.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZQ_T

template <class T>
class ZQ_T
	{
public:
	void swap(ZQ_T& iOther)
		{
		if (fHasValue)
			{
			if (iOther.fHasValue)
				{
				std::swap(*(T*)fBytes, *(T*)iOther.fBytes);
				}
			else
				{
				sCopyConstruct_T<T>(iOther.fBytes, fBytes);
				sDestroy_T<T>(fBytes);
				iOther.fHasValue = true;
				fHasValue = false;
				}
			}
		else if (iOther.fHasValue)
			{
			sCopyConstruct_T<T>(fBytes, iOther.fBytes);
			sDestroy_T<T>(iOther.fBytes);
			iOther.fHasValue = false;
			fHasValue = true;
			}
		}

	ZQ_T()
	:	fHasValue(false)
		{}

	ZQ_T(const ZQ_T& iOther)
	:	fHasValue(iOther.fHasValue)
		{
		if (fHasValue)
			sCopyConstruct_T<T>(fBytes, iOther.fBytes);
		}

	~ZQ_T()
		{
		if (fHasValue)
			sDestroy_T<T>(fBytes);
		}

	ZQ_T& operator=(const ZQ_T& iOther)
		{
		if (this != &iOther)
			{
			if (fHasValue)
				sDestroy_T<T>(fBytes);

			fHasValue = iOther.fHasValue;

			if (fHasValue)
				sCopyConstruct_T<T>(fBytes, iOther.fBytes);
			}
		}

	ZQ_T(const null_t& iP0)
	:	fHasValue(false)
		{}

	template <class P0>
	ZQ_T(const P0& iP0)
	:	fHasValue(true)
		{ sConstruct_T<T, P0>(fBytes, iP0); }

	template <class P0, class P1>
	ZQ_T(const P0& iP0, const P1& iP1)
	:	fHasValue(true)
		{ sConstruct_T<T, P0, P1>(fBytes, iP0, iP1); }

	ZQ_T& operator=(const T& iValue)
		{
		if (fHasValue)
			sDestroy_T<T>(fBytes);

		fHasValue = true;

		sCopyConstruct_T<T>(fBytes, &iValue);

		return *this;
		}

	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES_T(ZQ_T,
		operator_bool_generator_type, operator_bool_type);

	operator operator_bool_type() const
		{ return operator_bool_generator_type::translate(fHasValue); }

	const T& Get() const
		{
		ZAssert(fHasValue);
		return *sFetch_T<T>(fBytes);
		}

	T& Get()
		{
		ZAssert(fHasValue);
		return *sFetch_T<T>(fBytes);
		}

	T& OParam()
		{
		if (fHasValue)
			sDestroy_T<T>(fBytes);

		fHasValue = true;

		sConstruct_T<T>(fBytes);
		}

	void Clear()
		{
		if (fHasValue)
			{
			sDestroy_T<T>(fBytes);
			fHasValue = false;
			}
		}

private:
	char fBytes[sizeof(T)];
	bool fHasValue;
	};

} // namespace ZooLib

#endif // __ZQ_T__
