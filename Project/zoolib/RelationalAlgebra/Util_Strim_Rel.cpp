// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/RelationalAlgebra/Util_Strim_Rel.h"

#include "zoolib/ParseException.h"
#include "zoolib/Util_Chan_UTF.h"
#include "zoolib/Util_Chan_UTF_Operators.h"
#include "zoolib/Util_STL_set.h"
#include "zoolib/Util_string.h"
#include "zoolib/Util_ZZ_JSON.h"

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
	const ChanW_UTF& ww = pStrimW();
	ww << iExpr->DebugDescription();
	}

void Visitor::Visit_Expr_Rel_Calc(const ZP<Expr_Rel_Calc>& iExpr)
	{
	const ChanW_UTF& ww = pStrimW();
	ww << "Calc(";
	Util_Strim_RelHead::sWrite_PropName(ww, iExpr->GetColName());
	ww << " = /*Some function of*/";
	ww << ",";
	this->pWriteLFIndent();
	this->pToStrim(iExpr->GetOp0());
	ww << ")";
	}

void Visitor::Visit_Expr_Rel_Comment(const ZP<Expr_Rel_Comment>& iExpr)
	{
	const ChanW_UTF& ww = pStrimW();
	ww << "Comment(";
	Util_Chan_JSON::sWriteString(ww, iExpr->GetComment(), false);
	ww << ",";
	this->pWriteLFIndent();
	this->pToStrim(iExpr->GetOp0());
	ww << ")";
	}

void Visitor::Visit_Expr_Rel_Concrete(const ZP<Expr_Rel_Concrete>& iExpr)
	{
	const ChanW_UTF& ww = pStrimW();
	ww << "Concrete(" << iExpr->GetConcreteHead() << ")";
	}

void Visitor::Visit_Expr_Rel_Const(const ZP<Expr_Rel_Const>& iExpr)
	{
	const ChanW_UTF& ww = pStrimW();
	ww << "Const(";
	Util_Strim_RelHead::sWrite_PropName(ww, iExpr->GetColName());
	ww << ",";
	Util_ZZ_JSON::sWrite(ww, iExpr->GetVal().As<Val_ZZ>());
	ww << ")";
	}

void Visitor::Visit_Expr_Rel_Dee(const ZP<Expr_Rel_Dee>& iExpr)
	{ pStrimW() << "Dee()"; }

void Visitor::Visit_Expr_Rel_Difference(const ZP<Expr_Rel_Difference>& iExpr)
	{ this->pWriteBinary("Difference", iExpr); }

void Visitor::Visit_Expr_Rel_Dum(const ZP<Expr_Rel_Dum>& iExpr)
	{ pStrimW() << "Dum()"; }

void Visitor::Visit_Expr_Rel_Embed(const ZP<Expr_Rel_Embed>& iExpr)
	{
	const ChanW_UTF& ww = pStrimW();
	ww << "Embed(";
	this->pWriteLFIndent();
	this->pToStrim(iExpr->GetOp0());
	ww << ",";
	this->pWriteLFIndent();
	const RelHead theBoundNames = iExpr->GetBoundNames();
	if (sNotEmpty(theBoundNames))
		{
		ww << theBoundNames;
		ww << ",";
		this->pWriteLFIndent();
		}
	Util_Strim_RelHead::sWrite_PropName(ww, iExpr->GetColName());
	ww << " = ";
	++fIndent;
	this->pWriteLFIndent();
	this->pToStrim(iExpr->GetOp1());
	--fIndent;
	ww << ")";
	}

void Visitor::Visit_Expr_Rel_Intersect(const ZP<Expr_Rel_Intersect>& iExpr)
	{ this->pWriteBinary("Intersect", iExpr); }

void Visitor::Visit_Expr_Rel_Product(const ZP<Expr_Rel_Product>& iExpr)
	{ this->pWriteBinary("Product", iExpr); }

void Visitor::Visit_Expr_Rel_Project(const ZP<Expr_Rel_Project>& iExpr)
	{
	const ChanW_UTF& ww = pStrimW();
	ww << "Project(" << iExpr->GetProjectRelHead() << ",";
	this->pWriteLFIndent();
	this->pToStrim(iExpr->GetOp0());
	ww << ")";
	}

void Visitor::Visit_Expr_Rel_Rename(const ZP<Expr_Rel_Rename>& iExpr)
	{
	const ChanW_UTF& ww = pStrimW();
	ww << "Rename(";
	Util_Strim_RelHead::sWrite_PropName(ww, iExpr->GetNew());
	ww << "<--";
	Util_Strim_RelHead::sWrite_PropName(ww, iExpr->GetOld());
	ww << ",";
	this->pWriteLFIndent();
	this->pToStrim(iExpr->GetOp0());
	ww << ")";
	}

void Visitor::Visit_Expr_Rel_Restrict(const ZP<Expr_Rel_Restrict>& iExpr)
	{
	const ChanW_UTF& ww = pStrimW();
	ww << "Restrict(";
	this->pToStrim(iExpr->GetExpr_Bool());
	ww << ",";
	this->pWriteLFIndent();
	this->pToStrim(iExpr->GetOp0());
	ww << ")";
	}

void Visitor::Visit_Expr_Rel_Union(const ZP<Expr_Rel_Union>& iExpr)
	{ this->pWriteBinary("Union", iExpr); }

void Visitor::Visit_Expr_Rel_Search(const ZP<QueryEngine::Expr_Rel_Search>& iExpr)
	{
	const ChanW_UTF& ww = pStrimW();
	ww << "Search(";
	ww << iExpr->GetRelHead_Bound();
	ww << ",";
	this->pWriteLFIndent();
	Util_Strim_RelHead::sWrite_RenameWithOptional(ww,
		iExpr->GetRename(), iExpr->GetRelHead_Optional());
	ww << ",";
	this->pWriteLFIndent();
	this->pToStrim(iExpr->GetExpr_Bool());
	ww << ")";
	}

void Visitor::pWriteBinary(
	const string& iFunctionName, const ZP<Expr_Op2_T<Expr_Rel>>& iExpr)
	{
	const ChanW_UTF& ww = pStrimW();
	ww << iFunctionName << "(";

	this->pWriteLFIndent();
	this->pToStrim(iExpr->GetOp0());
	ww << ",";
	this->pWriteLFIndent();
	this->pToStrim(iExpr->GetOp1());
	ww << ")";
	}

// =================================================================================================
#pragma mark - RelationalAlgebra::Util_Strim_Rel::sToStrim

void sToStrim(const ChanW_UTF& iStrimW, const ZP<RelationalAlgebra::Expr_Rel>& iRel)
	{
	Options theOptions;
//	theOptions.fEOLString = "\n";
//	theOptions.fIndentString = "|\t";
	sToStrim(iStrimW, iRel, theOptions);
	}

void sToStrim_Parseable(const ChanW_UTF& iStrimW, const ZP<RelationalAlgebra::Expr_Rel>& iRel)
	{
	Options theOptions;
	theOptions.fEOLString = "";
	theOptions.fIndentString = "";
	sToStrim(iStrimW, iRel, theOptions);
	}

void sToStrim(const ChanW_UTF& iStrimW, const ZP<RelationalAlgebra::Expr_Rel>& iRel, const Options& iOptions)
	{ Visitor().ToStrim(iStrimW, iOptions, iRel); }

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

				ZP<Expr_Rel> childRel = sFromStrim(iChanRU);
				if (not childRel)
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

				if (NotQ<Val_ZZ> theValQ = Util_ZZ_JSON::sQRead(iChanRU))
					throw ParseException("Expected value as second param in Const");
				else
					result = sConst(*theColNameQ, theValQ->As<Val_DB>());
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
			ZP<Expr_Rel> outerRel = sFromStrim(iChanRU);
			if (not outerRel)
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

					ZP<Expr_Rel> innerRel = sFromStrim(iChanRU);
					if (not innerRel)
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

				ZP<Expr_Rel> childRel = sFromStrim(iChanRU);
				if (not childRel)
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

				ZP<Expr_Rel> childRel = sFromStrim(iChanRU);
				if (not childRel)
					throw ParseException("Expected Rel as second param in Rename");
				else
					result = sRename(childRel, theRenameQ->first, theRenameQ->second);
				}
			}
		else if (sEquali(*theNameQ, "Restrict"))
			{
			sSkip_WSAndCPlusPlusComments(iChanRU);
			ZP<Expr_Bool> theExpr = Util_Strim_Expr_Bool_ValPred::sQFromStrim(iChanRU);
			if (not theExpr)
				{ throw ParseException("Expected Expr_Bool param to Restrict"); }
			else
				{
				spRead_WSComma(iChanRU, " after ValPred in Restrict");

				ZP<Expr_Rel> childRel = sFromStrim(iChanRU);
				if (not childRel)
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

const ChanW_UTF& operator<<(const ChanW_UTF& ww, const ZP<RelationalAlgebra::Expr_Rel>& iRel)
	{
	RelationalAlgebra::Util_Strim_Rel::sToStrim(ww, iRel);
	return ww;
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
