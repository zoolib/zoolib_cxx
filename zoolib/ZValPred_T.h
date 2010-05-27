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

#ifndef __ZValPred_T__
#define __ZValPred_T__ 1
#include "zconfig.h"

#include "zoolib/ZRef_Counted.h"
#include "zoolib/ZTrail.h"
#include "zoolib/ZUtil_STL_set.h"

#include <map>

namespace ZooLib {

// There is an equivalent typedef in ZRA_RelHead, and it's possible that it
// should be promoted to its own file, but for now that seems excessive.
typedef std::map<std::string, std::string> Rename_t;

class ZValContext
	{};

template <class Val> class ZValComparand_T;

template <class Val> class ZValPred_T;

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparator_T

template <class Val>
class ZValComparator_T : public ZCounted
	{
protected:
	ZValComparator_T();

public:
	virtual ~ZValComparator_T();

	virtual bool Matches(const Val& iLHS, const Val& iRHS) = 0;
	};

template <class Val>
ZValComparator_T<Val>::ZValComparator_T()
	{}

template <class Val>
ZValComparator_T<Val>::~ZValComparator_T()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparator_Simple_T

template <class Val>
class ZValComparator_Simple_T : public ZValComparator_T<Val>
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

	ZValComparator_Simple_T(EComparator iEComparator);

// From ZValComparator_T
	virtual bool Matches(const Val& iLHS, const Val& iRHS);

// Our protocol
	EComparator GetEComparator();

private:
	const EComparator fEComparator;
	};

template <class Val>
ZValComparator_Simple_T<Val>::ZValComparator_Simple_T(EComparator iEComparator)
:	fEComparator(iEComparator)
	{}

template <class Val>
bool ZValComparator_Simple_T<Val>::Matches(const Val& iLHS, const Val& iRHS)
	{
	int compare = sCompare_T(iLHS, iRHS);
	switch (fEComparator)
		{
		case eLT: return compare < 0;
		case eLE: return compare <= 0;
		case eEQ: return compare == 0;
		case eGE: return compare >= 0;
		case eGT: return compare > 0;
		}
	ZUnimplemented();
	return false;
	}

template <class Val>
typename ZValComparator_Simple_T<Val>::EComparator
ZValComparator_Simple_T<Val>::GetEComparator()
	{ return fEComparator; }

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparator_StringContains_T

template <class Val>
class ZValComparator_StringContains_T : public ZValComparator_T<Val>
	{
public:
	ZValComparator_StringContains_T(int iStrength);

// From ZValComparator_T
	virtual bool Matches(const Val& iLHS, const Val& iRHS);

private:
	const int fStrength;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparator_SeqContains_T

template <class Val>
class ZValComparator_SeqContains_T : public ZValComparator_T<Val>
	{
public:
	ZValComparator_SeqContains_T();

// From ZValComparator_T
	virtual bool Matches(const Val& iLHS, const Val& iRHS);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparator_Regex_T

template <class Val>
class ZValComparator_Regex_T : public ZValComparator_T<Val>
	{
public:
	ZValComparator_Regex_T();

// From ZValComparator_T
	virtual bool Matches(const Val& iLHS, const Val& iRHS);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparand_T

template <class Val>
class ZValComparand_T : public ZCounted
	{
protected:
	ZValComparand_T();

public:
	virtual ~ZValComparand_T();

	virtual Val GetVal(ZValContext& iContext, const Val& iVal) = 0;
	virtual std::set<std::string> GetNames();
	virtual ZRef<ZValComparand_T> Renamed(const Rename_t& iRename);
	};

template <class Val>
ZValComparand_T<Val>::ZValComparand_T()
	{}

template <class Val>
ZValComparand_T<Val>::~ZValComparand_T()
	{}

template <class Val>
std::set<std::string> ZValComparand_T<Val>::GetNames()
	{ return std::set<std::string>(); }

template <class Val>
ZRef<ZValComparand_T<Val> > ZValComparand_T<Val>::Renamed(const Rename_t& iRename)
	{ return this; }

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparand_Const_T

template <class Val>
class ZValComparand_Const_T : public ZValComparand_T<Val>
	{
public:
	ZValComparand_Const_T(const Val& iVal);

// From ZValComparand_T
	virtual Val GetVal(ZValContext& iContext, const Val& iVal);

// Our protocol
	Val GetVal();

private:
	const Val fVal;
	};

template <class Val>
ZValComparand_Const_T<Val>::ZValComparand_Const_T(const Val& iVal)
:	fVal(iVal)
	{}

template <class Val>
Val ZValComparand_Const_T<Val>::GetVal(ZValContext& iContext, const Val& iVal)
	{ return fVal; }

template <class Val>
Val ZValComparand_Const_T<Val>::GetVal()
	{ return fVal; }

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparand_Trail_T

template <class Val>
class ZValComparand_Trail_T : public ZValComparand_T<Val>
	{
public:
	ZValComparand_Trail_T(const ZTrail& iTrail);

// From ZValComparand_T
	virtual Val GetVal(ZValContext& iContext, const Val& iVal);
	virtual std::set<std::string> GetNames();
	virtual ZRef<ZValComparand_T<Val> > Renamed(const Rename_t& iRename);

// Our protocol
	const ZTrail& GetTrail();

private:
	const ZTrail fTrail;
	};

template <class Val>
ZValComparand_Trail_T<Val>::ZValComparand_Trail_T(const ZTrail& iTrail)
:	fTrail(iTrail)
	{}

template <class Val>
Val ZValComparand_Trail_T<Val>::GetVal(ZValContext& iContext, const Val& iVal)
	{
	Val theVal = iVal;
	for (size_t x = 0, theCount = fTrail.Count(); x < theCount; ++x)
		theVal = theVal.GetMap().Get(fTrail.At(x));

	return theVal;
	}

template <class Val>
std::set<std::string> ZValComparand_Trail_T<Val>::GetNames()
	{
	std::set<std::string> theNames;
	if (fTrail.Count())
		theNames.insert(fTrail.At(0));
	return theNames;
	}

template <class Val>
const ZTrail& ZValComparand_Trail_T<Val>::GetTrail()
	{ return fTrail; }

template <class Val>
ZRef<ZValComparand_T<Val> > ZValComparand_Trail_T<Val>::Renamed(const Rename_t& iRename)
	{
	if (fTrail.Count())
		{
		std::string theName = fTrail.At(0);
		Rename_t::const_iterator i = iRename.find(theName);
		if (i != iRename.end())
			return new ZValComparand_Trail_T((*i).second + fTrail.SubTrail(1));
		}
	return this;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparand_Var_T

template <class Val>
class ZValComparand_Var_T : public ZValComparand_T<Val>
	{
public:
	ZValComparand_Var_T(const std::string& iVarName);

// From ZValComparand_T
	virtual Val GetVal(ZValContext& iContext, const Val& iVal);

// Our protocol
	std::string GetVarName();

private:
	const std::string fVarName;
	};

template <class Val>
ZValComparand_Var_T<Val>::ZValComparand_Var_T(const std::string& iVarName)
:	fVarName(iVarName)
	{}

template <class Val>
Val ZValComparand_Var_T<Val>::GetVal(ZValContext& iContext, const Val& iVal)
	{
	ZUnimplemented();
	return Val();
	}

template <class Val>
std::string ZValComparand_Var_T<Val>::GetVarName()
	{ return fVarName; }

// =================================================================================================
#pragma mark -
#pragma mark * ZValPred_T

template <class Val>
class ZValPred_T
	{
public:
	typedef ZValComparand_T<Val> Comparand;
	typedef ZValComparator_T<Val> Comparator;

	ZValPred_T();
	ZValPred_T(const ZValPred_T& iOther);
	~ZValPred_T();
	ZValPred_T& operator=(const ZValPred_T& iOther);

	ZValPred_T(
		const ZRef<Comparand>& iLHS,
		const ZRef<Comparator>& iComparator,
		const ZRef<Comparand>& iRHS);

	ZRef<Comparand> GetLHS() const;
	ZRef<Comparator> GetComparator() const;
	ZRef<Comparand> GetRHS() const;

	bool Matches(ZValContext& iContext, const Val& iVal) const;

	std::set<std::string> GetNames() const;

	bool Renamed(const Rename_t& iRename, ZValPred_T& oResult) const;

	ZValPred_T Renamed(const Rename_t& iRename) const;

private:
	ZRef<Comparand> fLHS;
	ZRef<Comparator> fComparator;
	ZRef<Comparand> fRHS;
	};

template <class Val>
ZValPred_T<Val>::ZValPred_T()
	{}

template <class Val>
ZValPred_T<Val>::ZValPred_T(const ZValPred_T& iOther)
:	fLHS(iOther.fLHS)
,	fComparator(iOther.fComparator)
,	fRHS(iOther.fRHS)
	{}

template <class Val>
ZValPred_T<Val>::~ZValPred_T()
	{}

template <class Val>
ZValPred_T<Val>&
ZValPred_T<Val>::operator=(const ZValPred_T& iOther)
	{
	fLHS = iOther.fLHS;
	fComparator = iOther.fComparator;
	fRHS = iOther.fRHS;
	return *this;
	}

template <class Val>
ZValPred_T<Val>::ZValPred_T(
	const ZRef<Comparand>& iLHS,
	const ZRef<Comparator>& iComparator,
	const ZRef<Comparand>& iRHS)
:	fLHS(iLHS)
,	fComparator(iComparator)
,	fRHS(iRHS)
	{}

template <class Val>
ZRef<typename ZValPred_T<Val>::Comparand>
ZValPred_T<Val>::GetLHS() const
	{ return fLHS; }

template <class Val>
ZRef<typename ZValPred_T<Val>::Comparator>
ZValPred_T<Val>::GetComparator() const
	{ return fComparator; }

template <class Val>
ZRef<typename ZValPred_T<Val>::Comparand>
ZValPred_T<Val>::GetRHS() const
	{ return fRHS; }

template <class Val>
bool ZValPred_T<Val>::Matches(ZValContext& iContext, const Val& iVal) const
	{ return fComparator->Matches(fLHS->GetVal(iContext, iVal), fRHS->GetVal(iContext, iVal)); }

template <class Val>
std::set<std::string> ZValPred_T<Val>::GetNames() const
	{ return ZUtil_STL_set::sOr(fLHS->GetNames(), fRHS->GetNames()); }

template <class Val>
bool ZValPred_T<Val>::Renamed(const Rename_t& iRename, ZValPred_T& oResult) const
	{
	ZRef<ZValComparand_T<Val> > newLHS = fLHS->Renamed(iRename);
	ZRef<ZValComparand_T<Val> > newRHS = fRHS->Renamed(iRename);
	if (newLHS == fLHS && newRHS == fRHS)
		return false;
	oResult = ZValPred_T(newLHS, fComparator, newRHS);
	return true;
	}

template <class Val>
ZValPred_T<Val> ZValPred_T<Val>::Renamed(const Rename_t& iRename) const
	{ return ZValPred_T(fLHS->Renamed(iRename), fComparator, fRHS->Renamed(iRename)); }

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparandPseudo_T

// This subclass of ZRef<ZValComparand_T<Val> > is returned by the C-prefixed pseudo constructor
// functions so we can use overloaded operators to construct a ZValPred_T<Val>.

template <class Val>
class ZValComparandPseudo_T
:	public ZRef<ZValComparand_T<Val> >
	{
public:
	ZValComparandPseudo_T(ZValComparand_T<Val>* iRep)
	:	ZRef<ZValComparand_T<Val> >(iRep)
		{}
	};

// =================================================================================================
#pragma mark -
#pragma mark * Comparand pseudo constructors

template <class Val>
ZValComparandPseudo_T<Val>
CConst_T(const char* iVal)
	{ return new ZValComparand_Const_T<Val>(std::string(iVal)); }

template <class Val>
ZValComparandPseudo_T<Val>
CConst_T(const Val& iVal)
	{ return new ZValComparand_Const_T<Val>(iVal); }

template <class Val>
ZValComparandPseudo_T<Val>
CName_T(const std::string& iName)
	{ return new ZValComparand_Trail_T<Val>(ZTrail(&iName, &iName + 1)); }

template <class Val>
ZValComparandPseudo_T<Val>
CTrail_T(const ZTrail& iTrail)
	{ return new ZValComparand_Trail_T<Val>(iTrail); }

template <class Val>
ZValComparandPseudo_T<Val>
CVal_T()
	{ return new ZValComparand_Trail_T<Val>(ZTrail()); }

// FIXME
// I'm not sure that we need Var -- when we're using ZValPred in relational queries
// we get the same effect by joining an explicit single-result relation with the target
// against which we're matching the condition.

template <class Val>
ZValComparandPseudo_T<Val>
CVar_T(const std::string& iVarName)
	{ return new ZValComparand_Var_T<Val>(iVarName); }

// =================================================================================================
#pragma mark -
#pragma mark * Comparison operators taking comparands and returning a condition

template <class Val>
ZValPred_T<Val> operator<(
	const ZValComparandPseudo_T<Val>& iLHS,
	const ZValComparandPseudo_T<Val>& iRHS)
	{
	return ZValPred_T<Val>
		(iLHS,
		new ZValComparator_Simple_T<Val>(ZValComparator_Simple_T<Val>::eLT),
		iRHS);
	}

template <class Val>
ZValPred_T<Val> operator<=(
	const ZValComparandPseudo_T<Val>& iLHS,
	const ZValComparandPseudo_T<Val>& iRHS)
	{
	return ZValPred_T<Val>
		(iLHS,
		new ZValComparator_Simple_T<Val>(ZValComparator_Simple_T<Val>::eLE),
		iRHS);
	}

template <class Val>
ZValPred_T<Val> operator==(
	const ZValComparandPseudo_T<Val>& iLHS,
	const ZValComparandPseudo_T<Val>& iRHS)
	{
	return ZValPred_T<Val>
		(iLHS,
		new ZValComparator_Simple_T<Val>(ZValComparator_Simple_T<Val>::eEQ),
		iRHS);
	}

template <class Val>
ZValPred_T<Val> operator>=(
	const ZValComparandPseudo_T<Val>& iLHS,
	const ZValComparandPseudo_T<Val>& iRHS)
	{
	return ZValPred_T<Val>
		(iLHS,
		new ZValComparator_Simple_T<Val>(ZValComparator_Simple_T<Val>::eGE),
		iRHS);
	}

template <class Val>
ZValPred_T<Val> operator>(
	const ZValComparandPseudo_T<Val>& iLHS,
	const ZValComparandPseudo_T<Val>& iRHS)
	{
	return ZValPred_T<Val>
		(iLHS,
		new ZValComparator_Simple_T<Val>(ZValComparator_Simple_T<Val>::eGT),
		iRHS);
	}

} // namespace ZooLib

#endif // __ZValPred_T__
