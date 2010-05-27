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

#include "zoolib/ZExpr_Logic_ValPred.h"
#include "zoolib/ZStrim.h"
#include "zoolib/ZStrim_Escaped.h"
#include "zoolib/ZTime.h"
#include "zoolib/ZUtil_Strim.h"
#include "zoolib/ZVisitor_Expr_Logic_ValPred_DoToStrim.h"
#include "zoolib/ZVisitor_Expr_Op_DoTransform_T.h"

#include "zoolib/zra/ZRA_Expr_Rel_Product.h"
#include "zoolib/zra/ZRA_Expr_Rel_Project.h"
#include "zoolib/zra/ZRA_Expr_Rel_Rename.h"
#include "zoolib/zra/ZRA_Expr_Rel_Restrict.h"
#include "zoolib/zra/ZRA_Expr_Rel_Select.h"
#include "zoolib/zra/ZRA_SQL.h"

#include <map>

namespace ZooLib {
namespace ZRA {
namespace SQL {

using std::map;
using std::set;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * DoRename (anonymous)

namespace { // anonymous

class DoRename
:	public virtual ZVisitor_Expr_Op_DoTransform_T<ZExpr_Logic>
,	public virtual ZVisitor_Expr_Logic_ValPred
	{
public:
	DoRename(const Rename_t& iRename);

	virtual void Visit_Expr_Logic_ValPred(ZRef<ZExpr_Logic_ValPred> iExpr);
private:
	const Rename_t& fRename;
	};

DoRename::DoRename(const Rename_t& iRename)
:	fRename(iRename)
	{}

void DoRename::Visit_Expr_Logic_ValPred(ZRef<ZExpr_Logic_ValPred> iExpr)
	{
	ZValPred result;
	if (iExpr->GetValPred().Renamed(fRename, result))
		this->pSetResult(new ZExpr_Logic_ValPred(result));
	else
		this->pSetResult(iExpr);
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * MakeSFW (anonymous)

namespace { // anonymous

class MakeSFW
:	public virtual ZVisitor_Expr_Op_DoTransform_T<Expr_Rel_SFW>
,	public virtual Visitor_Expr_Rel_Product
,	public virtual Visitor_Expr_Rel_Concrete
,	public virtual Visitor_Expr_Rel_Project
,	public virtual Visitor_Expr_Rel_Rename
,	public virtual Visitor_Expr_Rel_Restrict
,	public virtual Visitor_Expr_Rel_Select
	{
public:
	virtual void Visit_Expr_Op0(ZRef<ZExpr_Op0_T<ZRef<Expr_Rel_SFW> > > iExpr);
	virtual void Visit_Expr_Op1(ZRef<ZExpr_Op1_T<ZRef<Expr_Rel_SFW> > > iExpr);
	virtual void Visit_Expr_Op2(ZRef<ZExpr_Op2_T<ZRef<Expr_Rel_SFW> > > iExpr);

	virtual void Visit_Expr_Rel_Product(ZRef<Expr_Rel_Product> iExpr);

	virtual void Visit_Expr_Rel_Concrete(ZRef<Expr_Rel_Concrete> iExpr);

	virtual void Visit_Expr_Rel_Project(ZRef<Expr_Rel_Project> iExpr);
	virtual void Visit_Expr_Rel_Rename(ZRef<Expr_Rel_Rename> iExpr);
	virtual void Visit_Expr_Rel_Restrict(ZRef<Expr_Rel_Restrict> iExpr);
	virtual void Visit_Expr_Rel_Select(ZRef<Expr_Rel_Select> iExpr);
	};

void MakeSFW::Visit_Expr_Op0(ZRef<ZExpr_Op0_T<ZRef<Expr_Rel_SFW> > > iExpr)
	{ ZUnimplemented(); }

void MakeSFW::Visit_Expr_Op1(ZRef<ZExpr_Op1_T<ZRef<Expr_Rel_SFW> > > iExpr)
	{ ZUnimplemented(); }

void MakeSFW::Visit_Expr_Op2(ZRef<ZExpr_Op2_T<ZRef<Expr_Rel_SFW> > > iExpr)
	{ ZUnimplemented(); }

void MakeSFW::Visit_Expr_Rel_Product(ZRef<Expr_Rel_Product> iExpr)
	{
	ZRef<Expr_Rel_SFW> sfw0 = this->Do(iExpr->GetOp0());
	ZRef<Expr_Rel_SFW> sfw1 = this->Do(iExpr->GetOp1());

	Rename_t theRename = sfw0->GetRename();
	const Rename_t& theRename1 = sfw1->GetRename();
	theRename.insert(theRename1.begin(), theRename1.end());
	
	vector<ZRef<Expr_Rel_Concrete> > rels = sfw0->GetRels();
	const vector<ZRef<Expr_Rel_Concrete> > rels1 = sfw1->GetRels();
	rels.insert(rels.end(), rels1.begin(), rels1.end());

	ZRef<Expr_Rel_SFW> result = new Expr_Rel_SFW(
		theRename,
		sfw0->GetRelHead() | sfw1->GetRelHead(),
		sfw0->GetCondition() & sfw1->GetCondition(),
		rels);

	this->pSetResult(result);
	}

void MakeSFW::Visit_Expr_Rel_Concrete(ZRef<Expr_Rel_Concrete> iExpr)
	{
	ZRef<Expr_Rel_SFW> result = new Expr_Rel_SFW(
		Rename_t(),
		iExpr->GetRelHead(),
		sTrue(),
		vector<ZRef<Expr_Rel_Concrete> >(1, iExpr));

	this->pSetResult(result);
	}

void MakeSFW::Visit_Expr_Rel_Project(ZRef<Expr_Rel_Project> iExpr)
	{
	// Descend
	ZRef<Expr_Rel_SFW> sfw0 = this->Do(iExpr->GetOp0());

	ZRef<Expr_Rel_SFW> result = new Expr_Rel_SFW(
		sfw0->GetRename(),
		sfw0->GetRelHead() & iExpr->GetProjectRelHead(),
		sfw0->GetCondition(),
		sfw0->GetRels());

	this->pSetResult(result);
	}

static RelHead spRenamed(RelHead iRelHead, const string8& iOld, const string8& iNew)
	{
	if (iRelHead.Contains(iOld))
		{
		iRelHead -= iOld;
		iRelHead |= iNew;
		}
	return iRelHead;
	}

void MakeSFW::Visit_Expr_Rel_Rename(ZRef<Expr_Rel_Rename> iExpr)
	{
	ZRef<Expr_Rel_SFW> sfw0 = this->Do(iExpr->GetOp0());

	const string8& oldName = iExpr->GetOld();
	const string8& newName = iExpr->GetNew();
	Rename_t theRename = sfw0->GetRename();
	bool foundIt = false;
	for (Rename_t::iterator i = theRename.begin(); i != theRename.end(); /*no inc*/)
		{
		if ((*i).second == oldName)
			{
			(*i).second = newName;
			foundIt = true;
			}
		}

	if (!foundIt)
		theRename[oldName] = newName;

	ZRef<Expr_Rel_SFW> result = new Expr_Rel_SFW(
		theRename,
		spRenamed(sfw0->GetRelHead(), oldName, newName),
		sfw0->GetCondition(),
		sfw0->GetRels());

	this->pSetResult(result);	
	}

static ZValPred spRenamedInverse(
	const ZValPred& iValPred, const Rename_t& iRename)
	{
	ZValPred result;
	if (iValPred.Renamed(sInvert(iRename), result))
		return result;
	return iValPred;
	}

static ZRef<ZExpr_Logic> spRenamedInverse(
	ZRef<ZExpr_Logic> iExpr_Logic, const Rename_t& iRename)
	{ return DoRename(sInvert(iRename)).Do(iExpr_Logic); }

void MakeSFW::Visit_Expr_Rel_Restrict(ZRef<Expr_Rel_Restrict> iExpr)
	{
	ZRef<Expr_Rel_SFW> sfw0 = this->Do(iExpr->GetOp0());

	const Rename_t& theRename = sfw0->GetRename();	
	ZRef<Expr_Rel_SFW> result = new Expr_Rel_SFW(
		theRename,
		sfw0->GetRelHead(),
		sfw0->GetCondition() & spRenamedInverse(iExpr->GetValPred(), theRename),
		sfw0->GetRels());

	this->pSetResult(result);
	}

void MakeSFW::Visit_Expr_Rel_Select(ZRef<Expr_Rel_Select> iExpr)
	{
	ZRef<Expr_Rel_SFW> sfw0 = this->Do(iExpr->GetOp0());

	const Rename_t& theRename = sfw0->GetRename();	
	ZRef<Expr_Rel_SFW> result = new Expr_Rel_SFW(
		theRename,
		sfw0->GetRelHead(),
		sfw0->GetCondition() & spRenamedInverse(iExpr->GetExpr_Logic(), theRename),
		sfw0->GetRels());

	this->pSetResult(result);
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_SFW

Expr_Rel_SFW::Expr_Rel_SFW(const Rename_t& iRename,
	const RelHead& iRelHead,
	ZRef<ZExpr_Logic> iCondition,
	const vector<ZRef<Expr_Rel_Concrete> >& iRels)
:	fRename(iRename)
,	fRelHead(iRelHead)
,	fCondition(iCondition)
,	fRels(iRels)
	{}

RelHead Expr_Rel_SFW::GetRelHead()
	{ return fRelHead; }

const Rename_t& Expr_Rel_SFW::GetRename()
	{ return fRename; }

ZRef<ZExpr_Logic> Expr_Rel_SFW::GetCondition()
	{ return fCondition; }

const vector<ZRef<Expr_Rel_Concrete> >& Expr_Rel_SFW::GetRels()
	{ return fRels; }

// =================================================================================================
#pragma mark -
#pragma mark * ZRA::SQL::sConvert

ZRef<Expr_Rel_SFW> sConvert(ZRef<Expr_Rel> iExpr)
	{ return MakeSFW().Do(iExpr); }

// =================================================================================================
#pragma mark -
#pragma mark * ToStrim_SQL (anonymous)

namespace { // anonymous

class ToStrim_SQL
:	public virtual ZVisitor_DoToStrim
,	public virtual ZVisitor_Expr_Logic_True
,	public virtual ZVisitor_Expr_Logic_False
,	public virtual ZVisitor_Expr_Logic_Not
,	public virtual ZVisitor_Expr_Logic_And
,	public virtual ZVisitor_Expr_Logic_Or
,	public virtual ZVisitor_Expr_Logic_ValPred
	{
public:
	virtual void Visit_Expr_Logic_True(ZRef<ZExpr_Logic_True> iRep);
	virtual void Visit_Expr_Logic_False(ZRef<ZExpr_Logic_False> iRep);
	virtual void Visit_Expr_Logic_Not(ZRef<ZExpr_Logic_Not> iRep);
	virtual void Visit_Expr_Logic_And(ZRef<ZExpr_Logic_And> iRep);
	virtual void Visit_Expr_Logic_Or(ZRef<ZExpr_Logic_Or> iRep);
	virtual void Visit_Expr_Logic_ValPred(ZRef<ZExpr_Logic_ValPred> iRep);
	};

void ToStrim_SQL::Visit_Expr_Logic_True(ZRef<ZExpr_Logic_True> iRep)
	{ pStrimW() << "1"; }

void ToStrim_SQL::Visit_Expr_Logic_False(ZRef<ZExpr_Logic_False> iRep)
	{ pStrimW() << "0"; }

void ToStrim_SQL::Visit_Expr_Logic_Not(ZRef<ZExpr_Logic_Not> iRep)
	{
	pStrimW() << " NOT (";
	this->pDoToStrim(iRep->GetOp0());
	pStrimW() << ")";
	}

void ToStrim_SQL::Visit_Expr_Logic_And(ZRef<ZExpr_Logic_And> iRep)
	{
	pStrimW() << "(";
	this->pDoToStrim(iRep->GetOp0());
	pStrimW() << " AND ";
	this->pDoToStrim(iRep->GetOp1());
	pStrimW() << ")";
	}

void ToStrim_SQL::Visit_Expr_Logic_Or(ZRef<ZExpr_Logic_Or> iRep)
	{
	pStrimW() << "(";
	this->pDoToStrim(iRep->GetOp0());
	pStrimW() << " OR ";
	this->pDoToStrim(iRep->GetOp1());
	pStrimW() << ")";
	}

typedef ZValComparator_Simple_T<ZVal_Expr> ZValComparator_Simple;
typedef ZValComparand_Trail_T<ZVal_Expr> ZValComparand_Trail;
typedef ZValComparand_Var_T<ZVal_Expr> ZValComparand_Var;
typedef ZValComparand_Const_T<ZVal_Expr> ZValComparand_Const;

// =================================================================================================
#pragma mark -
#pragma mark * Static helper functions

static void spToStrim_SimpleValue(const ZStrimW& s, const ZAny& iAny)
	{
	int64 asInt64;

	if (false)
		{}
	else if (!iAny)
		{
		s << "null";
		}
	else if (const string8* theValue = iAny.PGet_T<string8>())
		{
		ZStrimW_Escaped::Options theOptions;
		theOptions.fQuoteQuotes = true;
		theOptions.fEscapeHighUnicode = false;
		s << "'";
		ZStrimW_Escaped(theOptions, s).Write(*theValue);
		s << "'";
		}
	else if (const bool* theValue = iAny.PGet_T<bool>())
		{
		if (*theValue)
			s.Write("1");
		else
			s.Write("0");
		}
	else if (sQCoerceInt(iAny, asInt64))
		{
		s.Writef("%lld", asInt64);
		}
	else if (const float* asFloat = iAny.PGet_T<float>())
		{
		ZUtil_Strim::sWriteExact(s, *asFloat);
		}
	else if (const double* asDouble = iAny.PGet_T<double>())
		{
		ZUtil_Strim::sWriteExact(s, *asDouble);
		}
	else if (const ZTime* asTime = iAny.PGet_T<ZTime>())
		{
		ZUtil_Strim::sWriteExact(s, asTime->fVal);
		}
	else
		{
		s << "null /*!! Unhandled: " << iAny.Type().name() << " !!*/";
		}
	}

static void spWrite_PropName(const string8& iName, const ZStrimW& s)
	{
	s << iName;
	}

static void spToStrim(const ZRef<ZValPred::Comparand>& iCR, const ZStrimW& s)
	{
	if (!iCR)
		{
		s << "!!Null Comparand!!";
		}
	else if (ZRef<ZValComparand_Trail> cr = iCR.DynamicCast<ZValComparand_Trail>())
		{
		const ZTrail& theTrail = cr->GetTrail();
		if (theTrail.Count() == 1)
			spWrite_PropName(theTrail.At(0), s);
		else
			spWrite_PropName("/" + theTrail.AsString(), s);
		}
	else if (ZRef<ZValComparand_Var> cr = iCR.DynamicCast<ZValComparand_Var>())
		{
		s << "$" << cr->GetVarName();
		}
	else if (ZRef<ZValComparand_Const> cr = iCR.DynamicCast<ZValComparand_Const>())
		{
		spToStrim_SimpleValue(s, cr->GetVal());
		}
	else
		{
		s << "!!Unknown Comparand!!";
		}
	}

static void spToStrim(const ZRef<ZValPred::Comparator>& iCR, const ZStrimW& s)
	{
	if (!iCR)
		{
		s << "!!Null Comparator!!";
		}
	else if (ZRef<ZValComparator_Simple> cr = iCR.DynamicCast<ZValComparator_Simple>())
		{
		switch (cr->GetEComparator())
			{
			case ZValComparator_Simple::eLT:
				{
				s << " < ";
				break;
				}
			case ZValComparator_Simple::eLE:
				{
				s << " <= ";
				break;
				}
			case ZValComparator_Simple::eEQ:
				{
				s << " = ";
				break;
				}
			case ZValComparator_Simple::eGE:
				{
				s << " >= ";
				break;
				}
			case ZValComparator_Simple::eGT:
				{
				s << " > ";
				break;
				}
			}
		}
	else
		{
		s << "!!Unknown Comparator!!";
		}
	}

void spToStrim(const ZValPred& iValPred, const ZStrimW& s)
	{
	spToStrim(iValPred.GetLHS(), s);
	spToStrim(iValPred.GetComparator(), s);
	spToStrim(iValPred.GetRHS(), s);
	}

void ToStrim_SQL::Visit_Expr_Logic_ValPred(
	ZRef<ZExpr_Logic_ValPred> iRep)
	{ spToStrim(iRep->GetValPred(), pStrimW()); }

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZRA::SQL::sConvert

void sAsSQL(ZRef<Expr_Rel_SFW> iSFW, const ZStrimW& s)
	{
	if (iSFW)
		{
		s << "SELECT";
		const RelHead& theRelHead = iSFW->GetRelHead();
		
		const Rename_t theRename = sInvert(iSFW->GetRename());

		const set<string8>& names = theRelHead.GetElems();

		bool isFirst = true;
		for (set<string8>::const_iterator i = names.begin(); i != names.end(); ++i)
			{
			if (!isFirst)
				s << ",";
			isFirst = false;

			const string8& theName = *i;
			Rename_t::const_iterator alias = theRename.find(theName);
			if (alias != theRename.end())
				{
				s << " " << (*alias).second << " AS " << theName;
				}
			else
				{
				s << " " << theName;
				}
			}

		s << " FROM";

		{
		bool isFirst = true;
		const vector<ZRef<Expr_Rel_Concrete> >& theRels = iSFW->GetRels();
		for (vector<ZRef<Expr_Rel_Concrete> >::const_iterator i = theRels.begin();
			i != theRels.end(); ++i)
			{
			if (!isFirst)
				s << ",";
			isFirst = false;
			s << " " << (*i)->GetName();
			}
		}

		s << " WHERE ";
		ToStrim_SQL().DoToStrim(ToStrim_SQL::Options(), s, iSFW->GetCondition()); 
		s << ";";
		}
	}

string8 sAsSQL(ZRef<Expr_Rel_SFW> iSFW)
	{
	string8 result;
	sAsSQL(iSFW, ZStrimW_String(result));
	return result;
	}

} // namespace SQL
} // namespace ZRA
} // namespace ZooLib
