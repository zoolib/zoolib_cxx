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
#include "zoolib/ZUtil_Expr_Bool_ValPred_Rename.h"
#include "zoolib/ZUtil_Strim.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZVisitor_Do_T.h"
#include "zoolib/ZVisitor_Expr_Bool_ValPred_Any_ToStrim.h"
#include "zoolib/ZVisitor_Expr_Op_Do_Transform_T.h"
#include "zoolib/ZValPred_Any.h"

#include "zoolib/zra/ZRA_AsSQL.h"
#include "zoolib/zra/ZRA_Expr_Rel_Concrete.h"
#include "zoolib/zra/ZRA_Expr_Rel_Product.h"
#include "zoolib/zra/ZRA_Expr_Rel_Project.h"
#include "zoolib/zra/ZRA_Expr_Rel_Rename.h"
#include "zoolib/zra/ZRA_Expr_Rel_Restrict.h"
#include "zoolib/zra/ZRA_Util_RelOperators.h"
#include "zoolib/zra/ZRA_Util_Strim_RelHead.h"

namespace ZooLib {
namespace ZRA {

using std::map;

// =================================================================================================
#pragma mark -
#pragma mark * Analysis (anonymous)

namespace { // anonymous

struct Analysis
	{
	ZMap_Any fConstValues;
	RelHead fRelHead_Physical;
	Rename fRename;
	Rename fRename_Inverse;
	ZRef<ZExpr_Bool> fCondition;
	};

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Analyzer (anonymous)

namespace { // anonymous

class Analyzer
:	public virtual ZVisitor_Do_T<Analysis>
,	public virtual Visitor_Expr_Rel_Concrete
,	public virtual Visitor_Expr_Rel_Const
,	public virtual Visitor_Expr_Rel_Dee
,	public virtual Visitor_Expr_Rel_Product
,	public virtual Visitor_Expr_Rel_Project
,	public virtual Visitor_Expr_Rel_Rename
,	public virtual Visitor_Expr_Rel_Restrict
	{
public:
	Analyzer(const map<string8,RelHead>& iTables);

	virtual void Visit(const ZRef<ZVisitee>& iRep);

	virtual void Visit_Expr_Rel_Concrete(const ZRef<Expr_Rel_Concrete>& iExpr);
	virtual void Visit_Expr_Rel_Const(const ZRef<Expr_Rel_Const>& iExpr);
	virtual void Visit_Expr_Rel_Dee(const ZRef<Expr_Rel_Dee>& iExpr);
	virtual void Visit_Expr_Rel_Product(const ZRef<Expr_Rel_Product>& iExpr);
	virtual void Visit_Expr_Rel_Project(const ZRef<Expr_Rel_Project>& iExpr);
	virtual void Visit_Expr_Rel_Rename(const ZRef<Expr_Rel_Rename>& iExpr);
	virtual void Visit_Expr_Rel_Restrict(const ZRef<Expr_Rel_Restrict>& iExpr);

	const map<string8,RelHead>& fTables;
	map<string8,int> fTablesUsed;
	};

Analyzer::Analyzer(const map<string8,RelHead>& iTables)
:	fTables(iTables)
	{}

void Analyzer::Visit(const ZRef<ZVisitee>& iRep)
	{ ZUnimplemented(); }

void Analyzer::Visit_Expr_Rel_Concrete(const ZRef<Expr_Rel_Concrete>& iExpr)
	{
	// Identify the table.
	const RelHead& theRH = iExpr->GetConcreteRelHead();

	ZQ<map<string8,RelHead>::const_iterator> found;
	for (map<string8,RelHead>::const_iterator iter = fTables.begin(); iter != fTables.end(); ++iter)
		{
		if ((sPrefixInserted(iter->first + "_", iter->second) & theRH).size() == theRH.size())
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
	const string8 usedTableName = realTableName + ZStringf("%d", numericSuffix);
	const string8 usedTableNameDot = usedTableName + ".";

	Analysis theAnalysis;
	theAnalysis.fCondition = sTrue();
	for (RelHead::const_iterator iter = theRH.begin(); iter != theRH.end(); ++iter)
		{
		const string8 attrName = *iter;
		const string8 fieldName = sPrefixErased(realTableNameUnderscore, attrName);
		const string8 physicalFieldName = usedTableNameDot + fieldName;
		theAnalysis.fRelHead_Physical |= physicalFieldName;
		ZUtil_STL::sInsertMustNotContain(1, theAnalysis.fRename, attrName, physicalFieldName);
		ZUtil_STL::sInsertMustNotContain(1, theAnalysis.fRename_Inverse, physicalFieldName, attrName);
		}

	this->pSetResult(theAnalysis);
	}

void Analyzer::Visit_Expr_Rel_Const(const ZRef<Expr_Rel_Const>& iExpr)
	{
	Analysis theAnalysis;
	theAnalysis.fCondition = sTrue();
	theAnalysis.fConstValues.Set(iExpr->GetRelName(), iExpr->GetVal());
	this->pSetResult(theAnalysis);
	}

void Analyzer::Visit_Expr_Rel_Dee(const ZRef<Expr_Rel_Dee>& iExpr)
	{
	Analysis theAnalysis;
	theAnalysis.fCondition = sTrue();
	this->pSetResult(theAnalysis);
	}

void Analyzer::Visit_Expr_Rel_Product(const ZRef<Expr_Rel_Product>& iExpr)
	{
	Analysis analysis0 = this->Do(iExpr->GetOp0());
	const Analysis analysis1 = this->Do(iExpr->GetOp1());

	for (ZMap_Any::Index_t i = analysis1.fConstValues.Begin();
		i != analysis1.fConstValues.End(); ++i)
		{
		analysis0.fConstValues.Set(analysis1.fConstValues.NameOf(i), analysis1.fConstValues.Get(i));
		}
	analysis0.fRelHead_Physical |= analysis1.fRelHead_Physical;
	analysis0.fRename.insert(analysis1.fRename.begin(), analysis1.fRename.end());
	analysis0.fRename_Inverse.insert
		(analysis1.fRename_Inverse.begin(), analysis1.fRename_Inverse.end());
	analysis0.fCondition &= analysis1.fCondition;

	this->pSetResult(analysis0);
	}

void Analyzer::Visit_Expr_Rel_Project(const ZRef<Expr_Rel_Project>& iExpr)
	{
	Analysis theAnalysis = this->Do(iExpr->GetOp0());
	const RelHead& theRH = iExpr->GetProjectRelHead();
	RelHead newRelHead;
	for (RelHead::iterator i = theAnalysis.fRelHead_Physical.begin();
		i != theAnalysis.fRelHead_Physical.end(); ++i)
		{
		const string8 theString1 = *i;
		const string8 theString2 =
			ZUtil_STL::sGetMustContain(1, theAnalysis.fRename_Inverse, theString1);
		if (ZUtil_STL::sContains(theRH, theString2))
			newRelHead.insert(theString1);
		}
	theAnalysis.fRelHead_Physical.swap(newRelHead);
	this->pSetResult(theAnalysis);
	}

void Analyzer::Visit_Expr_Rel_Restrict(const ZRef<Expr_Rel_Restrict>& iExpr)
	{
	Analysis theAnalysis = this->Do(iExpr->GetOp0());
	theAnalysis.fCondition &= Util_Expr_Bool::sRenamed(theAnalysis.fRename, iExpr->GetExpr_Bool());
	this->pSetResult(theAnalysis);
	}

void Analyzer::Visit_Expr_Rel_Rename(const ZRef<Expr_Rel_Rename>& iExpr)
	{
	Analysis theAnalysis = this->Do(iExpr->GetOp0());
	const RelName& oldName = iExpr->GetOld();
	const RelName& newName = iExpr->GetNew();
	if (ZQ<RelName> theQ = ZUtil_STL::sEraseAndReturnIfContains(theAnalysis.fRename, oldName))
		{
		const RelName orgName = theQ.Get();
		const RelName orgNameInverse =
			ZUtil_STL::sEraseAndReturnIfContains(theAnalysis.fRename_Inverse, orgName).Get();
		ZAssert(orgNameInverse == oldName);
		ZUtil_STL::sInsertMustNotContain(1, theAnalysis.fRename, newName, orgName);
		ZUtil_STL::sInsertMustNotContain(1, theAnalysis.fRename_Inverse, orgName, newName);
		}
	else
		{
		ZMap_Any::Index_t theIndex = theAnalysis.fConstValues.IndexOf(oldName);
		ZAssert(theIndex != theAnalysis.fConstValues.End());
		const ZVal_Any theVal = theAnalysis.fConstValues.Get(theIndex);
		theAnalysis.fConstValues.Erase(theIndex);
		theAnalysis.fConstValues.Set(newName, theVal);
		}

	this->pSetResult(theAnalysis);
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ToStrim_SQL (anonymous)

namespace { // anonymous

class ToStrim_SQL
:	public virtual ZVisitor_ToStrim
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
	this->pToStrim(iRep->GetOp0());
	pStrimW() << ")";
	}

void ToStrim_SQL::Visit_Expr_Bool_And(const ZRef<ZExpr_Bool_And>& iRep)
	{
	ZRef<ZExpr_Bool> theFalse = sFalse();
	ZRef<ZExpr_Bool> theTrue = sTrue();

	ZRef<ZExpr_Bool> theOp0 = iRep->GetOp0();
	ZRef<ZExpr_Bool> theOp1 = iRep->GetOp1();
	if (theOp0 == theFalse || theOp1 == theFalse)
		{
		this->pToStrim(theFalse);
		}
	else if (theOp0 == theTrue)
		{
		this->pToStrim(theOp1);
		}
	else if (theOp1 == theTrue)
		{
		this->pToStrim(theOp0);
		}
	else
		{
		pStrimW() << "(";
		this->pToStrim(iRep->GetOp0());
		pStrimW() << " AND ";
		this->pToStrim(iRep->GetOp1());
		pStrimW() << ")";
		}
	}

void ToStrim_SQL::Visit_Expr_Bool_Or(const ZRef<ZExpr_Bool_Or>& iRep)
	{
	ZRef<ZExpr_Bool> theFalse = sFalse();
	ZRef<ZExpr_Bool> theTrue = sTrue();

	ZRef<ZExpr_Bool> theOp0 = iRep->GetOp0();
	ZRef<ZExpr_Bool> theOp1 = iRep->GetOp1();
	if (theOp0 == theTrue || theOp1 == theTrue)
		{
		this->pToStrim(theTrue);
		}
	else if (theOp0 == theFalse)
		{
		this->pToStrim(theOp1);
		}
	else if (theOp1 == theFalse)
		{
		this->pToStrim(theOp0);
		}
	else
		{
		pStrimW() << "(";
		this->pToStrim(iRep->GetOp0());
		pStrimW() << " OR ";
		this->pToStrim(iRep->GetOp1());
		pStrimW() << ")";
		}
	}

static void spToStrim_SimpleValue(const ZStrimW& s, const ZAny& iAny)
	{
	if (false)
		{}
	else if (not iAny)
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
	else if (ZQ<int64> theQ = sQCoerceInt(iAny))
		{
		s.Writef("%lld", theQ.Get());
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
	{ s << iName; }

static void spToStrim(const ZRef<ZValComparand>& iComparand, const ZStrimW& s)
	{
	if (not iComparand)
		{
		s << "/*Null Comparand*/";
		}
	else if (ZRef<ZValComparand_Name> asName = iComparand.DynamicCast<ZValComparand_Name>())
		{
		spWrite_PropName(asName->GetName(), s);
		}
	else if (ZRef<ZValComparand_Const_Any> asConst =
		iComparand.DynamicCast<ZValComparand_Const_Any>())
		{
		spToStrim_SimpleValue(s, asConst->GetVal());
		}
	else
		{
		s << "/*Unknown Comparand*/";
		}
	}

void spToStrim(const ZValPred& iValPred, const ZStrimW& s)
	{
	if (ZRef<ZValComparator_Simple> asSimple =
		iValPred.GetComparator().DynamicCast<ZValComparator_Simple>())
		{
		spToStrim(iValPred.GetLHS(), s);
		switch (asSimple->GetEComparator())
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
		spToStrim(iValPred.GetRHS(), s);
		}
	else if (ZRef<ZValComparator_StringContains> asStringContains =
		iValPred.GetComparator().DynamicCast<ZValComparator_StringContains>())
		{
		if (ZRef<ZValComparand_Name> asName = iValPred.GetLHS().DynamicCast<ZValComparand_Name>())
			{
			if (ZRef<ZValComparand_Const_Any> asConst =
				iValPred.GetRHS().DynamicCast<ZValComparand_Const_Any>())
				{
				if (const string8* asString = asConst->GetVal().PGet<string8>())
					{
					spWrite_PropName(asName->GetName(), s);
					s << " LIKE ";
					ZStrimW_Escaped::Options theOptions;
					theOptions.fQuoteQuotes = true;
					theOptions.fEscapeHighUnicode = false;
					s << "'%";
					ZStrimW_Escaped(theOptions, s).Write(*asString);
					s << "%'";
					return;
					}
				}
			}
		s << "/*Unhandled StringContains*/";
		}
	else
		{
		s << "/*Unhandled Comparator*/";
		}
	}

void ToStrim_SQL::Visit_Expr_Bool_ValPred(const ZRef<ZExpr_Bool_ValPred>& iRep)
	{ spToStrim(iRep->GetValPred(), pStrimW()); }

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZRA::sWriteAsSQL

bool sWriteAsSQL(const map<string8,RelHead>& iTables, ZRef<Expr_Rel> iRel, const ZStrimW& s)
	{
	try
		{
		Analyzer theAnalyzer(iTables);
		const Analysis theAnalysis = theAnalyzer.Do(iRel);

		s << "SELECT DISTINCT ";

		{
		RelHead theRHLogical;
		for (RelHead::iterator i = theAnalysis.fRelHead_Physical.begin();
			i != theAnalysis.fRelHead_Physical.end(); ++i)
			{
			theRHLogical |= ZUtil_STL::sGetMustContain(1, theAnalysis.fRename_Inverse, *i);
			}

		for (ZMap_Any::Index_t i = theAnalysis.fConstValues.Begin();
			i != theAnalysis.fConstValues.End(); ++i)
			{
			theRHLogical |= theAnalysis.fConstValues.NameOf(i);
			}

		bool isFirst = true;
		for (RelHead::iterator i = theRHLogical.begin(); i != theRHLogical.end(); ++i)
			{
			if (not isFirst)
				s << ",";
			isFirst = false;
			if (ZQ<string8> theQ = ZUtil_STL::sGetIfContains(theAnalysis.fRename, *i))
				s << theQ.Get();
			else
				spToStrim_SimpleValue(s, theAnalysis.fConstValues.Get(*i));
			}
		}

		s << " FROM ";

		{
		bool isFirst = true;

		for (map<string8,int>::iterator i = theAnalyzer.fTablesUsed.begin();
			i != theAnalyzer.fTablesUsed.end(); ++i)
			{
			for (int x = 0; x < i->second; ++x)
				{
				if (not isFirst)
					s << ",";
				isFirst = false;
				s << i->first << " AS " << i->first << ZStringf("%d", x);
				}
			}
		}

		s << " WHERE ";

		ToStrim_SQL().ToStrim(ToStrim_SQL::Options(), s, theAnalysis.fCondition);

		s << ";";
		return true;
		}
	catch (...)
		{}

	return false;
	}

} // namespace ZRA
} // namespace ZooLib
