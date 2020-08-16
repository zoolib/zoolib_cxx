// Copyright (c) 2015 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Expr/Util_Strim_Expr_Bool.h"

#include "zoolib/ParseException.h"
#include "zoolib/Util_Chan_UTF.h"

namespace ZooLib {
namespace Util_Strim_Expr_Bool {

using namespace Util_Chan;

// =================================================================================================
#pragma mark - Helper functions (anonymous)

namespace { // anonymous

ZP<Expr_Bool> spExpression(
	const ZP<Callable_Terminal>& iCallable_Terminal,
	const ChanRU_UTF& iChanRU);

// -----

ZP<Expr_Bool> spParenthable(
	const ZP<Callable_Terminal>& iCallable_Terminal,
	const ChanRU_UTF& iChanRU)
	{
	if (sTryRead_String("TRUE", iChanRU))
		return sTrue();

	if (sTryRead_String("FALSE", iChanRU))
		return sFalse();

	if (ZP<Expr_Bool> child = sCall(iCallable_Terminal, iChanRU))
		return child;

	if (ZP<Expr_Bool> child = spExpression(iCallable_Terminal, iChanRU))
		return child;

	throw ParseException("Expected expression or terminal");
	}

ZP<Expr_Bool> spNotable(
	const ZP<Callable_Terminal>& iCallable_Terminal,
	const ChanRU_UTF& iChanRU)
	{
	sSkip_WSAndCPlusPlusComments(iChanRU);

	if (not sTryRead_CP('(', iChanRU))
		return spParenthable(iCallable_Terminal, iChanRU);

	ZP<Expr_Bool> child = spExpression(iCallable_Terminal, iChanRU);
	if (not child)
		throw ParseException("Expected expression or terminal");

	sSkip_WSAndCPlusPlusComments(iChanRU);

	if (not sTryRead_CP(')', iChanRU))
		throw ParseException("Expected close paren");

	return child;
	}

ZP<Expr_Bool> spAndable(
	const ZP<Callable_Terminal>& iCallable_Terminal,
	const ChanRU_UTF& iChanRU)
	{
	bool isNotted = false;
	for (;;)
		{
		sSkip_WSAndCPlusPlusComments(iChanRU);

		if (not sTryRead_CP('~', iChanRU))
			break;

		isNotted = ~isNotted;
		}

	ZP<Expr_Bool> child = spNotable(iCallable_Terminal, iChanRU);
	if (not child)
		throw ParseException("Expected notable");

	if (isNotted)
		return new Expr_Bool_Not(child);
	return child;
	}

ZP<Expr_Bool> spOrable(
	const ZP<Callable_Terminal>& iCallable_Terminal,
	const ChanRU_UTF& iChanRU)
	{
	ZP<Expr_Bool> exprL = spAndable(iCallable_Terminal, iChanRU);
	if (not exprL)
		return null;
	for (;;)
		{
		sSkip_WSAndCPlusPlusComments(iChanRU);

		if (not sTryRead_CP('&', iChanRU))
			return exprL;

		ZP<Expr_Bool> exprR = spAndable(iCallable_Terminal, iChanRU);
		if (not exprR)
			throw ParseException("Expected operand after '&'");

		exprL = sAnd(exprL, exprR);
		}
	}

ZP<Expr_Bool> spExpression(
	const ZP<Callable_Terminal>& iCallable_Terminal,
	const ChanRU_UTF& iChanRU)
	{
	ZP<Expr_Bool> exprL = spOrable(iCallable_Terminal, iChanRU);
	if (not exprL)
		return null;

	for (;;)
		{
		sSkip_WSAndCPlusPlusComments(iChanRU);

		if (not sTryRead_CP('|', iChanRU))
			return exprL;

		ZP<Expr_Bool> exprR = spOrable(iCallable_Terminal, iChanRU);
		if (not exprR)
			throw ParseException("Expected operand after '|'");

		exprL = sOr(exprL, exprR);
		}
	}

} // anonymous namespace

// =================================================================================================
#pragma mark - Util_Strim_Expr_Bool

ZP<Expr_Bool> sQFromStrim(
	const ZP<Callable_Terminal>& iCallable_Terminal,
	const ChanRU_UTF& iChanRU)
	{
	return spExpression(iCallable_Terminal, iChanRU);
	}

} // namespace Util_Strim_Expr_Bool
} // namespace ZooLib
