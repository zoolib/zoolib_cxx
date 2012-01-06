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

#ifndef __ZValPred_h__
#define __ZValPred_h__ 1
#include "zconfig.h"

#include "zoolib/ZCounted.h"

#include <string>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparator

class ZValComparator : public ZCounted
	{
protected:
	ZValComparator();

public:
	virtual ~ZValComparator();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparator_Simple

class ZValComparator_Simple : public ZValComparator
	{
public:
	enum EComparator { eLT, eLE, eEQ, eNE, eGE, eGT };

	ZValComparator_Simple(EComparator iEComparator);

// Our protocol
	EComparator GetEComparator() const;

private:
	const EComparator fEComparator;
	};

template<>
int sCompare_T(const ZValComparator_Simple& iL, const ZValComparator_Simple& iR);

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparand

class ZValComparand : public ZCounted
	{
protected:
	ZValComparand();

public:
	virtual ~ZValComparand();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparand_Name

class ZValComparand_Name : public ZValComparand
	{
public:
	ZValComparand_Name(const std::string& iName);

// Our protocol
	const std::string& GetName() const;

private:
	const std::string fName;
	};

template<>
int sCompare_T(const ZValComparand_Name& iL, const ZValComparand_Name& iR);

// =================================================================================================
#pragma mark -
#pragma mark * ZValPred

class ZValPred
	{
public:
	ZValPred();
	ZValPred(const ZValPred& iOther);
	~ZValPred();

	ZValPred& operator=(const ZValPred& iOther);

	ZValPred
		(const ZRef<ZValComparand>& iLHS,
		const ZRef<ZValComparator>& iComparator,
		const ZRef<ZValComparand>& iRHS);

	const ZRef<ZValComparand>& GetLHS() const;
	const ZRef<ZValComparator>& GetComparator() const;
	const ZRef<ZValComparand>& GetRHS() const;

private:
	ZRef<ZValComparand> fLHS;
	ZRef<ZValComparator> fComparator;
	ZRef<ZValComparand> fRHS;
	};

template<>
int sCompare_T(const ZValPred& iL, const ZValPred& iR);

bool operator<(const ZValPred& iL, const ZValPred& iR);

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparandPseudo

// This subclass of ZRef<ZValComparand> is returned by the C-prefixed pseudo constructor
// functions so we can more easily use overloaded operators to construct a ZValPred.

class ZValComparandPseudo
:	public ZRef<ZValComparand>
	{
public:
	ZValComparandPseudo(ZValComparand* iRep);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Comparand pseudo constructors

ZValComparandPseudo CName(const std::string& iName);

// =================================================================================================
#pragma mark -
#pragma mark * Comparison operators taking comparands and returning a condition

ZValPred operator<(const ZValComparandPseudo& iLHS, const ZValComparandPseudo& iRHS);
ZValPred operator<=(const ZValComparandPseudo& iLHS, const ZValComparandPseudo& iRHS);
ZValPred operator==(const ZValComparandPseudo& iLHS, const ZValComparandPseudo& iRHS);
ZValPred operator!=(const ZValComparandPseudo& iLHS, const ZValComparandPseudo& iRHS);
ZValPred operator>=(const ZValComparandPseudo& iLHS, const ZValComparandPseudo& iRHS);
ZValPred operator>(const ZValComparandPseudo& iLHS, const ZValComparandPseudo& iRHS);

} // namespace ZooLib

#endif // __ZValPred_h__
