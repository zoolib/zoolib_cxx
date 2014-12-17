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

#ifndef __ZooLib_ValPred_ValPred_Any_h__
#define __ZooLib_ValPred_ValPred_Any_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"
#include "zoolib/Val_Any.h"

#include "zoolib/ValPred/ValPred.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ValComparand_Const_Any

class ValComparand_Const_Any : public ValComparand
	{
public:
	ValComparand_Const_Any(const Val_Any& iVal);

// Our protocol
	const Val_Any& GetVal() const;

private:
	const Val_Any fVal;
	};

template <>
int sCompare_T(const ValComparand_Const_Any& iL, const ValComparand_Const_Any& iR);

// =================================================================================================
// MARK: - ValComparator_Callable_Any

class ValComparator_Callable_Any : public ValComparator
	{
public:
	typedef Callable<bool(const Val_Any& iLHS, const Val_Any& iRHS)> Callable_t;

	ValComparator_Callable_Any(ZRef<Callable_t> iCallable);

// Our protocol
	const ZRef<Callable_t>& GetCallable() const;

private:
	ZRef<Callable_t> fCallable;
	};

template <>
int sCompare_T(const ValComparator_Callable_Any& iL, const ValComparator_Callable_Any& iR);

// =================================================================================================
// MARK: - ValComparator_StringContains

class ValComparator_StringContains : public ValComparator
	{
public:
	ValComparator_StringContains(int iStrength);

// Our protocol
	int GetStrength() const;

private:
	int fStrength;
	};

template <>
int sCompare_T(const ValComparator_StringContains& iL, const ValComparator_StringContains& iR);

// =================================================================================================
// MARK: - Comparand pseudo constructors

ValComparandPseudo CConst(const Val_Any& iVal);

// =================================================================================================
// MARK: -

bool sMatches(const ValPred& iValPred, const Val_Any& iVal);

} // namespace ZooLib

#endif // __ZooLib_ValPred_ValPred_Any_h__
