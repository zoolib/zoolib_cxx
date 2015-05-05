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

#include "zoolib/Util_Chan_UTF.h"
#include "zoolib/Util_Chan_UTF_Operators.h"
#include "zoolib/Util_string.h"
#include "zoolib/Yad_Any.h"
#include "zoolib/Yad_JSON.h"

#include "zoolib/RelationalAlgebra/GetRelHead.h"
#include "zoolib/RelationalAlgebra/Util_Strim_Rel.h"
#include "zoolib/RelationalAlgebra/Util_Strim_RelHead.h"

#include "zoolib/ValPred/Util_Strim_Expr_Bool_ValPred.h"

#include "zoolib/Log.h"

namespace ZooLib {
namespace RelationalAlgebra {
namespace Util_Strim_Rel {

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark RelationalAlgebra::Util_Strim_Rel::Visitor

void Visitor::Visit_Expr(const ZRef<Expr>& iExpr)
	{
	const ChanW_UTF& w = pStrimW();
	w << iExpr->DebugDescription();
	}

void Visitor::Visit_Expr_Rel_Calc(const ZRef<Expr_Rel_Calc>& iExpr)
	{
	const ChanW_UTF& w = pStrimW();
	w << "Calc(";
	Util_Strim_RelHead::sWrite_PropName(iExpr->GetColName(), w);
	w << " = /*Some function of*/";
	w << ",";
	this->pWriteLFIndent();
	this->pToStrim(iExpr->GetOp0());
	w << ")";
	}

void Visitor::Visit_Expr_Rel_Concrete(const ZRef<Expr_Rel_Concrete>& iExpr)
	{
	const ChanW_UTF& w = pStrimW();
	w << "Concrete(" << iExpr->GetConcreteHead() << ")";
	}

void Visitor::Visit_Expr_Rel_Const(const ZRef<Expr_Rel_Const>& iExpr)
	{
	const ChanW_UTF& w = pStrimW();
	w << "Const(";
	Util_Strim_RelHead::sWrite_PropName(iExpr->GetColName(), w);
	w << ",";
	Yad_JSON::sToChan(sYadR(iExpr->GetVal()), w);
	w << ")";
	}

void Visitor::Visit_Expr_Rel_Dee(const ZRef<Expr_Rel_Dee>& iExpr)
	{ pStrimW() << "Dee()"; }

void Visitor::Visit_Expr_Rel_Difference(const ZRef<Expr_Rel_Difference>& iExpr)
	{ this->pWriteBinary("Difference", iExpr); }

void Visitor::Visit_Expr_Rel_Dum(const ZRef<Expr_Rel_Dum>& iExpr)
	{ pStrimW() << "Dum()"; }

void Visitor::Visit_Expr_Rel_Embed(const ZRef<Expr_Rel_Embed>& iExpr)
	{
	const ChanW_UTF& w = pStrimW();
	w << "Embed(";
	this->pWriteLFIndent();
	this->pToStrim(iExpr->GetOp0());
	w << ",";
	this->pWriteLFIndent();
	Util_Strim_RelHead::sWrite_PropName(iExpr->GetColName(), w);
	w << " = ";
	++fIndent;
	this->pToStrim(iExpr->GetOp1());
	--fIndent;
	w << ")";
	}

void Visitor::Visit_Expr_Rel_Intersect(const ZRef<Expr_Rel_Intersect>& iExpr)
	{ this->pWriteBinary("Intersect", iExpr); }

void Visitor::Visit_Expr_Rel_Product(const ZRef<Expr_Rel_Product>& iExpr)
	{ this->pWriteBinary("Product", iExpr); }

void Visitor::Visit_Expr_Rel_Project(const ZRef<Expr_Rel_Project>& iExpr)
	{
	const ChanW_UTF& w = pStrimW();
	w << "Project(" << iExpr->GetProjectRelHead() << ",";
	this->pWriteLFIndent();
	this->pToStrim(iExpr->GetOp0());
	w << ")";
	}

void Visitor::Visit_Expr_Rel_Rename(const ZRef<Expr_Rel_Rename>& iExpr)
	{
	const ChanW_UTF& w = pStrimW();
	w << "Rename(";
	Util_Strim_RelHead::sWrite_PropName(iExpr->GetNew(), w);
	w << "<--";
	Util_Strim_RelHead::sWrite_PropName(iExpr->GetOld(), w);
	w << ",";
	this->pWriteLFIndent();
	this->pToStrim(iExpr->GetOp0());
	w << ")";
	}

void Visitor::Visit_Expr_Rel_Restrict(const ZRef<Expr_Rel_Restrict>& iExpr)
	{
	const ChanW_UTF& w = pStrimW();
	w << "Restrict(";
	this->pToStrim(iExpr->GetExpr_Bool());
	w << ",";
	this->pWriteLFIndent();
	this->pToStrim(iExpr->GetOp0());
	w << ")";
	}

void Visitor::Visit_Expr_Rel_Union(const ZRef<Expr_Rel_Union>& iExpr)
	{ this->pWriteBinary("Union", iExpr); }

void Visitor::Visit_Expr_Rel_Search(const ZRef<QueryEngine::Expr_Rel_Search>& iExpr)
	{
	const ChanW_UTF& w = pStrimW();
	w << "Search(";
	this->pToStrim(iExpr->GetExpr_Bool());
	w << ",";
	this->pWriteLFIndent();
	w << iExpr->GetRename();
	w << ")";
	}

void Visitor::pWriteBinary(
	const string& iFunctionName, const ZRef<Expr_Op2_T<Expr_Rel> >& iExpr)
	{
	const ChanW_UTF& w = pStrimW();
	w << iFunctionName << "(";

	this->pWriteLFIndent();
	this->pToStrim(iExpr->GetOp0());
	w << ",";
	this->pWriteLFIndent();
	this->pToStrim(iExpr->GetOp1());
	w << ")";
	}

// =================================================================================================
#pragma mark -
#pragma mark RelationalAlgebra::Util_Strim_Rel::sToStrim

void sToStrim(const ZRef<RelationalAlgebra::Expr_Rel>& iRel, const ChanW_UTF& iStrimW)
	{
	Options theOptions;
	theOptions.fEOLString = "\n";
	theOptions.fIndentString = "|\t";
	sToStrim(iRel, theOptions, iStrimW);
	}

void sToStrim_Parseable(const ZRef<RelationalAlgebra::Expr_Rel>& iRel, const ChanW_UTF& iStrimW)
	{
	Options theOptions;
//##	theOptions.fEOLString = "\n";
//##	theOptions.fIndentString = "\t";
	sToStrim(iRel, theOptions, iStrimW);
	}

void sToStrim(const ZRef<RelationalAlgebra::Expr_Rel>& iRel, const Options& iOptions, const ChanW_UTF& iStrimW)
	{ Visitor().ToStrim(iOptions, iStrimW, iRel); }

// =================================================================================================
#pragma mark -
#pragma mark RelationalAlgebra::Util_Strim_Rel::sFromStrim

ZRef<Expr_Rel> sQFromStrim(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU)
	{
	try { return sFromStrim(iChanR, iChanU); }
	catch (...) {}
	return null;
	}

using namespace Util_Chan;
using std::pair;

typedef pair<ZRef<Expr_Rel>,ZRef<Expr_Rel> > RelPair;

static void spRead_WSComma(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU,
	const string8& iMessage)
	{
	sSkip_WSAndCPlusPlusComments(iChanR, iChanU);
	if (not sTryRead_CP(',', iChanR, iChanU))
		throw ParseException("Expected ','" + iMessage);
	}

static ZQ<RelPair> spQReadPair(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU,
	const string& iMessage)
	{
	sSkip_WSAndCPlusPlusComments(iChanR, iChanU);
	if (ZRef<Expr_Rel> left = sQFromStrim(iChanR, iChanU))
		{
		spRead_WSComma(iChanR, iChanU, iMessage);
		if (ZRef<Expr_Rel> right = sQFromStrim(iChanR, iChanU))
			return RelPair(left, right);
		}
	return null;
	}

ZRef<Expr_Rel> sFromStrim(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU)
	{
	sSkip_WSAndCPlusPlusComments(iChanR, iChanU);
	if (ZQ<string8,false> theNameQ = Yad_JSON::sQRead_PropName(iChanR, iChanU))
		{ return null; }
	else
		{
		sSkip_WSAndCPlusPlusComments(iChanR, iChanU);
		if (not sTryRead_CP('(', iChanR, iChanU))
			throw ParseException("Expected '(' after " + *theNameQ);

		sSkip_WSAndCPlusPlusComments(iChanR, iChanU);

		using Util_string::sEquali;

		if (ZLOGF(w, eDebug + 1))
			w << "Read " << *theNameQ;

		ZRef<Expr_Rel> result;

		if (false)
			{}
		else if (sEquali(*theNameQ, "Calc"))
			{
			ZAssert(false);
			}
		else if (sEquali(*theNameQ, "Concrete"))
			{
			if (NotQ<ConcreteHead> theConcreteHeadQ = Util_Strim_RelHead::sQFromStrim_ConcreteHead(iChanR, iChanU))
				throw ParseException("Expected ConcreteHead param to Concrete");
			else
				result = sConcrete(*theConcreteHeadQ);
			}
		else if (sEquali(*theNameQ, "Const"))
			{
			ZAssert(false);
			}
		else if (sEquali(*theNameQ, "Dee"))
			{
			result = sDee();
			}
		else if (sEquali(*theNameQ, "Difference"))
			{
			ZAssert(false);
			}
		else if (sEquali(*theNameQ, "Dum"))
			{
			result = sDum();
			}
		else if (sEquali(*theNameQ, "Embed"))
			{
			if (ZRef<Expr_Rel,false> outerRel = sFromStrim(iChanR, iChanU))
				throw ParseException("Expected Rel as first param in Embed");
			else
				{
				spRead_WSComma(iChanR, iChanU, " after Rel in Embed");

				sSkip_WSAndCPlusPlusComments(iChanR, iChanU);

				if (NotQ<ColName> theColNameQ = Util_Strim_RelHead::sQRead_PropName(iChanR, iChanU))
					throw ParseException("Expected ColName as first param in Embed");
				else
					{
					sSkip_WSAndCPlusPlusComments(iChanR, iChanU);

					if (not sTryRead_CP('=', iChanR, iChanU))
						throw ParseException("Expected '='");

					sSkip_WSAndCPlusPlusComments(iChanR, iChanU);

					if (ZRef<Expr_Rel,false> innerRel = sFromStrim(iChanR, iChanU))
						throw ParseException("Expected Rel as second param in Embed");
					else
						result = sEmbed(outerRel, *theColNameQ, innerRel);
					}
				}
			}
		else if (sEquali(*theNameQ, "Intersect"))
			{
			ZAssert(false);
			}
		else if (sEquali(*theNameQ, "Product"))
			{
			if (NotQ<RelPair> theQ = spQReadPair(iChanR, iChanU, "in Product"))
				throw ParseException("Expected Rel params to Product");
			else
				result = sProduct(theQ->first, theQ->second);
			}
		else if (sEquali(*theNameQ, "Project"))
			{
			if (NotQ<RelHead> theRelHeadQ = Util_Strim_RelHead::sQFromStrim_RelHead(iChanR, iChanU))
				throw ParseException("Expected RelHead param to Project");
			else
				{
				spRead_WSComma(iChanR, iChanU, " after RelHead in Project");

				if (ZRef<Expr_Rel,false> childRel = sFromStrim(iChanR, iChanU))
					throw ParseException("Expected Rel as second param in Project");
				else
					result = sProject(childRel, *theRelHeadQ);
				}
			}
		else if (sEquali(*theNameQ, "Rename"))
			{
			if (NotQ<pair<ColName,ColName> > theRenameQ = Util_Strim_RelHead::sQFromStrim_Rename(iChanR, iChanU))
				{ throw ParseException("Expected Rename param to Rename"); }
			else
				{
				spRead_WSComma(iChanR, iChanU, " after Rename in Rename");

				if (ZRef<Expr_Rel,false> childRel = sFromStrim(iChanR, iChanU))
					throw ParseException("Expected Rel as second param in Rename");
				else
					result = sRename(childRel, theRenameQ->first, theRenameQ->second);
				}
			}
		else if (sEquali(*theNameQ, "Restrict"))
			{
			sSkip_WSAndCPlusPlusComments(iChanR, iChanU);
			if (ZRef<Expr_Bool,false> theExpr = Util_Strim_Expr_Bool_ValPred::sQFromStrim(iChanR, iChanU))
				{ throw ParseException("Expected Expr_Bool param to Restrict"); }
			else
				{
				spRead_WSComma(iChanR, iChanU, " after ValPred in Restrict");

				if (ZRef<Expr_Rel,false> childRel = sFromStrim(iChanR, iChanU))
					throw ParseException("Expected Rel as second param in Restrict");
				else
					result = sRestrict(childRel, theExpr);
				}
			}
		else if (sEquali(*theNameQ, "Union"))
			{
			ZAssert(false);
			}
		else if (sEquali(*theNameQ, "Search"))
			{
			ZAssert(false);
			}
		else
			{
			throw ParseException("Unexpected name: " + *theNameQ);
			}

		sSkip_WSAndCPlusPlusComments(iChanR, iChanU);
		if (not sTryRead_CP(')', iChanR, iChanU))
			throw ParseException("Expected ')' after " + *theNameQ);
		return result;
		}
	return null;
	}

} // namespace Util_Strim_Rel
} // namespace RelationalAlgebra

const ChanW_UTF& operator<<(const ChanW_UTF& w, const ZRef<RelationalAlgebra::Expr_Rel>& iRel)
	{
	RelationalAlgebra::Util_Strim_Rel::sToStrim(iRel, w);
	return w;
	}

} // namespace ZooLib
