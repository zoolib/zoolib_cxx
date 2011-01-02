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

#include "zoolib/ZLog.h"
#include "zoolib/ZStrim_Escaped.h"
#include "zoolib/ZString.h"
#include "zoolib/ZUtil_Any.h"
#include "zoolib/ZUtil_Strim.h"
#include "zoolib/ZVisitor_Do_T.h"
#include "zoolib/ZVisitor_Expr_Bool_ValPred_Any_DoToStrim.h"
#include "zoolib/ZVisitor_Expr_Op_DoTransform_T.h"
#include "zoolib/ZValPred_Any.h"
#include "zoolib/ZValPred_Rename.h"

#include "zoolib/zra/ZRA_Util_Strim_RelHead.h"

#include "zoolib/zra/ZRA_AsSQL.h"
#include "zoolib/zra/ZRA_Expr_Rel_Concrete.h"
#include "zoolib/zra/ZRA_Expr_Rel_Product.h"
#include "zoolib/zra/ZRA_Expr_Rel_Project.h"
#include "zoolib/zra/ZRA_Expr_Rel_Rename.h"
#include "zoolib/zra/ZRA_Expr_Rel_Restrict.h"
#include "zoolib/zra/ZRA_Expr_Rel_Select.h"
#include "zoolib/zra/ZRA_Util_RelOperators.h"

#include <set>

namespace ZooLib {
namespace ZRA {

using std::map;
using std::set;
using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * spRenamed (anonymous)

namespace { // anonymous

static ZValPred spRenamed(const Rename& iRename, const ZValPred& iValPred)
	{ return ZooLib::sRenamed(iRename, iValPred); }

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * DoRename (anonymous)

namespace { // anonymous

class DoRename
:	public virtual ZVisitor_Expr_Op_DoTransform_T<ZExpr_Bool>
,	public virtual ZVisitor_Expr_Bool_ValPred
	{
public:
	DoRename(const Rename& iRename);

	virtual void Visit_Expr_Bool_ValPred(const ZRef<ZExpr_Bool_ValPred>& iExpr);
private:
	const Rename& fRename;
	};

DoRename::DoRename(const Rename& iRename)
:	fRename(iRename)
	{}

void DoRename::Visit_Expr_Bool_ValPred(const ZRef<ZExpr_Bool_ValPred>& iExpr)
	{ this->pSetResult(new ZExpr_Bool_ValPred(spRenamed(fRename, iExpr->GetValPred()))); }

static ZRef<ZExpr_Bool> spRenamed(const Rename& iRename, ZRef<ZExpr_Bool> iExpr_Bool)
	{ return DoRename(iRename).Do(iExpr_Bool); }

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Thing (anonymous)

namespace { // anonymous

struct Thing
	{
	RelHead fRelHead_Physical;
	Rename fRename;
	Rename fRename_Inverse;
	ZRef<ZExpr_Bool> fCondition;
	};

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZRA::SQL::sAsSQL

class MakeThing
:	public virtual ZVisitor_Do_T<Thing>
,	public virtual Visitor_Expr_Rel_Product
,	public virtual Visitor_Expr_Rel_Project
,	public virtual Visitor_Expr_Rel_Rename
,	public virtual Visitor_Expr_Rel_Restrict
,	public virtual Visitor_Expr_Rel_Select
,	public virtual Visitor_Expr_Rel_Concrete
	{
public:
	MakeThing(const map<string8,RelHead>& iTables);

	virtual void Visit_Expr_Op1(const ZRef<ZExpr_Op1_T<ZRef<Expr_Rel> > >& iExpr);
	virtual void Visit_Expr_Op2(const ZRef<ZExpr_Op2_T<ZRef<Expr_Rel> > >& iExpr);
	virtual void Visit_Expr_Op0(const ZRef<ZExpr_Op0_T<ZRef<Expr_Rel> > >& iExpr);

	virtual void Visit_Expr_Rel_Product(const ZRef<Expr_Rel_Product>& iExpr);

	virtual void Visit_Expr_Rel_Project(const ZRef<Expr_Rel_Project>& iExpr);
	virtual void Visit_Expr_Rel_Rename(const ZRef<Expr_Rel_Rename>& iExpr);
	virtual void Visit_Expr_Rel_Restrict(const ZRef<Expr_Rel_Restrict>& iExpr);
	virtual void Visit_Expr_Rel_Select(const ZRef<Expr_Rel_Select>& iExpr);

	virtual void Visit_Expr_Rel_Concrete(const ZRef<Expr_Rel_Concrete>& iExpr);

	const map<string8,RelHead>& fTables;
	map<string8,int> fTablesUsed;
	};

MakeThing::MakeThing(const map<string8,RelHead>& iTables)
:	fTables(iTables)
	{}

void MakeThing::Visit_Expr_Op2(const ZRef<ZExpr_Op2_T<ZRef<Expr_Rel> > >& iExpr)
	{ ZUnimplemented(); }

void MakeThing::Visit_Expr_Op1(const ZRef<ZExpr_Op1_T<ZRef<Expr_Rel> > >& iExpr)
	{ ZUnimplemented(); }

void MakeThing::Visit_Expr_Op0(const ZRef<ZExpr_Op0_T<ZRef<Expr_Rel> > >& iExpr)
	{ ZUnimplemented(); }

void MakeThing::Visit_Expr_Rel_Product(const ZRef<Expr_Rel_Product>& iExpr)
	{
	Thing thing0 = this->Do(iExpr->GetOp0());
	const Thing thing1 = this->Do(iExpr->GetOp1());

	thing0.fRelHead_Physical |= thing1.fRelHead_Physical;
	thing0.fRename.insert(thing1.fRename.begin(), thing1.fRename.end());
	thing0.fRename_Inverse.insert(thing1.fRename_Inverse.begin(), thing1.fRename_Inverse.end());
	thing0.fCondition &= thing1.fCondition;
	
	this->pSetResult(thing0);
	}

void MakeThing::Visit_Expr_Rel_Project(const ZRef<Expr_Rel_Project>& iExpr)
	{
	Thing theThing = this->Do(iExpr->GetOp0());
	const RelHead& theRH = iExpr->GetProjectRelHead();
	RelHead newRelHead;
	for (RelHead::iterator i = theThing.fRelHead_Physical.begin();
		i != theThing.fRelHead_Physical.end(); ++i)
		{
		string theString1 = *i;
		string theString2 = ZUtil_STL::sGetMustContain(1, theThing.fRename_Inverse, theString1);
		if (ZUtil_STL::sContains(theRH, theString2))
			newRelHead.insert(theString1);
		}
	theThing.fRelHead_Physical.swap(newRelHead);
	this->pSetResult(theThing);
	}

void MakeThing::Visit_Expr_Rel_Restrict(const ZRef<Expr_Rel_Restrict>& iExpr)
	{
	Thing theThing = this->Do(iExpr->GetOp0());
	theThing.fCondition &= spRenamed(theThing.fRename, iExpr->GetValPred());
	this->pSetResult(theThing);
	}

void MakeThing::Visit_Expr_Rel_Select(const ZRef<Expr_Rel_Select>& iExpr)
	{
	Thing theThing = this->Do(iExpr->GetOp0());
	theThing.fCondition &= spRenamed(theThing.fRename, iExpr->GetExpr_Bool());
	this->pSetResult(theThing);
	}

void MakeThing::Visit_Expr_Rel_Rename(const ZRef<Expr_Rel_Rename>& iExpr)
	{
	Thing theThing = this->Do(iExpr->GetOp0());
	const RelName& oldName = iExpr->GetOld();
	const RelName& newName = iExpr->GetNew();
	const RelName orgName = ZUtil_STL::sEraseAndReturnIfContains(theThing.fRename, oldName).Get();
	const RelName orgNameInverse = ZUtil_STL::sEraseAndReturnIfContains(theThing.fRename_Inverse, orgName).Get();
	ZAssert(orgNameInverse == oldName);
	ZUtil_STL::sInsertMustNotContain(1, theThing.fRename, newName, orgName);
	ZUtil_STL::sInsertMustNotContain(1, theThing.fRename_Inverse, orgName, newName);

	this->pSetResult(theThing);
	}

void MakeThing::Visit_Expr_Rel_Concrete(const ZRef<Expr_Rel_Concrete>& iExpr)
	{
	// Identify the table.
	const RelHead& theRH = iExpr->GetConcreteRelHead();

	ZQ<map<string8,RelHead>::const_iterator> found;
	for (map<string8,RelHead>::const_iterator iter = fTables.begin(); iter != fTables.end(); ++iter)
		{
		if ((sPrefixInsert(iter->first + "_", iter->second) & theRH).size())
			{
			found = iter;
			break;
			}
		}
	if (!found)
		throw std::runtime_error("Couldn't find table");

	const string8 realTableName = found.Get()->first;
	const string8 realTableNameUnderscore = realTableName + "_";
	const int numericSuffix = fTablesUsed[realTableName]++;
	const string8 usedTableName = realTableName + ZStringf("%d",numericSuffix);
	const string8 usedTableNameDot = usedTableName + ".";

	Thing theThing;
	theThing.fCondition = sTrue();
	for (RelHead::const_iterator iter = theRH.begin(); iter != theRH.end(); ++iter)
		{
		const string8 attrName = *iter;
		const string8 fieldName = sPrefixErase(realTableNameUnderscore, attrName);
		const string8 physicalFieldName = usedTableNameDot + fieldName;
		theThing.fRelHead_Physical |= physicalFieldName;
		ZUtil_STL::sInsertMustNotContain(1, theThing.fRename, attrName, physicalFieldName);
		ZUtil_STL::sInsertMustNotContain(1, theThing.fRename_Inverse, physicalFieldName, attrName);
		}

	this->pSetResult(theThing);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ToStrim_SQL (anonymous)

namespace { // anonymous

class ToStrim_SQL
:	public virtual ZVisitor_DoToStrim
,	public virtual ZVisitor_Expr_Bool_True
,	public virtual ZVisitor_Expr_Bool_False
,	public virtual ZVisitor_Expr_Bool_Not
,	public virtual ZVisitor_Expr_Bool_And
,	public virtual ZVisitor_Expr_Bool_Or
,	public virtual ZVisitor_Expr_Bool_ValPred
	{
public:
	virtual void Visit_Expr_Bool_True(const ZRef<ZExpr_Bool_True>& iRep);
	virtual void Visit_Expr_Bool_False(const ZRef<ZExpr_Bool_False>& iRep);
	virtual void Visit_Expr_Bool_Not(const ZRef<ZExpr_Bool_Not>& iRep);
	virtual void Visit_Expr_Bool_And(const ZRef<ZExpr_Bool_And>& iRep);
	virtual void Visit_Expr_Bool_Or(const ZRef<ZExpr_Bool_Or>& iRep);
	virtual void Visit_Expr_Bool_ValPred(const ZRef<ZExpr_Bool_ValPred>& iRep);
	};

void ToStrim_SQL::Visit_Expr_Bool_True(const ZRef<ZExpr_Bool_True>& iRep)
	{ pStrimW() << "1"; }

void ToStrim_SQL::Visit_Expr_Bool_False(const ZRef<ZExpr_Bool_False>& iRep)
	{ pStrimW() << "0"; }

void ToStrim_SQL::Visit_Expr_Bool_Not(const ZRef<ZExpr_Bool_Not>& iRep)
	{
	pStrimW() << " NOT (";
	this->pDoToStrim(iRep->GetOp0());
	pStrimW() << ")";
	}

void ToStrim_SQL::Visit_Expr_Bool_And(const ZRef<ZExpr_Bool_And>& iRep)
	{
	pStrimW() << "(";
	this->pDoToStrim(iRep->GetOp0());
	pStrimW() << " AND ";
	this->pDoToStrim(iRep->GetOp1());
	pStrimW() << ")";
	}

void ToStrim_SQL::Visit_Expr_Bool_Or(const ZRef<ZExpr_Bool_Or>& iRep)
	{
	pStrimW() << "(";
	this->pDoToStrim(iRep->GetOp0());
	pStrimW() << " OR ";
	this->pDoToStrim(iRep->GetOp1());
	pStrimW() << ")";
	}

static void spToStrim_SimpleValue(const ZStrimW& s, const ZAny& iAny)
	{
	int64 asInt64;

	if (false)
		{}
	else if (!iAny)
		{
		s << "null";
		}
	else if (const string8* theValue = iAny.PGet<string8>())
		{
		ZStrimW_Escaped::Options theOptions;
		theOptions.fQuoteQuotes = true;
		theOptions.fEscapeHighUnicode = false;
		s << "'";
		ZStrimW_Escaped(theOptions, s).Write(*theValue);
		s << "'";
		}
	else if (const bool* theValue = iAny.PGet<bool>())
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
	else if (const float* asFloat = iAny.PGet<float>())
		{
		ZUtil_Strim::sWriteExact(s, *asFloat);
		}
	else if (const double* asDouble = iAny.PGet<double>())
		{
		ZUtil_Strim::sWriteExact(s, *asDouble);
		}
	else if (const ZTime* asTime = iAny.PGet<ZTime>())
		{
		ZUtil_Strim::sWriteExact(s, asTime->fVal);
		}
	else
		{
		s << "null /*!!Unhandled: " << iAny.Type().name() << "!!*/";
		}
	}

static void spWrite_PropName(const string8& iName, const ZStrimW& s)
	{
	s << iName;
	}

static void spToStrim(const ZRef<ZValComparand>& iCR, const ZStrimW& s)
	{
	if (!iCR)
		{
		s << "/*Null Comparand*/";
		}
	else if (ZRef<ZValComparand_Name> cr = iCR.DynamicCast<ZValComparand_Name>())
		{
		spWrite_PropName(cr->GetName(), s);
		}
	else if (ZRef<ZValComparand_Const_Any> cr = iCR.DynamicCast<ZValComparand_Const_Any>())
		{
		spToStrim_SimpleValue(s, cr->GetVal());
		}
	else
		{
		s << "/*Unknown Comparand*/";
		}
	}

static void spToStrim(const ZRef<ZValComparator>& iCR, const ZStrimW& s)
	{
	if (!iCR)
		{
		s << "/*Null Comparator*/";
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
			case ZValComparator_Simple::eNE:
				{
				s << " != ";
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
		s << "/*Unknown Comparator*/";
		}
	}

void spToStrim(const ZValPred& iValPred, const ZStrimW& s)
	{
	spToStrim(iValPred.GetLHS(), s);
	spToStrim(iValPred.GetComparator(), s);
	spToStrim(iValPred.GetRHS(), s);
	}

void ToStrim_SQL::Visit_Expr_Bool_ValPred(const ZRef<ZExpr_Bool_ValPred>& iRep)
	{ spToStrim(iRep->GetValPred(), pStrimW()); }

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZRA::SQL::sAsSQL

bool sWriteAsSQL(const map<string8,RelHead>& iTables, ZRef<Expr_Rel> iRel, const ZStrimW& s)
	{
	try
		{
		MakeThing theMakeThing(iTables);
		const Thing theThing = theMakeThing.Do(iRel);
		
		s << "SELECT DISTINCT ";

		{
		RelHead theRHLogical;
		for (RelHead::iterator i = theThing.fRelHead_Physical.begin();
			i != theThing.fRelHead_Physical.end(); ++i)
			{
			theRHLogical |= ZUtil_STL::sGetMustContain(1, theThing.fRename_Inverse, *i);
			}

		bool isFirst = true;
		for (RelHead::iterator i = theRHLogical.begin(); i != theRHLogical.end(); ++i)
			{
			if (!isFirst)
				s << ",";
			isFirst = false;
			s << ZUtil_STL::sGetMustContain(1, theThing.fRename, *i);
			}
		}

		s << " FROM ";

		{
		bool isFirst = true;

		for (map<string8,int>::iterator i = theMakeThing.fTablesUsed.begin();
			i != theMakeThing.fTablesUsed.end(); ++i)
			{
			for (int x = 0; x < i->second; ++x)
				{
				if (!isFirst)
					s << ",";
				isFirst = false;
				s << i->first << " AS " << i->first << ZStringf("%d", x);
				}
			}
		}

		s << " WHERE ";

		ToStrim_SQL().DoToStrim(ToStrim_SQL::Options(), s, theThing.fCondition);
		
		s << ";\n";
		return true;
		}
	catch (...)
		{}

	return false;
	}

} // namespace ZRA
} // namespace ZooLib
