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

#include "zoolib/ZUtil_Strim_RelHead.h"

#include "zoolib/zql/ZQL_Visitor_ExprRep_Relation_DoToStrim.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

using std::set;
using std::string;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Static helpers

namespace ZANONYMOUS {

void spWrite(const string& iString, const ZStrimW& s)
	{ s.Write(iString); }

void spWrite(const UTF8* iString, const ZStrimW& s)
	{ s.Write(iString); }

void spWrite_RelHead(const ZRelHead& iRelHead, const ZStrimW& iStrimW)
	{ ZUtil_Strim_RelHead::sWrite_RelHead(iRelHead, iStrimW); }

void spWrite_PropName(const string& iPropName, const ZStrimW& iStrimW)
	{ ZUtil_Strim_RelHead::sWrite_PropName(iPropName, iStrimW); }

void spWrite_EffectiveRelHeadComment(ZRef<ExprRep_Relation> iRep, const ZStrimW& iStrimW)
	{
	iStrimW.Write(" // ");
	ZUtil_Strim_RelHead::sWrite_RelHead(iRep->GetRelHead(), iStrimW);
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_Relation_DoToStrim

Visitor_ExprRep_Relation_DoToStrim::Visitor_ExprRep_Relation_DoToStrim(
	const Options& iOptions, const ZStrimW& iStrimW)
:	ZVisitor_ExprRep_DoToStrim(iOptions, iStrimW)
	{}

bool Visitor_ExprRep_Relation_DoToStrim::Visit_ExprRep_Relation_Difference(
	ZRef<ExprRep_Relation_Difference> iRep)
	{ return this->pWriteDyadic("Difference", iRep); }

bool Visitor_ExprRep_Relation_DoToStrim::Visit_ExprRep_Relation_Intersect(
	ZRef<ExprRep_Relation_Intersect> iRep)
	{ return this->pWriteDyadic("Intersect", iRep); }

bool Visitor_ExprRep_Relation_DoToStrim::Visit_ExprRep_Relation_Join(
	ZRef<ExprRep_Relation_Join> iRep)
	{
	ZRef<ExprRep_Relation> theLHS = iRep->GetLHS();
	ZRef<ExprRep_Relation> theRHS = iRep->GetRHS();

	spWrite("Join", fStrimW);

	if (fOptions.fDebuggingOutput)
		spWrite_EffectiveRelHeadComment(iRep, fStrimW);

	this->pWriteLFIndent();
	spWrite("(", fStrimW);
	if (fOptions.fDebuggingOutput)
		{
		spWrite(" // Joining on: ", fStrimW);
		const ZRelHead joinOn = theLHS->GetRelHead()
			& theRHS->GetRelHead();
		spWrite_RelHead(joinOn, fStrimW);
		}

	this->pWriteLFIndent();
	this->DoToStrim(theLHS);
	spWrite(", ", fStrimW);

	this->pWriteLFIndent();
	this->DoToStrim(theRHS);
	this->pWriteLFIndent();

	spWrite(")", fStrimW);
	return true;
	}

bool Visitor_ExprRep_Relation_DoToStrim::Visit_ExprRep_Relation_Project(
	ZRef<ExprRep_Relation_Project> iRep)
	{
	spWrite("Project", fStrimW);

	if (fOptions.fDebuggingOutput)
		spWrite_EffectiveRelHeadComment(iRep, fStrimW);

	this->pWriteLFIndent();
	spWrite("(", fStrimW);

	this->pWriteLFIndent();
	spWrite_RelHead(iRep->GetProjectRelHead(), fStrimW);
	spWrite(",", fStrimW);

	this->pWriteLFIndent();
	this->DoToStrim(iRep->GetExprRep());
	this->pWriteLFIndent();

	spWrite(")", fStrimW);
	return true;
	}

bool Visitor_ExprRep_Relation_DoToStrim::Visit_ExprRep_Relation_Rename(
	ZRef<ExprRep_Relation_Rename> iRep)
	{
	spWrite("Rename", fStrimW);

	if (fOptions.fDebuggingOutput)
		spWrite_EffectiveRelHeadComment(iRep, fStrimW);

	this->pWriteLFIndent();
	spWrite("(", fStrimW);

	this->pWriteLFIndent();
	spWrite_PropName(iRep->GetOld(), fStrimW);
	spWrite(", ", fStrimW);
	spWrite_PropName(iRep->GetNew(), fStrimW);
	spWrite(",", fStrimW);

	this->pWriteLFIndent();
	this->DoToStrim(iRep->GetExprRep());
	this->pWriteLFIndent();

	spWrite(")", fStrimW);
	return true;
	}

bool Visitor_ExprRep_Relation_DoToStrim::Visit_ExprRep_Relation_Union(
	ZRef<ExprRep_Relation_Union> iRep)
	{ return this->pWriteDyadic("Union", iRep); }

bool Visitor_ExprRep_Relation_DoToStrim::pWriteDyadic(
	const string& iFunctionName, ZRef<ExprRep_Relation_Dyadic> iRep)
	{
	spWrite(iFunctionName, fStrimW);

	if (fOptions.fDebuggingOutput)
		spWrite_EffectiveRelHeadComment(iRep, fStrimW);

	this->pWriteLFIndent();
	spWrite("(", fStrimW);

	this->pWriteLFIndent();
	this->DoToStrim(iRep->GetLHS());
	spWrite(", ", fStrimW);

	this->pWriteLFIndent();
	this->DoToStrim(iRep->GetRHS());
	this->pWriteLFIndent();

	spWrite(")", fStrimW);
	return true;
	}

} // namespace ZQL
NAMESPACE_ZOOLIB_END
