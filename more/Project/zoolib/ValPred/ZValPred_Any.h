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

#ifndef __ZValPred_Any_h__
#define __ZValPred_Any_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"

#include "zoolib/ZVal_Any.h"
#include "zoolib/ValPred/ZValPred.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZValComparand_Const_Any

class ZValComparand_Const_Any : public ZValComparand
	{
public:
	ZValComparand_Const_Any(const ZVal_Any& iVal);

// Our protocol
	const ZVal_Any& GetVal() const;

private:
	const ZVal_Any fVal;
	};

template <>
int sCompare_T(const ZValComparand_Const_Any& iL, const ZValComparand_Const_Any& iR);

// =================================================================================================
// MARK: - ZValComparator_Callable_Any

class ZValComparator_Callable_Any : public ZValComparator
	{
public:
	typedef Callable<bool(const ZVal_Any& iLHS, const ZVal_Any& iRHS)> Callable_t;

	ZValComparator_Callable_Any(ZRef<Callable_t> iCallable);

// Our protocol
	const ZRef<Callable_t>& GetCallable() const;

private:
	ZRef<Callable_t> fCallable;
	};

template <>
int sCompare_T(const ZValComparator_Callable_Any& iL, const ZValComparator_Callable_Any& iR);

// =================================================================================================
// MARK: - ZValComparator_StringContains

class ZValComparator_StringContains : public ZValComparator
	{
public:
	ZValComparator_StringContains(int iStrength);

// Our protocol
	int GetStrength() const;

private:
	int fStrength;
	};

template <>
int sCompare_T(const ZValComparator_StringContains& iL, const ZValComparator_StringContains& iR);

// =================================================================================================
// MARK: - Comparand pseudo constructors

ZValComparandPseudo CConst(const ZVal_Any& iVal);

// =================================================================================================
// MARK: -

bool sMatches(const ZValPred& iValPred, const ZVal_Any& iVal);

} // namespace ZooLib

#endif // __ZValPred_Any_h__
