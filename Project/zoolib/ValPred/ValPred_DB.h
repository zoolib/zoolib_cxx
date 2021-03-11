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

// From ValComparand
	virtual int Compare(const ZP<ValComparand>& iOther);

// Our protocol
	const Val_DB& GetVal() const;

protected:
	Val_DB fVal;
	};

// =================================================================================================
#pragma mark - ValComparator_Callable_DB

class ValComparator_Callable_DB : public ValComparator
	{
public:
	typedef Callable<bool(const Val_DB& iLHS, const Val_DB& iRHS)> Callable_t;

	ValComparator_Callable_DB(ZP<Callable_t> iCallable);

// From ValComparator
	virtual int Compare(const ZP<ValComparator>& iOther);

// Our protocol
	const ZP<Callable_t>& GetCallable() const;

private:
	ZP<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark - ValComparator_StringContains

class ValComparator_StringContains : public ValComparator
	{
public:
	ValComparator_StringContains(int iStrength);

// From ValComparator
	virtual int Compare(const ZP<ValComparator>& iOther);

// Our protocol
	int GetStrength() const;

private:
	int fStrength;
	};

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
