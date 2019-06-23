/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#ifndef __ZooLib_ValPred_ValPred_h__
#define __ZooLib_ValPred_ValPred_h__ 1
#include "zconfig.h"

#include "zoolib/Compare_T.h"
#include "zoolib/Counted.h"

#include <string>

namespace ZooLib {

// =================================================================================================
#pragma mark - ValComparator

class ValComparator : public Counted
	{
protected:
	ValComparator();

public:
	virtual ~ValComparator();
	};

// =================================================================================================
#pragma mark - ValComparator_Simple

class ValComparator_Simple : public ValComparator
	{
public:
	enum EComparator { eLT, eLE, eEQ, eNE, eGE, eGT };

	ValComparator_Simple(EComparator iEComparator);

// Our protocol
	EComparator GetEComparator() const;

private:
	const EComparator fEComparator;
	};

template <>
int sCompare_T(const ValComparator_Simple& iL, const ValComparator_Simple& iR);

// =================================================================================================
#pragma mark - ValComparand

class ValComparand : public Counted
	{
protected:
	ValComparand();

public:
	virtual ~ValComparand();
	};

// =================================================================================================
#pragma mark - ValComparand_Name

class ValComparand_Name : public ValComparand
	{
public:
	ValComparand_Name(const std::string& iName);

// Our protocol
	const std::string& GetName() const;

private:
	const std::string fName;
	};

template <>
int sCompare_T(const ValComparand_Name& iL, const ValComparand_Name& iR);

// =================================================================================================
#pragma mark - ValPred

class ValPred
	{
public:
	ValPred();
	ValPred(const ValPred& iOther);
	~ValPred();

	ValPred& operator=(const ValPred& iOther);

	ValPred(
		const ZP<ValComparand>& iLHS,
		const ZP<ValComparator>& iComparator,
		const ZP<ValComparand>& iRHS);

	const ZP<ValComparand>& GetLHS() const;
	const ZP<ValComparator>& GetComparator() const;
	const ZP<ValComparand>& GetRHS() const;

private:
	ZP<ValComparand> fLHS;
	ZP<ValComparator> fComparator;
	ZP<ValComparand> fRHS;
	};

template <>
int sCompare_T(const ValPred& iL, const ValPred& iR);

bool operator<(const ValPred& iL, const ValPred& iR);

// =================================================================================================
#pragma mark - ValComparandPseudo

// This subclass of ZP<ValComparand> is returned by the C-prefixed pseudo constructor
// functions so we can more easily use overloaded operators to construct a ValPred.

class ValComparandPseudo
:	public ZP<ValComparand>
	{
	typedef ZP<ValComparand> inherited;
public:
	ValComparandPseudo(ValComparand* iRep)
	:	inherited(iRep)
		{}

	ValComparandPseudo(const ZP<ValComparand>& iOther)
	:	inherited(iOther)
		{}

	ValComparandPseudo(const ValComparandPseudo& iOther)
	:	inherited(iOther)
		{}
	};

// =================================================================================================
#pragma mark - Comparand pseudo constructors

ValComparandPseudo CName(const std::string& iName);

// =================================================================================================
#pragma mark - Comparison operators taking comparands and returning a condition

inline ValPred operator<(const ZP<ValComparand>& iLHS, const ValComparandPseudo& iRHS)
	{ return ValPred(iLHS, new ValComparator_Simple(ValComparator_Simple::eLT), iRHS); }

inline ValPred operator<(const ValComparandPseudo& iLHS, const ZP<ValComparand>& iRHS)
	{ return ValPred(iLHS, new ValComparator_Simple(ValComparator_Simple::eLT), iRHS); }

inline ValPred operator<(const ValComparandPseudo& iLHS, const ValComparandPseudo& iRHS)
	{ return ValPred(iLHS, new ValComparator_Simple(ValComparator_Simple::eLT), iRHS); }

inline ValPred operator<=(const ZP<ValComparand>& iLHS, const ValComparandPseudo& iRHS)
	{ return ValPred(iLHS, new ValComparator_Simple(ValComparator_Simple::eLE), iRHS); }

inline ValPred operator<=(const ValComparandPseudo& iLHS, const ZP<ValComparand>& iRHS)
	{ return ValPred(iLHS, new ValComparator_Simple(ValComparator_Simple::eLE), iRHS); }

inline ValPred operator<=(const ValComparandPseudo& iLHS, const ValComparandPseudo& iRHS)
	{ return ValPred(iLHS, new ValComparator_Simple(ValComparator_Simple::eLE), iRHS); }

inline ValPred operator==(const ZP<ValComparand>& iLHS, const ValComparandPseudo& iRHS)
	{ return ValPred(iLHS, new ValComparator_Simple(ValComparator_Simple::eEQ), iRHS); }

inline ValPred operator==(const ValComparandPseudo& iLHS, const ZP<ValComparand>& iRHS)
	{ return ValPred(iLHS, new ValComparator_Simple(ValComparator_Simple::eEQ), iRHS); }

inline ValPred operator==(const ValComparandPseudo& iLHS, const ValComparandPseudo& iRHS)
	{ return ValPred(iLHS, new ValComparator_Simple(ValComparator_Simple::eEQ), iRHS); }

inline ValPred operator!=(const ZP<ValComparand>& iLHS, const ValComparandPseudo& iRHS)
	{ return ValPred(iLHS, new ValComparator_Simple(ValComparator_Simple::eNE), iRHS); }

inline ValPred operator!=(const ValComparandPseudo& iLHS, const ZP<ValComparand>& iRHS)
	{ return ValPred(iLHS, new ValComparator_Simple(ValComparator_Simple::eNE), iRHS); }

inline ValPred operator!=(const ValComparandPseudo& iLHS, const ValComparandPseudo& iRHS)
	{ return ValPred(iLHS, new ValComparator_Simple(ValComparator_Simple::eNE), iRHS); }

inline ValPred operator>(const ZP<ValComparand>& iLHS, const ValComparandPseudo& iRHS)
	{ return ValPred(iLHS, new ValComparator_Simple(ValComparator_Simple::eGT), iRHS); }

inline ValPred operator>(const ValComparandPseudo& iLHS, const ZP<ValComparand>& iRHS)
	{ return ValPred(iLHS, new ValComparator_Simple(ValComparator_Simple::eGT), iRHS); }

inline ValPred operator>(const ValComparandPseudo& iLHS, const ValComparandPseudo& iRHS)
	{ return ValPred(iLHS, new ValComparator_Simple(ValComparator_Simple::eGT), iRHS); }

inline ValPred operator>=(const ZP<ValComparand>& iLHS, const ValComparandPseudo& iRHS)
	{ return ValPred(iLHS, new ValComparator_Simple(ValComparator_Simple::eGE), iRHS); }

inline ValPred operator>=(const ValComparandPseudo& iLHS, const ZP<ValComparand>& iRHS)
	{ return ValPred(iLHS, new ValComparator_Simple(ValComparator_Simple::eGE), iRHS); }

inline ValPred operator>=(const ValComparandPseudo& iLHS, const ValComparandPseudo& iRHS)
	{ return ValPred(iLHS, new ValComparator_Simple(ValComparator_Simple::eGE), iRHS); }

} // namespace ZooLib

#endif // __ZooLib_ValPred_ValPred_h__
