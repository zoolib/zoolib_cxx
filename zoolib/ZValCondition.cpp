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

#include "zoolib/ZValCondition.h"
#include "zoolib/ZValCondition_Priv.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * Explicit instantiations

template class ZValComparatorRep<ZVal_Expr>;
template class ZValComparatorRep_Simple<ZVal_Expr>;
template class ZValComparatorRep_StringContains<ZVal_Expr>;
template class ZValComparatorRep_SeqContains<ZVal_Expr>;
template class ZValComparatorRep_Regex<ZVal_Expr>;
template class ZValComparandRep<ZVal_Expr>;
template class ZValComparandRep_Const<ZVal_Expr>;
template class ZValComparandRep_Trail<ZVal_Expr>;
template class ZValComparandRep_Var<ZVal_Expr>;
template class ZValCondition_T<ZVal_Expr>;
template class ZValComparand_T<ZVal_Expr>;

// Forwarding calls.
ZValComparand CConst(const char* iVal)
	{ return CConst_T<ZVal_Expr>(iVal); }

ZValComparand CConst(const ZVal_Expr& iVal)
	{ return CConst_T<ZVal_Expr>(iVal); }

ZValComparand CName(const std::string& iName)
	{ return CName_T<ZVal_Expr>(iName); }

ZValComparand CTrail(const ZTrail& iTrail)
	{ return CTrail_T<ZVal_Expr>(iTrail); }

ZValComparand CVal()
	{ return CVal_T<ZVal_Expr>(); }

ZValComparand CVar(const std::string& iVarName)
	{ return CVar_T<ZVal_Expr>(iVarName); }

NAMESPACE_ZOOLIB_END
