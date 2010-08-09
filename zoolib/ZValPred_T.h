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

#include "zoolib/ZCompare_T.h"
#include "zoolib/ZRef_Counted.h"
#include "zoolib/ZUtil_STL_set.h"

#include <map>
#include <string>

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

template <class Val> class ZValComparator_Simple_T;
template <class Val> class ZValComparator_StringContains_T;
template <class Val> class ZValComparator_SeqContains_T;
template <class Val> class ZValComparator_Regex_T;

template <class Val>
class ZValComparator_T : public ZCounted
	{
protected:
	ZValComparator_T();

public:
	virtual ~ZValComparator_T();

	virtual bool Matches(const Val& iLHS, const Val& iRHS) = 0;

	int Compare(ZRef<ZValComparator_T> iOther);

	virtual int pRevCompare(ZValComparator_T<Val>* iOther) = 0;
	virtual int pCompare_Simple(ZValComparator_Simple_T<Val>* iOther) = 0;
	virtual int pCompare_StringContains(ZValComparator_StringContains_T<Val>* iOther) = 0;
	virtual int pCompare_SeqContains(ZValComparator_SeqContains_T<Val>* iOther) = 0;
	virtual int pCompare_Regex(ZValComparator_Regex_T<Val>* iOther) = 0;
	};

template <class Val>
ZValComparator_T<Val>::ZValComparator_T()
	{}

template <class Val>
ZValComparator_T<Val>::~ZValComparator_T()
	{}

template <class Val>
int ZValComparator_T<Val>::Compare(ZRef<ZValComparator_T> iOther)
	{
	// An extant node is greater than an absent node.
	if (!iOther)
		return 1;

	// VERY IMPORTANT. pRevCompare returns 1 if the callee is less than
	// the param. So we invoke it on iOther to get the sense that callers expect.
	return iOther.Get()->pRevCompare(this);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparand_GT bases

template <class Val>
class ZValComparator_GT_T : public ZValComparator_T<Val>
	{
public:
// From ZValComparand_T<Val>
	virtual int pCompare_Simple(ZValComparator_Simple_T<Val>* iOther)
		{ return -1 ;}

	virtual int pCompare_StringContains(ZValComparator_StringContains_T<Val>* iOther)
		{ return -1 ;}

	virtual int pCompare_SeqContains(ZValComparator_SeqContains_T<Val>* iOther)
		{ return -1 ;}

	virtual int pCompare_Regex(ZValComparator_Regex_T<Val>* iOther)
		{ return -1 ;}
	};

template <class Val>
class ZValComparator_GT_Simple_T : public ZValComparator_GT_T<Val>
	{
public:
	virtual int pCompare_Simple(ZValComparator_Simple_T<Val>* iOther)
		{ return 1 ;}
	};

template <class Val>
class ZValComparator_GT_StringContains_T : public ZValComparator_GT_Simple_T<Val>
	{
public:
	virtual int pCompare_StringContains(ZValComparator_StringContains_T<Val>* iOther)
		{ return 1 ;}
	};

template <class Val>
class ZValComparator_GT_SeqContains_T : public ZValComparator_GT_StringContains_T<Val>
	{
public:
	virtual int pCompare_SeqContains(ZValComparator_SeqContains_T<Val>* iOther)
		{ return 1 ;}
	};

template <class Val>
class ZValComparator_GT_Regex_T : public ZValComparator_GT_SeqContains_T<Val>
	{
public:
	virtual int pCompare_Regex(ZValComparator_Regex_T<Val>* iOther)
		{ return 1 ;}
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparator_Simple_T

template <class Val>
class ZValComparator_Simple_T : public ZValComparator_GT_T<Val>
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

	virtual int pRevCompare(ZValComparator_T<Val>* iOther);
	virtual int pCompare_Simple(ZValComparator_Simple_T<Val>* iOther);

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
	const int compare = sCompare_T(iLHS, iRHS);
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

template <class Val>
int ZValComparator_Simple_T<Val>::pRevCompare(ZValComparator_T<Val>* iOther)
	{ return iOther->pCompare_Simple(this); }

template <class Val>
int ZValComparator_Simple_T<Val>::pCompare_Simple(ZValComparator_Simple_T<Val>* iOther)
	{ return sCompare_T(int(fEComparator), int(iOther->fEComparator)); }

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparator_StringContains_T

template <class Val>
class ZValComparator_StringContains_T : public ZValComparator_GT_Simple_T<Val>
	{
public:
	ZValComparator_StringContains_T(int iStrength);

// From ZValComparator_T
	virtual bool Matches(const Val& iLHS, const Val& iRHS);

	virtual int pRevCompare(ZValComparator_T<Val>* iOther);
	virtual int pCompare_StringContains(ZValComparator_StringContains_T<Val>* iOther);

private:
	const int fStrength;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparator_SeqContains_T

template <class Val>
class ZValComparator_SeqContains_T : public ZValComparator_GT_StringContains_T<Val>
	{
public:
	ZValComparator_SeqContains_T();

// From ZValComparator_T
	virtual bool Matches(const Val& iLHS, const Val& iRHS);

	virtual int pRevCompare(ZValComparator_T<Val>* iOther);
	virtual int pCompare_SeqContains(ZValComparator_SeqContains_T<Val>* iOther);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparator_Regex_T

template <class Val>
class ZValComparator_Regex_T : public ZValComparator_GT_SeqContains_T<Val>
	{
public:
	ZValComparator_Regex_T();

// From ZValComparator_T
	virtual bool Matches(const Val& iLHS, const Val& iRHS);

	virtual int pRevCompare(ZValComparator_T<Val>* iOther);
	virtual int pCompare_Regex(ZValComparator_Regex_T<Val>* iOther);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparand_T

template <class Val> class ZValComparand_Const_T;
template <class Val> class ZValComparand_Name_T;
template <class Val> class ZValComparand_Var_T;

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

	int Compare(ZRef<ZValComparand_T> iOther);

	virtual int pRevCompare(ZValComparand_T<Val>* iOther) = 0;
	virtual int pCompare_Const(ZValComparand_Const_T<Val>* iOther) = 0;
	virtual int pCompare_Name(ZValComparand_Name_T<Val>* iOther) = 0;
	virtual int pCompare_Var(ZValComparand_Var_T<Val>* iOther) = 0;
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

template <class Val>
int ZValComparand_T<Val>::Compare(ZRef<ZValComparand_T> iOther)
	{
	// An extant node is greater than an absent node.
	if (!iOther)
		return 1;

	// VERY IMPORTANT. pRevCompare returns 1 if the callee is less than
	// the param. So we invoke it on iOther to get the sense that callers expect.
	return iOther.Get()->pRevCompare(this);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparand_GT bases

template <class Val>
class ZValComparand_GT_T : public ZValComparand_T<Val>
	{
public:
// From ZValComparand_T<Val>
	virtual int pCompare_Const(ZValComparand_Const_T<Val>* iOther)
		{ return -1 ;}

	virtual int pCompare_Name(ZValComparand_Name_T<Val>* iOther)
		{ return -1 ;}

	virtual int pCompare_Var(ZValComparand_Var_T<Val>* iOther)
		{ return -1 ;}
	};

template <class Val>
class ZValComparand_GT_Const_T : public ZValComparand_GT_T<Val>
	{
public:
	virtual int pCompare_Const(ZValComparand_Const_T<Val>* iOther)
		{ return 1 ;}
	};

template <class Val>
class ZValComparand_GT_Name_T : public ZValComparand_GT_T<Val>
	{
public:
	virtual int pCompare_Name(ZValComparand_Name_T<Val>* iOther)
		{ return 1 ;}
	};

template <class Val>
class ZValComparand_GT_Var_T : public ZValComparand_GT_T<Val>
	{
public:
	virtual int pCompare_Var(ZValComparand_Var_T<Val>* iOther)
		{ return 1 ;}
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparand_Const_T

template <class Val>
class ZValComparand_Const_T : public ZValComparand_GT_T<Val>
	{
public:
	ZValComparand_Const_T(const Val& iVal);

// From ZValComparand_T
	virtual Val GetVal(ZValContext& iContext, const Val& iVal);

	virtual int pRevCompare(ZValComparand_T<Val>* iOther);
	virtual int pCompare_Const(ZValComparand_Const_T<Val>* iOther);

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

template <class Val>
int ZValComparand_Const_T<Val>::pRevCompare(ZValComparand_T<Val>* iOther)
	{ return iOther->pCompare_Const(this); }

template <class Val>
int ZValComparand_Const_T<Val>::pCompare_Const(ZValComparand_Const_T<Val>* iOther)
	{ return sCompare_T(fVal, iOther->fVal); }

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparand_Name_T

template <class Val>
class ZValComparand_Name_T : public ZValComparand_GT_Const_T<Val>
	{
public:
	ZValComparand_Name_T(const std::string& iName);

// From ZValComparand_T
	virtual Val GetVal(ZValContext& iContext, const Val& iVal);
	virtual std::set<std::string> GetNames();
	virtual ZRef<ZValComparand_T<Val> > Renamed(const Rename_t& iRename);

	virtual int pRevCompare(ZValComparand_T<Val>* iOther);
	virtual int pCompare_Name(ZValComparand_Name_T<Val>* iOther);

// Our protocol
	const std::string& GetName();

private:
	const std::string fName;
	};

template <class Val>
ZValComparand_Name_T<Val>::ZValComparand_Name_T(const std::string& iName)
:	fName(iName)
	{}

template <class Val>
Val ZValComparand_Name_T<Val>::GetVal(ZValContext& iContext, const Val& iVal)
	{ return iVal.GetMap().Get(fName); }

template <class Val>
std::set<std::string> ZValComparand_Name_T<Val>::GetNames()
	{ return std::set<std::string>(&fName, &fName + 1); }

template <class Val>
const std::string& ZValComparand_Name_T<Val>::GetName()
	{ return fName; }

template <class Val>
ZRef<ZValComparand_T<Val> > ZValComparand_Name_T<Val>::Renamed(const Rename_t& iRename)
	{
	Rename_t::const_iterator i = iRename.find(fName);
	if (i != iRename.end())
		return new ZValComparand_Name_T(i->second);
	return this;
	}

template <class Val>
int ZValComparand_Name_T<Val>::pRevCompare(ZValComparand_T<Val>* iOther)
	{ return iOther->pCompare_Name(this); }

template <class Val>
int ZValComparand_Name_T<Val>::pCompare_Name(ZValComparand_Name_T<Val>* iOther)
	{ return sCompare_T(fName, iOther->fName); }

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparand_Var_T

template <class Val>
class ZValComparand_Var_T : public ZValComparand_GT_Name_T<Val>
	{
public:
	ZValComparand_Var_T(const std::string& iVarName);

// From ZValComparand_T
	virtual Val GetVal(ZValContext& iContext, const Val& iVal);

	virtual int pRevCompare(ZValComparand_T<Val>* iOther);
	virtual int pCompare_Var(ZValComparand_Var_T<Val>* iOther);

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

template <class Val>
int ZValComparand_Var_T<Val>::pRevCompare(ZValComparand_T<Val>* iOther)
	{ return iOther->pCompare_Var(this); }

template <class Val>
int ZValComparand_Var_T<Val>::pCompare_Var(ZValComparand_Var_T<Val>* iOther)
	{ return sCompare_T(fVarName, iOther->fVarName); }

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
	{ return new ZValComparand_Name_T<Val>(iName); }

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

// =================================================================================================
#pragma mark -
#pragma mark * sCompare_T

template <class Val>
int sCompare_T(const ZValPred_T<Val>& iL, const ZValPred_T<Val>& iR)
	{
	if (int compare = iL.GetLHS()->Compare(iR.GetLHS()))
		return compare;

	if (int compare = iL.GetComparator()->Compare(iR.GetComparator()))
		return compare;

	return iL.GetRHS()->Compare(iR.GetRHS());
	}

} // namespace ZooLib

#endif // __ZValPred_T__
