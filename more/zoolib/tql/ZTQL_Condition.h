/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZTQL_Condition__
#define __ZTQL_Condition__ 1

#include "zconfig.h"

#include "zoolib/tql/ZTQL_RelHead.h"
#include "zoolib/tql/ZTQL_Val.h"

#include <set>

NAMESPACE_ZOOLIB_BEGIN

namespace ZTQL {

// =================================================================================================
#pragma mark -
#pragma mark * ComparatorRep

class ComparatorRep : public ZRefCounted
	{
protected:
	ComparatorRep();

public:
	virtual ~ComparatorRep();

	virtual bool Matches(const Val& iLHS, const Val& iRHS) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ComparatorRep_Simple

class ComparatorRep_Simple : public ComparatorRep
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

	ComparatorRep_Simple(EComparator iEComparator);

// From ComparatorRep
	virtual bool Matches(const Val& iLHS, const Val& iRHS);

// Our protocol
	EComparator GetEComparator();

private:
	const EComparator fEComparator;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ComparatorRep_StringContains

class ComparatorRep_StringContains : public ComparatorRep
	{
public:
	ComparatorRep_StringContains(int iStrength);

// From ComparatorRep
	virtual bool Matches(const Val& iLHS, const Val& iRHS);

private:
	const int fStrength;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ComparatorRep_VectorContains

class ComparatorRep_VectorContains : public ComparatorRep
	{
public:
	ComparatorRep_VectorContains();

// From ComparatorRep
	virtual bool Matches(const Val& iLHS, const Val& iRHS);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ComparatorRep_Regex

class ComparatorRep_Regex : public ComparatorRep
	{
public:
	ComparatorRep_Regex(const std::string& iPattern, int iStrength);

// From ComparatorRep
	virtual bool Matches(const Val& iLHS, const Val& iRHS);

private:
	const std::string fPattern;
	const int fStrength;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Comparator

class Comparator
	{
public:
	Comparator();
	Comparator(ZRef<ComparatorRep> iRep);

	ZRef<ComparatorRep> GetRep() const;

	bool Matches(const Val& iLHS, const Val& iRHS) const;

private:
	ZRef<ComparatorRep> fRep;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ComparandRep

class ComparandRep : public ZRefCounted
	{
protected:
	ComparandRep();

public:
	virtual ~ComparandRep();

	virtual const Val& Imp_GetVal(const Map& iMap) = 0;
	virtual void GatherPropNames(std::set<ZTName>& ioNames);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ComparandRep_Name

class ComparandRep_Name : public ComparandRep
	{
public:
	ComparandRep_Name(const ZTName& iName);

// From ComparandRep
	virtual const Val& Imp_GetVal(const Map& iMap);
	virtual void GatherPropNames(std::set<ZTName>& ioNames);

// Our protocol
	const ZTName& GetName();

private:
	const ZTName fName;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ComparandRep_Val

class ComparandRep_Val : public ComparandRep
	{
public:
	ComparandRep_Val(const Val& iVal);

// From ComparandRep
	virtual const Val& Imp_GetVal(const Map& iMap);

// Our protocol
	const Val& GetVal();

private:
	const Val fVal;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Comparand

class Condition;

class Comparand
	{
public:
	Comparand();
	Comparand(const Comparand& iOther);
	~Comparand();
	Comparand& operator=(const Comparand& iOther);

	Comparand(ZRef<ComparandRep> iRep);

	ZRef<ComparandRep> GetRep() const;

	const Val& GetVal(const Map& iMap) const;
	void GatherPropNames(std::set<ZTName>& ioNames) const;

	Condition LT(const Comparand& iRHS) const;
	Condition LE(const Comparand& iRHS) const;
	Condition EQ(const Comparand& iRHS) const;
	Condition GE(const Comparand& iRHS) const;
	Condition GT(const Comparand& iRHS) const;

private:
	ZRef<ComparandRep> fRep;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Condition

class Condition
	{
public:
	Condition();
	Condition(const Condition& iOther);
	~Condition();
	Condition& operator=(const Condition& iOther);

	Condition(ZRef<ComparandRep> iLHS, ZRef<ComparatorRep> iComparator, ZRef<ComparandRep> iRHS);
	Condition(const Comparand& iLHS, const Comparator& iComparator, const Comparand& iRHS);

	const Comparand& GetLHS() const;
	const Comparator& GetComparator() const;
	const Comparand& GetRHS() const;

	bool Matches(const Map& iMap) const;
	void GatherPropNames(std::set<ZTName>& ioNames) const;

private:
	Comparand fLHS;
	Comparator fComparator;
	Comparand fRHS;
	};

} // namespace ZTQL

NAMESPACE_ZOOLIB_END

#endif // __ZTQL_Condition__
