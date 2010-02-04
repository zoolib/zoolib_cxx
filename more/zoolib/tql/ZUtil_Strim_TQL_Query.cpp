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

#include "zoolib/tql/ZUtil_Strim_TQL.h"
#include "zoolib/tql/ZUtil_Strim_TQL_Query.h"
#include "zoolib/tql/ZUtil_Strim_TQL_Spec.h"

NAMESPACE_ZOOLIB_BEGIN

namespace ZUtil_Strim_TQL {

using std::set;
using std::string;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Options

Options::Options()
:	fEOLString("\n"),
	fIndentString("  ")
	{}

namespace ZANONYMOUS {

// =================================================================================================
#pragma mark -
#pragma mark * Static helpers

static void spWrite(const string& iString, const ZStrimW& s)
	{ s.Write(iString); }

static void spWrite(const UTF8* iString, const ZStrimW& s)
	{ s.Write(iString); }

static void spWrite_Indent(size_t iCount, const Options& iOptions, const ZStrimW& iStrimW)
	{
	while (iCount--)
		spWrite(iOptions.fIndentString, iStrimW);
	}

static void spWrite_LFIndent(size_t iCount, const Options& iOptions, const ZStrimW& iStrimW)
	{
	spWrite(iOptions.fEOLString, iStrimW);
	spWrite_Indent(iCount, iOptions, iStrimW);
	}

// =================================================================================================
#pragma mark -
#pragma mark * Writer

class Writer : public ZVisitor_ExprRep_Query
	{
public:
	Writer(size_t iIndent, const Options& iOptions, const ZStrimW& iStrimW);

// From NodeVisitor
	virtual bool Visit_Difference(ZRef<ZExprRep_Relational_Difference> iRep);
	virtual bool Visit_Intersect(ZRef<ZExprRep_Relational_Intersect> iRep);
	virtual bool Visit_Join(ZRef<ZExprRep_Relational_Join> iRep);
	virtual bool Visit_Project(ZRef<ZExprRep_Relational_Project> iRep);
	virtual bool Visit_Rename(ZRef<ZExprRep_Relational_Rename> iRep);
	virtual bool Visit_Union(ZRef<ZExprRep_Relational_Union> iRep);

// From ZVisitor_ExprRep_Query
	virtual bool Visit_All(ZRef<ZExprRep_Query_All> iRep);
	virtual bool Visit_Explicit(ZRef<ZExprRep_Query_Explicit> iRep);
	virtual bool Visit_Restrict(ZRef<ZExprRep_Query_Restrict> iRep);
	virtual bool Visit_Select(ZRef<ZExprRep_Query_Select> iRep);

private:
	bool pWriteDyadic(const string& iFunctionName, ZRef<ZExprRep_Relational_Dyadic> iRep);

	size_t fIndent;
	const Options& fOptions;
	const ZStrimW& fStrimW;
	};

static void spWrite_EffectiveRelHeadComment(ZRef<ZExprRep_Relational> iRep, const ZStrimW& s)
	{
	s.Write(" // ");
	ZUtil_Strim_TQL::sWrite_RelHead(iRep->GetRelHead(), s);
	}

static void spWrite(size_t iIndent, const Options& iOptions,
	ZRef<ZExprRep_Relational> iRep,
	const ZStrimW& s)
	{
	spWrite_LFIndent(iIndent, iOptions, s);
	Writer theWriter(iIndent, iOptions, s);
	iRep->Accept(theWriter);
	}

Writer::Writer(size_t iIndent, const Options& iOptions, const ZStrimW& iStrimW)
:	fIndent(iIndent),
	fOptions(iOptions),
	fStrimW(iStrimW)
	{}

bool Writer::Visit_Difference(ZRef<ZExprRep_Relational_Difference> iRep)
	{ return this->pWriteDyadic("Difference", iRep); }

bool Writer::Visit_Intersect(ZRef<ZExprRep_Relational_Intersect> iRep)
	{ return this->pWriteDyadic("Intersect", iRep); }

bool Writer::Visit_Join(ZRef<ZExprRep_Relational_Join> iRep)
	{
	ZRef<ZExprRep_Relational> theLHS = iRep->GetLHS();
	ZRef<ZExprRep_Relational> theRHS = iRep->GetRHS();

	const ZRelHead joinOn = theLHS->GetRelHead()
		& theRHS->GetRelHead();

	spWrite("Join", fStrimW);
	spWrite_EffectiveRelHeadComment(iRep, fStrimW);
	spWrite_LFIndent(fIndent + 1, fOptions, fStrimW);
	spWrite("( // Join on: ", fStrimW);
	sWrite_RelHead(joinOn, fStrimW);
	spWrite(fIndent + 1, fOptions, theLHS, fStrimW);
	spWrite(", ", fStrimW);
	spWrite(fIndent + 1, fOptions, theRHS, fStrimW);
	spWrite_LFIndent(fIndent + 1, fOptions, fStrimW);
	spWrite(")", fStrimW);
	return true;
	}

bool Writer::Visit_Project(ZRef<ZExprRep_Relational_Project> iRep)
	{
	spWrite("Project", fStrimW);
	spWrite_EffectiveRelHeadComment(iRep, fStrimW);
	spWrite_LFIndent(fIndent + 1, fOptions, fStrimW);
	spWrite("(", fStrimW);
	spWrite_LFIndent(fIndent + 1, fOptions, fStrimW);
	sWrite_RelHead(iRep->GetProjectRelHead(), fStrimW);
	spWrite(",", fStrimW);
	spWrite(fIndent + 1, fOptions, iRep->GetExpr(), fStrimW);
	spWrite_LFIndent(fIndent + 1, fOptions, fStrimW);
	spWrite(")", fStrimW);
	return true;
	}

bool Writer::Visit_Rename(ZRef<ZExprRep_Relational_Rename> iRep)
	{
	spWrite("Rename", fStrimW);
	spWrite_EffectiveRelHeadComment(iRep, fStrimW);
	spWrite_LFIndent(fIndent + 1, fOptions, fStrimW);
	spWrite("(", fStrimW);
	spWrite_LFIndent(fIndent + 1, fOptions, fStrimW);
	sWrite_PropName(iRep->GetOld(), fStrimW);
	spWrite(", ", fStrimW);
	sWrite_PropName(iRep->GetNew(), fStrimW);
	spWrite(",", fStrimW);
	spWrite(fIndent + 1, fOptions, iRep->GetExpr(), fStrimW);
	spWrite_LFIndent(fIndent + 1, fOptions, fStrimW);
	spWrite(")", fStrimW);
	return true;
	}

bool Writer::Visit_Union(ZRef<ZExprRep_Relational_Union> iRep)
	{ return this->pWriteDyadic("Union", iRep); }

bool Writer::Visit_All(ZRef<ZExprRep_Query_All> iRep)
	{
	const string theIDPropName = iRep->GetIDPropName();
	if (theIDPropName.empty())
		{
		spWrite("All(", fStrimW);
		}
	else
		{
		spWrite("AID(", fStrimW);
		sWrite_PropName(theIDPropName, fStrimW);
		spWrite(", ", fStrimW);
		}
	sWrite_RelHead(iRep->GetAllRelHead(), fStrimW);
	spWrite(")", fStrimW);
	return true;
	}

bool Writer::Visit_Explicit(ZRef<ZExprRep_Query_Explicit> iRep)
	{
	spWrite("Explicit(", fStrimW);
	bool isFirst = true;
	const vector<ZVal_Expr> theVals = iRep->GetVals();
	for (vector<ZVal_Expr>::const_iterator i = theVals.begin(); i != theVals.end(); ++i)
		{
		if (!isFirst)
			spWrite(", ", fStrimW);
		isFirst = false;

		ZUtil_Strim_TQL::sWrite(*i, fStrimW);
		}
	spWrite(")", fStrimW);
	return true;
	}

bool Writer::Visit_Restrict(ZRef<ZExprRep_Query_Restrict> iRep)
	{
	spWrite("Restrict", fStrimW);
	spWrite_EffectiveRelHeadComment(iRep, fStrimW);
	spWrite_LFIndent(fIndent + 1, fOptions, fStrimW);
	spWrite("(", fStrimW);
	spWrite_LFIndent(fIndent + 1, fOptions, fStrimW);
	sToStrim(iRep->GetValCondition(), fStrimW);
	spWrite(",", fStrimW);
	spWrite(fIndent + 1, fOptions, iRep->GetExpr(), fStrimW);
	spWrite_LFIndent(fIndent + 1, fOptions, fStrimW);
	spWrite(")", fStrimW);
	return true;
	}

bool Writer::Visit_Select(ZRef<ZExprRep_Query_Select> iRep)
	{
	spWrite("Select", fStrimW);
	spWrite_EffectiveRelHeadComment(iRep, fStrimW);
	spWrite_LFIndent(fIndent + 1, fOptions, fStrimW);
	spWrite("(", fStrimW);
	spWrite_LFIndent(fIndent + 1, fOptions, fStrimW);
	sToStrim(iRep->GetExpr_Logical(), fStrimW);
	spWrite(",", fStrimW);
	spWrite(fIndent + 1, fOptions, iRep->GetExpr_Relational(), fStrimW);
	spWrite_LFIndent(fIndent + 1, fOptions, fStrimW);
	spWrite(")", fStrimW);
	return true;
	}

bool Writer::pWriteDyadic(const string& iFunctionName, ZRef<ZExprRep_Relational_Dyadic> iRep)
	{
	spWrite(iFunctionName, fStrimW);

	spWrite_EffectiveRelHeadComment(iRep, fStrimW);

	spWrite_LFIndent(fIndent + 1, fOptions, fStrimW);
	spWrite("(", fStrimW);

	spWrite(fIndent + 1, fOptions, iRep->GetLHS(), fStrimW);
	spWrite(", ", fStrimW);

	spWrite(fIndent + 1, fOptions, iRep->GetRHS(), fStrimW);
	spWrite_LFIndent(fIndent + 1, fOptions, fStrimW);

	spWrite(")", fStrimW);
	return true;
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Strim_TQL

void sToStrim(const ZRef<ZExprRep_Relational>& iRep, const ZStrimW& s)
	{ sToStrim(0, Options(), iRep, s); }

void sToStrim(size_t iInitialIndent, const Options& iOptions,
	const ZRef<ZExprRep_Relational>& iRep,
	const ZStrimW& s)
	{ spWrite(iInitialIndent, iOptions, iRep, s); }

} // namespace ZUtil_Strim_TQL

NAMESPACE_ZOOLIB_END
