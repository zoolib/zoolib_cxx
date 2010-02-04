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

#ifndef __ZValCondition_T__
#define __ZValCondition_T__ 1

#include "zconfig.h"

#include "zoolib/ZRef_Counted.h"
#include "zoolib/ZRelHead.h"
#include "zoolib/ZTrail.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparatorRep

template <class Val>
class ZValComparatorRep : public ZRefCounted
	{
protected:
	ZValComparatorRep();

public:
	virtual ~ZValComparatorRep();

	virtual bool Matches(const Val& iLHS, const Val& iRHS) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparatorRep_Simple

template <class Val>
class ZValComparatorRep_Simple : public ZValComparatorRep<Val>
	{
public:
	enum EComparator
		{
		eLT,
		eLE,
		eEQ,
		eGE,
		eGT
		};

	ZValComparatorRep_Simple(EComparator iEComparator);

// From ZValComparatorRep
	virtual bool Matches(const Val& iLHS, const Val& iRHS);

// Our protocol
	EComparator GetEComparator();

private:
	const EComparator fEComparator;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparatorRep_StringContains

template <class Val>
class ZValComparatorRep_StringContains : public ZValComparatorRep<Val>
	{
public:
	ZValComparatorRep_StringContains(int iStrength);

// From ZValComparatorRep
	virtual bool Matches(const Val& iLHS, const Val& iRHS);

private:
	const int fStrength;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparatorRep_SeqContains

template <class Val>
class ZValComparatorRep_SeqContains : public ZValComparatorRep<Val>
	{
public:
	ZValComparatorRep_SeqContains();

// From ZValComparatorRep
	virtual bool Matches(const Val& iLHS, const Val& iRHS);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparatorRep_Regex

template <class Val>
class ZValComparatorRep_Regex : public ZValComparatorRep<Val>
	{
public:
	ZValComparatorRep_Regex();

// From ZValComparatorRep
	virtual bool Matches(const Val& iLHS, const Val& iRHS);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValContext

class ZValContext
	{};

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparandRep

template <class Val>
class ZValComparandRep : public ZRefCountedWithFinalize
	{
protected:
	ZValComparandRep();

public:
	virtual ~ZValComparandRep();

	virtual Val Imp_GetVal(ZValContext& iContext, const Val& iVal) = 0;
	virtual ZRelHead GetRelHead();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparandRep_Const

template <class Val>
class ZValComparandRep_Const : public ZValComparandRep<Val>
	{
public:
	ZValComparandRep_Const(const Val& iVal);

// From ZValComparandRep
	virtual Val Imp_GetVal(ZValContext& iContext, const Val& iVal);

// Our protocol
	Val GetVal();

private:
	const Val fVal;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparandRep_Trail

template <class Val>
class ZValComparandRep_Trail : public ZValComparandRep<Val>
	{
public:
	ZValComparandRep_Trail(const ZTrail& iTrail);

// From ZValComparandRep
	virtual Val Imp_GetVal(ZValContext& iContext, const Val& iVal);
	virtual ZRelHead GetRelHead();

// Our protocol
	const ZTrail& GetTrail();

private:
	const ZTrail fTrail;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparandRep_Var

template <class Val>
class ZValComparandRep_Var : public ZValComparandRep<Val>
	{
public:
	ZValComparandRep_Var(const std::string& iVarName);

// From ZValComparandRep
	virtual Val Imp_GetVal(ZValContext& iContext, const Val& iVal);

// Our protocol
	std::string GetVarName();

private:
	const std::string fVarName;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Comparand

template <class Val>
class ZValCondition_T;

template <class Val>
class ZValComparand_T : public ZRef<ZValComparandRep<Val> >
	{
	typedef ZRef<ZValComparandRep<Val> > inherited;
public:
	ZValComparand_T();
	ZValComparand_T(const ZValComparand_T& iOther);
	~ZValComparand_T();
	ZValComparand_T& operator=(const ZValComparand_T& iOther);

	ZValComparand_T(const ZRef<ZValComparandRep<Val> >& iRep);
	ZValComparand_T& operator=(const ZRef<ZValComparandRep<Val> >& iRep);

	Val GetVal(ZValContext& iContext, const Val& iVal) const;
	ZRelHead GetRelHead() const;

	ZValCondition_T<Val> LT(const ZRef<ZValComparandRep<Val> >& iRHS) const;
	ZValCondition_T<Val> LE(const ZRef<ZValComparandRep<Val> >& iRHS) const;
	ZValCondition_T<Val> EQ(const ZRef<ZValComparandRep<Val> >& iRHS) const;
	ZValCondition_T<Val> GE(const ZRef<ZValComparandRep<Val> >& iRHS) const;
	ZValCondition_T<Val> GT(const ZRef<ZValComparandRep<Val> >& iRHS) const;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValCondition_T

template <class Val>
class ZValCondition_T
	{
public:
	ZValCondition_T();
	ZValCondition_T(const ZValCondition_T& iOther);
	~ZValCondition_T();
	ZValCondition_T& operator=(const ZValCondition_T& iOther);

	ZValCondition_T(
		const ZRef<ZValComparandRep<Val> >& iLHS,
		const ZRef<ZValComparatorRep<Val> >& iComparator,
		const ZRef<ZValComparandRep<Val> >& iRHS);

	ZValComparand_T<Val> GetLHS() const;
	ZRef<ZValComparatorRep<Val> > GetComparator() const;
	ZValComparand_T<Val> GetRHS() const;

	bool Matches(ZValContext& iContext, const Val& iVal) const;

	ZRelHead GetRelHead() const;

private:
	ZValComparand_T<Val> fLHS;
	ZRef<ZValComparatorRep<Val> > fComparator;
	ZValComparand_T<Val> fRHS;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValCondition_T and ZValComparand_T 

// Not at all sure about these -- they make it possible to write
// infix comparison statements, but also break our ability to use
// Comparands in the normal fashion.

template <class Val>
ZValComparand_T<Val> CConst_T(const char* iVal);

template <class Val>
ZValComparand_T<Val> CConst_T(const Val& iVal);

template <class Val>
ZValComparand_T<Val> CName_T(const std::string& iName);

template <class Val>
ZValComparand_T<Val> CTrail_T(const ZTrail& iTrail);

template <class Val>
ZValComparand_T<Val> CVal_T();

template <class Val>
ZValComparand_T<Val> CVar_T(const std::string& iVarName);

NAMESPACE_ZOOLIB_END

#endif // __ZValCondition_T__
