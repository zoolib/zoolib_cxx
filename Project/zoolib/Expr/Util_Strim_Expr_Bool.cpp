/* -------------------------------------------------------------------------------------------------
Copyright (c) 2015 Andrew Green
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

#include "zoolib/Expr/Util_Strim_Expr_Bool.h"

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
		{
		return spParenthable(iCallable_Terminal, iChanRU);
		}
	else if (NotP<Expr_Bool> child = spExpression(iCallable_Terminal, iChanRU))
		{
		throw ParseException("Expected expression or terminal");
		}
	else
		{
		sSkip_WSAndCPlusPlusComments(iChanRU);

		if (not sTryRead_CP(')', iChanRU))
			throw ParseException("Expected close paren");

		return child;
		}
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

	if (NotP<Expr_Bool> child = spNotable(iCallable_Terminal, iChanRU))
		throw ParseException("Expected notable");
	else if (isNotted)
		return new Expr_Bool_Not(child);
	else
		return child;
	}

ZP<Expr_Bool> spOrable(
	const ZP<Callable_Terminal>& iCallable_Terminal,
	const ChanRU_UTF& iChanRU)
	{
	if (NotP<Expr_Bool> exprL = spAndable(iCallable_Terminal, iChanRU))
		{ return null; }
	else for (;;)
		{
		sSkip_WSAndCPlusPlusComments(iChanRU);

		if (not sTryRead_CP('&', iChanRU))
			return exprL;

		if (NotP<Expr_Bool> exprR = spAndable(iCallable_Terminal, iChanRU))
			throw ParseException("Expected operand after '&'");
		else
			exprL = sAnd(exprL, exprR);
		}
	}

ZP<Expr_Bool> spExpression(
	const ZP<Callable_Terminal>& iCallable_Terminal,
	const ChanRU_UTF& iChanRU)
	{
	if (NotP<Expr_Bool> exprL = spOrable(iCallable_Terminal, iChanRU))
		{ return null; }
	else for (;;)
		{
		sSkip_WSAndCPlusPlusComments(iChanRU);

		if (not sTryRead_CP('|', iChanRU))
			return exprL;

		if (NotP<Expr_Bool> exprR = spOrable(iCallable_Terminal, iChanRU))
			throw ParseException("Expected operand after '|'");
		else
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
