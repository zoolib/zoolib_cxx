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

#include "ZTQL_RelHead.h"
#include "ZTuple.h"

#include <set>

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::ComparatorRep

namespace ZTQL {

class ComparatorRep : public ZRefCounted
	{
protected:
	ComparatorRep();

public:
	virtual ~ComparatorRep();

	virtual bool Matches(const ZTValue& iLHS, const ZTValue& iRHS) = 0;
	};

} // namespace ZTQL

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::ComparatorRep_Simple

namespace ZTQL {

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
	virtual bool Matches(const ZTValue& iLHS, const ZTValue& iRHS);

// Our protocol
	EComparator GetEComparator();

private:
	EComparator fEComparator;
	};

} // namespace ZTQL

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::ComparatorRep_StringContains

namespace ZTQL {

class ComparatorRep_StringContains : public ComparatorRep
	{
public:
	ComparatorRep_StringContains(int iStrength);

// From ComparatorRep
	virtual bool Matches(const ZTValue& iLHS, const ZTValue& iRHS);

private:
	int fStrength;
	};

} // namespace ZTQL

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::ComparatorRep_VectorContains

namespace ZTQL {

class ComparatorRep_VectorContains : public ComparatorRep
	{
public:
	ComparatorRep_VectorContains();

// From ComparatorRep
	virtual bool Matches(const ZTValue& iLHS, const ZTValue& iRHS);

private:
	};

} // namespace ZTQL

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::ComparatorRep_Regex

namespace ZTQL {

class ComparatorRep_Regex : public ComparatorRep
	{
public:
	ComparatorRep_Regex(const std::string& iPattern, int iStrength);

// From ComparatorRep
	virtual bool Matches(const ZTValue& iLHS, const ZTValue& iRHS);

private:
	std::string fPattern;
	int fStrength;
	};

} // namespace ZTQL

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::Comparator

namespace ZTQL {

class Comparator
	{
public:
	Comparator();
	Comparator(ZRef<ComparatorRep> iRep);

	ZRef<ComparatorRep> GetRep() const;

	bool Matches(const ZTValue& iLHS, const ZTValue& iRHS) const;

private:
	ZRef<ComparatorRep> fRep;
	};

} // namespace ZTQL

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::ComparandRep

namespace ZTQL {

class ComparandRep : public ZRefCounted
	{
protected:
	ComparandRep();

public:
	virtual ~ComparandRep();

	virtual const ZTValue& Imp_GetValue(const ZTuple& iTuple) = 0;
	virtual void GatherPropNames(std::set<ZTName>& ioNames);
	};

} // namespace ZTQL

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::ComparandRep_Name

namespace ZTQL {

class ComparandRep_Name : public ComparandRep
	{
public:
	ComparandRep_Name(const ZTName& iName);

// From ComparandRep
	virtual const ZTValue& Imp_GetValue(const ZTuple& iTuple);
	virtual void GatherPropNames(std::set<ZTName>& ioNames);

// Our protocol
	const ZTName& GetName();

private:
	ZTName fName;
	};

} // namespace ZTQL

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::ComparandRep_Value

namespace ZTQL {

class ComparandRep_Value : public ComparandRep
	{
public:
	ComparandRep_Value(const ZTValue& iValue);

// From ComparandRep
	virtual const ZTValue& Imp_GetValue(const ZTuple& iTuple);

// Our protocol
	const ZTValue& GetValue();

private:
	ZTValue fValue;
	};

} // namespace ZTQL

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::Comparand

namespace ZTQL {

class Condition;

class Comparand
	{
public:
	Comparand();
	Comparand(const Comparand& iOther);
	Comparand(ZRef<ComparandRep> iRep);
	~Comparand();
	Comparand& operator=(const Comparand& iOther);

	ZRef<ComparandRep> GetRep() const;

	const ZTValue& GetValue(const ZTuple& iTuple) const;
	void GatherPropNames(std::set<ZTName>& ioNames) const;

	Condition LT(const Comparand& iRHS) const;
	Condition LE(const Comparand& iRHS) const;
	Condition EQ(const Comparand& iRHS) const;
	Condition GE(const Comparand& iRHS) const;
	Condition GT(const Comparand& iRHS) const;

private:
	ZRef<ComparandRep> fRep;
	};

} // namespace ZTQL

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::Condition

namespace ZTQL {

class Condition
	{
public:
	Condition(ZRef<ComparandRep> iLHS, ZRef<ComparatorRep> iComparator, ZRef<ComparandRep> iRHS);
	Condition(const Comparand& iLHS, const Comparator& iComparator, const Comparand& iRHS);

	const Comparand& GetLHS() const;
	const Comparator& GetComparator() const;
	const Comparand& GetRHS() const;

	bool Matches(const ZTuple& iTuple) const;
	void GatherPropNames(std::set<ZTName>& ioNames) const;

private:
	Comparand fLHS;
	Comparator fComparator;
	Comparand fRHS;
	};

} // namespace ZTQL

#endif // __ZTQL_Condition__
