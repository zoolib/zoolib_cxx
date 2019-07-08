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

#include "zoolib/RelationalAlgebra/Util_Strim_Rel.h"

#include "zoolib/ParseException.h"
#include "zoolib/Util_Any_JSON.h"
#include "zoolib/Util_Chan_UTF.h"
#include "zoolib/Util_Chan_UTF_Operators.h"
#include "zoolib/Util_STL_set.h"
#include "zoolib/Util_string.h"

#include "zoolib/RelationalAlgebra/GetRelHead.h"
#include "zoolib/RelationalAlgebra/Util_Strim_RelHead.h"

#include "zoolib/ValPred/Util_Strim_Expr_Bool_ValPred.h"

#include "zoolib/pdesc.h"
#if defined(ZMACRO_pdesc)
	#include "zoolib/StdIO.h"
#endif

namespace ZooLib {
namespace RelationalAlgebra {
namespace Util_Strim_Rel {

using namespace Util_STL;
using std::string;

// =================================================================================================
#pragma mark - RelationalAlgebra::Util_Strim_Rel::Visitor

void Visitor::Visit_Expr(const ZP<Expr>& iExpr)
	{
	const ChanW_UTF& w = pStrimW();
	w << iExpr->DebugDescription();
	}

void Visitor::Visit_Expr_Rel_Calc(const ZP<Expr_Rel_Calc>& iExpr)
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

void Visitor::Visit_Expr_Rel_Comment(const ZP<Expr_Rel_Comment>& iExpr)
	{
	const ChanW_UTF& w = pStrimW();
	w << "Comment(";
	Util_Chan_JSON::sWriteString(iExpr->GetComment(), false, w);
	w << ",";
	this->pWriteLFIndent();
	this->pToStrim(iExpr->GetOp0());
	w << ")";
	}

void Visitor::Visit_Expr_Rel_Concrete(const ZP<Expr_Rel_Concrete>& iExpr)
	{
	const ChanW_UTF& w = pStrimW();
	w << "Concrete(" << iExpr->GetConcreteHead() << ")";
	}

void Visitor::Visit_Expr_Rel_Const(const ZP<Expr_Rel_Const>& iExpr)
	{
	const ChanW_UTF& w = pStrimW();
	w << "Const(";
	Util_Strim_RelHead::sWrite_PropName(iExpr->GetColName(), w);
	w << ",";
	Util_Any_JSON::sWrite(iExpr->GetVal(), w);
	w << ")";
	}

void Visitor::Visit_Expr_Rel_Dee(const ZP<Expr_Rel_Dee>& iExpr)
	{ pStrimW() << "Dee()"; }

void Visitor::Visit_Expr_Rel_Difference(const ZP<Expr_Rel_Difference>& iExpr)
	{ this->pWriteBinary("Difference", iExpr); }

void Visitor::Visit_Expr_Rel_Dum(const ZP<Expr_Rel_Dum>& iExpr)
	{ pStrimW() << "Dum()"; }

void Visitor::Visit_Expr_Rel_Embed(const ZP<Expr_Rel_Embed>& iExpr)
	{
	const ChanW_UTF& w = pStrimW();
	w << "Embed(";
	this->pWriteLFIndent();
	this->pToStrim(iExpr->GetOp0());
	w << ",";
	this->pWriteLFIndent();
	const RelHead theBoundNames = iExpr->GetBoundNames();
	if (sNotEmpty(theBoundNames))
		{
		w << theBoundNames;
		w << ",";
		this->pWriteLFIndent();
		}
	Util_Strim_RelHead::sWrite_PropName(iExpr->GetColName(), w);
	w << " = ";
	++fIndent;
	this->pWriteLFIndent();
	this->pToStrim(iExpr->GetOp1());
	--fIndent;
	w << ")";
	}

void Visitor::Visit_Expr_Rel_Intersect(const ZP<Expr_Rel_Intersect>& iExpr)
	{ this->pWriteBinary("Intersect", iExpr); }

void Visitor::Visit_Expr_Rel_Product(const ZP<Expr_Rel_Product>& iExpr)
	{ this->pWriteBinary("Product", iExpr); }

void Visitor::Visit_Expr_Rel_Project(const ZP<Expr_Rel_Project>& iExpr)
	{
	const ChanW_UTF& w = pStrimW();
	w << "Project(" << iExpr->GetProjectRelHead() << ",";
	this->pWriteLFIndent();
	this->pToStrim(iExpr->GetOp0());
	w << ")";
	}

void Visitor::Visit_Expr_Rel_Rename(const ZP<Expr_Rel_Rename>& iExpr)
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

void Visitor::Visit_Expr_Rel_Restrict(const ZP<Expr_Rel_Restrict>& iExpr)
	{
	const ChanW_UTF& w = pStrimW();
	w << "Restrict(";
	this->pToStrim(iExpr->GetExpr_Bool());
	w << ",";
	this->pWriteLFIndent();
	this->pToStrim(iExpr->GetOp0());
	w << ")";
	}

void Visitor::Visit_Expr_Rel_Union(const ZP<Expr_Rel_Union>& iExpr)
	{ this->pWriteBinary("Union", iExpr); }

void Visitor::Visit_Expr_Rel_Search(const ZP<QueryEngine::Expr_Rel_Search>& iExpr)
	{
	const ChanW_UTF& w = pStrimW();
	w << "Search(";
	w << iExpr->GetRelHead_Bound();
	w << ",";
	this->pWriteLFIndent();
	Util_Strim_RelHead::sWrite_RenameWithOptional(
	iExpr->GetRename(), iExpr->GetRelHead_Optional(), w);
	w << ",";
	this->pWriteLFIndent();
	this->pToStrim(iExpr->GetExpr_Bool());
	w << ")";
	}

void Visitor::pWriteBinary(
	const string& iFunctionName, const ZP<Expr_Op2_T<Expr_Rel>>& iExpr)
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
#pragma mark - RelationalAlgebra::Util_Strim_Rel::sToStrim

void sToStrim(const ZP<RelationalAlgebra::Expr_Rel>& iRel, const ChanW_UTF& iStrimW)
	{
	Options theOptions;
//	theOptions.fEOLString = "\n";
//	theOptions.fIndentString = "|\t";
	sToStrim(iRel, theOptions, iStrimW);
	}

void sToStrim_Parseable(const ZP<RelationalAlgebra::Expr_Rel>& iRel, const ChanW_UTF& iStrimW)
	{
	Options theOptions;
	theOptions.fEOLString = "";
	theOptions.fIndentString = "";
	sToStrim(iRel, theOptions, iStrimW);
	}

void sToStrim(const ZP<RelationalAlgebra::Expr_Rel>& iRel, const Options& iOptions, const ChanW_UTF& iStrimW)
	{ Visitor().ToStrim(iOptions, iStrimW, iRel); }

// =================================================================================================
#pragma mark - RelationalAlgebra::Util_Strim_Rel::sFromStrim

ZP<Expr_Rel> sQFromStrim(const ChanRU_UTF& iChanRU)
	{
	try { return sFromStrim(iChanRU); }
	catch (...) {}
	return null;
	}

using namespace Util_Chan;
using std::pair;

typedef pair<ZP<Expr_Rel>,ZP<Expr_Rel>> RelPair;

static void spRead_WSComma(const ChanRU_UTF& iChanRU, const string8& iMessage)
	{
	sSkip_WSAndCPlusPlusComments(iChanRU);
	if (not sTryRead_CP(',', iChanRU))
		throw ParseException("Expected ','" + iMessage);
	}

static ZQ<RelPair> spQReadPair(const ChanRU_UTF& iChanRU, const string& iMessage)
	{
	sSkip_WSAndCPlusPlusComments(iChanRU);
	if (ZP<Expr_Rel> left = sQFromStrim(iChanRU))
		{
		spRead_WSComma(iChanRU, iMessage);
		if (ZP<Expr_Rel> right = sQFromStrim(iChanRU))
			return RelPair(left, right);
		}
	return null;
	}

ZP<Expr_Rel> sFromStrim(const ChanRU_UTF& iChanRU)
	{
	sSkip_WSAndCPlusPlusComments(iChanRU);
	if (NotQ<string8> theNameQ = Util_Chan_JSON::sQRead_PropName(iChanRU))
		{ return null; }
	else
		{
		sSkip_WSAndCPlusPlusComments(iChanRU);
		if (not sTryRead_CP('(', iChanRU))
			throw ParseException("Expected '(' after " + *theNameQ);

		sSkip_WSAndCPlusPlusComments(iChanRU);

		using Util_string::sEquali;

		ZP<Expr_Rel> result;

		if (false)
			{}
		else if (sEquali(*theNameQ, "Calc"))
			{
			ZAssert(false);
			}
		else if (sEquali(*theNameQ, "Comment"))
			{
			string theComment;
			if (not Util_Chan_JSON::sTryRead_JSONString(iChanRU, theComment))
				{
				throw ParseException("Expected string param to Comment");
				}
			else
				{
				spRead_WSComma(iChanRU, " after string in Comment");

				if (NotP<Expr_Rel> childRel = sFromStrim(iChanRU))
					throw ParseException("Expected Rel as second param in Comment");
				else
					result = sComment(childRel, theComment);
				}
			}
		else if (sEquali(*theNameQ, "Concrete"))
			{
			if (NotQ<ConcreteHead> theConcreteHeadQ = Util_Strim_RelHead::sQFromStrim_ConcreteHead(iChanRU))
				throw ParseException("Expected ConcreteHead param to Concrete");
			else
				result = sConcrete(*theConcreteHeadQ);
			}
		else if (sEquali(*theNameQ, "Const"))
			{
			if (NotQ<ColName> theColNameQ = Util_Strim_RelHead::sQRead_PropName(iChanRU))
				throw ParseException("Expected ColName as first param in Const");
			else
				{
				spRead_WSComma(iChanRU, " after ColName in Const");

				if (NotQ<Any> theValQ = Util_Any_JSON::sQRead(iChanRU))
					throw ParseException("Expected value as second param in Const");
				else
					result = sConst(*theColNameQ, *theValQ);
				}
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
			if (NotP<Expr_Rel> outerRel = sFromStrim(iChanRU))
				throw ParseException("Expected Rel as first param in Embed");
			else
				{
				spRead_WSComma(iChanRU, " after Rel in Embed");

				sSkip_WSAndCPlusPlusComments(iChanRU);

				const ZQ<RelHead> theBoundNamesQ = Util_Strim_RelHead::sQFromStrim_RelHead(iChanRU);
				if (theBoundNamesQ)
					{
					spRead_WSComma(iChanRU, " after BoundNames in Embed");

					sSkip_WSAndCPlusPlusComments(iChanRU);
					}

				if (NotQ<ColName> theColNameQ = Util_Strim_RelHead::sQRead_PropName(iChanRU))
					throw ParseException("Expected ColName as first param in Embed");
				else
					{
					sSkip_WSAndCPlusPlusComments(iChanRU);

					if (not sTryRead_CP('=', iChanRU))
						throw ParseException("Expected '='");

					sSkip_WSAndCPlusPlusComments(iChanRU);

					if (NotP<Expr_Rel> innerRel = sFromStrim(iChanRU))
						throw ParseException("Expected Rel as second param in Embed");
					else
						result = sEmbed(outerRel, sGet(theBoundNamesQ), *theColNameQ, innerRel);
					}
				}
			}
		else if (sEquali(*theNameQ, "Intersect"))
			{
			ZAssert(false);
			}
		else if (sEquali(*theNameQ, "Product"))
			{
			if (NotQ<RelPair> theQ = spQReadPair(iChanRU, "in Product"))
				throw ParseException("Expected Rel params to Product");
			else
				result = sProduct(theQ->first, theQ->second);
			}
		else if (sEquali(*theNameQ, "Project"))
			{
			if (NotQ<RelHead> theRelHeadQ = Util_Strim_RelHead::sQFromStrim_RelHead(iChanRU))
				throw ParseException("Expected RelHead param to Project");
			else
				{
				spRead_WSComma(iChanRU, " after RelHead in Project");

				if (NotP<Expr_Rel> childRel = sFromStrim(iChanRU))
					throw ParseException("Expected Rel as second param in Project");
				else
					result = sProject(childRel, *theRelHeadQ);
				}
			}
		else if (sEquali(*theNameQ, "Rename"))
			{
			if (NotQ<pair<ColName,ColName>> theRenameQ = Util_Strim_RelHead::sQFromStrim_Rename(iChanRU))
				{ throw ParseException("Expected Rename param to Rename"); }
			else
				{
				spRead_WSComma(iChanRU, " after Rename in Rename");

				if (NotP<Expr_Rel> childRel = sFromStrim(iChanRU))
					throw ParseException("Expected Rel as second param in Rename");
				else
					result = sRename(childRel, theRenameQ->first, theRenameQ->second);
				}
			}
		else if (sEquali(*theNameQ, "Restrict"))
			{
			sSkip_WSAndCPlusPlusComments(iChanRU);
			if (NotP<Expr_Bool> theExpr = Util_Strim_Expr_Bool_ValPred::sQFromStrim(iChanRU))
				{ throw ParseException("Expected Expr_Bool param to Restrict"); }
			else
				{
				spRead_WSComma(iChanRU, " after ValPred in Restrict");

				if (NotP<Expr_Rel> childRel = sFromStrim(iChanRU))
					throw ParseException("Expected Rel as second param in Restrict");
				else
					result = sRestrict(childRel, theExpr);
				}
			}
		else if (sEquali(*theNameQ, "Union"))
			{
			if (NotQ<RelPair> theQ = spQReadPair(iChanRU, "in Union"))
				throw ParseException("Expected Rel params to Union");
			else
				result = sUnion(theQ->first, theQ->second);
			}
		else if (sEquali(*theNameQ, "Search"))
			{
			ZAssert(false);
			}
		else
			{
			throw ParseException("Unexpected name: " + *theNameQ);
			}

		sSkip_WSAndCPlusPlusComments(iChanRU);
		if (not sTryRead_CP(')', iChanRU))
			throw ParseException("Expected ')' after " + *theNameQ);
		return result;
		}
	return null;
	}

} // namespace Util_Strim_Rel
} // namespace RelationalAlgebra

const ChanW_UTF& operator<<(const ChanW_UTF& w, const ZP<RelationalAlgebra::Expr_Rel>& iRel)
	{
	RelationalAlgebra::Util_Strim_Rel::sToStrim(iRel, w);
	return w;
	}

} // namespace ZooLib

// =================================================================================================
#pragma mark - pdesc

#if defined(ZMACRO_pdesc)

using namespace ZooLib;

ZMACRO_pdesc(const ZP<RelationalAlgebra::Expr_Rel>& iRel)
	{
	StdIO::sChan_UTF_Err << iRel << "\n";
	}

#endif // defined(ZMACRO_pdesc)
