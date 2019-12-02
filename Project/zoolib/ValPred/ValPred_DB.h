// Copyright (c) 2010-2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ValPred_ValPred_DB_h__
#define __ZooLib_ValPred_ValPred_DB_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"
#include "zoolib/Val_DB.h"

#include "zoolib/ValPred/ValPred.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ValComparand_Const_DB

class ValComparand_Const_DB : public ValComparand
	{
public:
	ValComparand_Const_DB(const Val_DB& iVal);

// Our protocol
	const Val_DB& GetVal() const;

protected:
	Val_DB fVal;
	};

template <>
int sCompare_T(const ValComparand_Const_DB& iL, const ValComparand_Const_DB& iR);

// =================================================================================================
#pragma mark - ValComparator_Callable_DB

class ValComparator_Callable_DB : public ValComparator
	{
public:
	typedef Callable<bool(const Val_DB& iLHS, const Val_DB& iRHS)> Callable_t;

	ValComparator_Callable_DB(ZP<Callable_t> iCallable);

// Our protocol
	const ZP<Callable_t>& GetCallable() const;

private:
	ZP<Callable_t> fCallable;
	};

template <>
int sCompare_T(const ValComparator_Callable_DB& iL, const ValComparator_Callable_DB& iR);

// =================================================================================================
#pragma mark - ValComparator_StringContains

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
#pragma mark - Comparand pseudo constructors

ValComparandPseudo CConst(const Val_DB& iVal);

// Explicit, for when Val_DB is not a Val_ZZ and doesn't have the special case stuff therein.
ValComparandPseudo CConst(const UTF8* iVal);

// =================================================================================================
#pragma mark -

bool sMatches(const ValPred& iValPred, const Val_DB& iVal);

} // namespace ZooLib

#endif // __ZooLib_ValPred_ValPred_DB_h__
